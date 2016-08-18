#include "StdAfx.h"
#include "TinyXML.h"
#include <UserData.h>
#include "LODGroupManager.h"
#include "HDRPostEffect.h"
#include "GlobalSingletonStorage.h"
#include "PhysicsObject.h"
#include "HelperFunctions.h"
#include "cal3d/memory_leak.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_CAMERA_VECTOR_DELTA	1.0f
#define MAX_CAMERA_ANGLE_DELTA	2.0f

CLODGroupManager::CLODGroupManager():
	MP_VECTOR_INIT(m_privelegedObjects),
	MP_VECTOR_INIT(m_unpickables),
	m_transparentObjectsCount(0),
	m_alphatestObjectsCount(0),	
	m_lodScale(1.0f),
	m_pitch(0),
	m_roll(0),
	m_yaw(0)
{
	ReserveMemory(5000);
	m_lastEyePos.Set(0.0f, 0.0f, 0.0f);
	m_lastLookAt.Set(0.0f, 0.0f, 0.0f);
	m_privelegedObjects.reserve(10);	
}

void CLODGroupManager::SetLODScale(const float lodScale)
{
	float lastLodScaleSq = m_lodScaleSq;

	if (1.0f == lodScale)
	{
		m_lodScale = (float)g->rs.GetInt(NORMAL_LOD_SCALE) / 100.0f;		
	}
	else
	{
		m_lodScale = lodScale;		
	}

	m_lodScaleSq = m_lodScale * m_lodScale;

	if (lastLodScaleSq != m_lodScaleSq)
	{
		std::vector<CLODGroup *>::iterator it = GetLiveObjectsBegin();
		std::vector<CLODGroup *>::iterator itEnd = GetLiveObjectsEnd();
		for ( ; it != itEnd; it++)
		{
			(*it)->NeedToUpdate();
		}
	}
}

void CLODGroupManager::UpdateAll()
{
	std::vector<CLODGroup *>::iterator it = GetLiveObjectsBegin();
	std::vector<CLODGroup *>::iterator itEnd = GetLiveObjectsEnd();
	for ( ; it != itEnd; it++)
	{
		(*it)->Update(true);
	}

	m_lastEyePos.Set(0, 0, 0);
}

void CLODGroupManager::DrawAll()
{
	if (!g->cm.GetActiveCamera())
	{
		return;
	}

	stat_allObjectCount = g->o3dm.GetLiveObjectsCount();

	bool isCameraChanged = IsCameraChanged();	

	std::vector<CLODGroup *>::iterator it = GetLiveObjectsBegin();
	std::vector<CLODGroup *>::iterator itEnd = GetLiveObjectsEnd();
	for ( ; it != itEnd; it++)
	{
		(*it)->Update(isCameraChanged);
		(*it)->Draw();
	}

	if (isCameraChanged)
	{
		CCamera3D* cam = g->cm.GetActiveCamera();
		m_lastEyePos = cam->GetEyePosition();
		m_lastLookAt = cam->GetLookAt();
		cam->GetAdditionalRotation(m_yaw, m_pitch, m_roll);
	}
}

bool CLODGroupManager::IsCameraChanged()const
{	
	CCamera3D* cam = g->cm.GetActiveCamera();
	CVector3D r1 = m_lastEyePos;
	r1 -= cam->GetEyePosition();
	CVector3D r2 = m_lastLookAt;
	r2 -= cam->GetLookAt();
	bool isCameraChanged = ((r1.GetLengthSq() > MAX_CAMERA_VECTOR_DELTA) || (r2.GetLengthSq() > MAX_CAMERA_VECTOR_DELTA));

	if (!isCameraChanged)
	if (g->rs.IsOculusRiftEnabled())
	{
		float yaw, roll, pitch;
		cam->GetAdditionalRotation(yaw, roll, pitch);
		float yawDelta = fabsf(yaw - m_yaw) * 180.0f / (float)M_PI;
		float pitchDelta = fabsf(pitch - m_pitch) * 180.0f / (float)M_PI;
		float rollDelta = fabsf(roll - m_roll) * 180.0f / (float)M_PI;
		isCameraChanged = ((yawDelta > MAX_CAMERA_ANGLE_DELTA) || (pitchDelta > MAX_CAMERA_ANGLE_DELTA) || (rollDelta > MAX_CAMERA_ANGLE_DELTA));
	}

	return isCameraChanged;
}

void CLODGroupManager::DrawAllWithoutUpdate()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CLODGroup* lodGroup = GetObjectPointer(i);

		lodGroup->Draw();
	}
}

void CLODGroupManager::OnNewFrame()
{
	m_transparentObjectsCount = 0;
	m_alphatestObjectsCount = 0;
	m_privelegedObjects.clear();
}

int CompareTransparent(const void* a, const void* b)
{
  return ((STransparentObject*)b)->dist -((STransparentObject*)a)->dist;
}

void CLODGroupManager::AddTransparentObject(C3DObject* const obj3d)
{
	if ((int)obj3d->GetUserData(USER_DATA_LEVEL) == LEVEL_SEA)
	{
		m_privelegedObjects.push_back(obj3d);		
		return;
	}

	if (m_transparentObjectsCount < MAX_TRANSPARENT_OBJECTS)
	{
		m_transparentObjects[m_transparentObjectsCount].obj3d = obj3d;
		m_transparentObjects[m_transparentObjectsCount].dist = (unsigned int)obj3d->GetDistSq();
		m_transparentObjectsCount++;
	}
}

void CLODGroupManager::AddAlphaTestObject(C3DObject* const obj3d)
{
	if (m_alphatestObjectsCount < MAX_ALPHATEST_OBJECTS)
	{
		m_alphatestObjects[m_alphatestObjectsCount].obj3d = obj3d;
		m_alphatestObjects[m_alphatestObjectsCount].dist = (unsigned int)obj3d->GetDistSq();
		m_alphatestObjectsCount++;
	}
}

void CLODGroupManager::ClearPrivelegedObjects()
{
	m_privelegedObjects.clear();
}

void CLODGroupManager::DrawAllTransparent()
{
	qsort(m_transparentObjects, m_transparentObjectsCount, sizeof(STransparentObject), CompareTransparent);

	g->stp.SetState(ST_BLEND, true);

	for (int i = 0; i < m_transparentObjectsCount; i++)
	{
		m_transparentObjects[i].obj3d->GetLODGroup()->RenderObject(m_transparentObjects[i].obj3d);
	}

	if (m_privelegedObjects.size() > 0)
	{
		CHDRPostEffect* postEffect = (CHDRPostEffect *)g->scm.GetActiveScene()->GetPostEffect();
		if (postEffect)
		{
			postEffect->BakeRefractionTexture();
		}

		std::vector<C3DObject *>::iterator it = m_privelegedObjects.begin();
		std::vector<C3DObject *>::iterator itEnd = m_privelegedObjects.end();

		for ( ; it != itEnd; it++)
		{
			(*it)->GetLODGroup()->RenderObject(*it);
		}
	}

	g->stp.SetState(ST_BLEND, false);
}

int CompareAlphaTest(const void* a, const void* b)
{
  return ((SAlphaTestObject*)b)->dist - ((SAlphaTestObject*)a)->dist;
}

void CLODGroupManager::DrawAllAlphaTest()
{
	qsort(m_alphatestObjects, m_alphatestObjectsCount, sizeof(SAlphaTestObject), CompareAlphaTest);

	g->mr.SetMode(MODE_RENDER_ALPHATEST);

	for (int i = 0; i < m_alphatestObjectsCount; i++)
	{
		m_alphatestObjects[i].obj3d->GetLODGroup()->RenderObject(m_alphatestObjects[i].obj3d);
	}
}

void CLODGroupManager::DrawAllUnpickable()
{
	g->mr.SetMode(MODE_RENDER_ALL);

	std::vector<CLODGroup *>::iterator it = m_unpickables.begin();
	std::vector<CLODGroup *>::iterator itEnd = m_unpickables.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->Update();
		(*it)->Draw();
	}
}

void CLODGroupManager::MakeUnpickable(CLODGroup* const lodGroup)
{
	g->lod.UnManageObject(lodGroup->GetID());	

	std::vector<CLODGroup *>::iterator it = m_unpickables.begin();
	std::vector<CLODGroup *>::iterator itEnd = m_unpickables.end();

	for ( ; it != itEnd; it++)
		if (*it == lodGroup)
		{
			return;
		}

	m_unpickables.push_back(lodGroup);
	lodGroup->SetUnpickable(true);
}

void CLODGroupManager::MakePickable(CLODGroup* const lodGroup)
{
	lodGroup->SetUnpickable(false);

	std::vector<CLODGroup *>::iterator it = m_unpickables.begin();
	std::vector<CLODGroup *>::iterator itEnd = m_unpickables.end();

	for ( ; it != itEnd; it++)
	if (*it == lodGroup)
	{
		m_unpickables.erase(it);
		g->lod.ManageObject(lodGroup);
		return;
	}	
}

void CLODGroupManager::LoadAll()
{
	TiXmlDocument doc("config\\lodgroups\\index.xml");
	//bool isLoaded = doc.LoadFile();
	bool isLoaded = g->rl->LoadXml( (void*)&doc, L"config\\lodgroups\\index.xml");

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading lod descriptions failed...");
		return;
	}

	TiXmlNode* groups = doc.FirstChild();

	if ((groups->NoChildren()) || (groups == NULL))
	{
		g->lw.WriteLn("[WARNING] Found 0 lod descriptions...");
		return;
	}

	TiXmlNode* group = groups->FirstChild();
		
	while (group != NULL)
	{
		TiXmlElement* elem = group->ToElement();

		if (elem)
		if (elem->GetText())
		{
			std::string groupFileName = elem->GetText();
			
			LoadLodGroup(groupFileName);
		}

		group = groups->IterateChildren(group);
	}
}

void CLODGroupManager::LoadLodGroup(const std::string& groupFileName)
{
	std::string fullGroupFileName = "config\\lodgroups\\" + groupFileName;
	TiXmlDocument doc(fullGroupFileName.c_str());
	//bool isLoaded = doc.LoadFile();
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fullGroupFileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading " + groupFileName + "failed...");
		return;
	}

	TiXmlNode* lodgroup = doc.FirstChild();
	if (!lodgroup)
	{
		g->lw.WriteLn("[ERROR] Loading " + groupFileName + "failed...");
		return;
	}
	if (!lodgroup->ToElement()->Attribute("name"))
	{
		g->lw.WriteLn("[ERROR] Loading " + groupFileName + "failed...");
		return;
	}
	std::string name = lodgroup->ToElement()->Attribute("name");		

	TiXmlNode* level = lodgroup->FirstChild("level");
	if (!level)
	{
		g->lw.WriteLn("[ERROR] Loading " + groupFileName + "failed...");
		return;
	}

	int id = AddObject(name);
	CLODGroup* lodGroupObj = GetObjectPointer(id);

	if (lodgroup->ToElement()->Attribute("collisionbits"))
	{	
		std::string collisionbits = lodgroup->ToElement()->Attribute("collisionbits");		
		unsigned long bits = (unsigned long)_atoi64(collisionbits.data());
		lodGroupObj->SetCollisionBits(bits);
	}

	while (level != NULL)
	{
		if (!level->ToElement()->Attribute("type"))
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}
		std::string type = level->ToElement()->Attribute("type");		

		if (!level->ToElement()->Attribute("shadow"))
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}
		std::string shadow = level->ToElement()->Attribute("shadow");		

		if (!level->ToElement()->Attribute("maxdist"))
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}
		std::string maxdistStr = level->ToElement()->Attribute("maxdist");		
		float maxDist = StringToFloat(maxdistStr.c_str());

		if (!level->ToElement()->Attribute("scale"))
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}
		std::string scaleStr = level->ToElement()->Attribute("scale");		
		float scale = StringToFloat(scaleStr.c_str());
		
		int shadowType = SHADOWS_NONE;

		if ("stencil" == shadow)
		{
			shadowType = SHADOWS_STENCIL;
		}
		if ("shadowmap" == shadow)
		{
			shadowType = SHADOWS_SHADOWMAP;
		}

		float shadowPower = 1.5f;

		if (level->ToElement()->Attribute("shadowpower"))
		{
			shadowPower = StringToFloat(level->ToElement()->Attribute("shadowpower"));
		}
		
		CVector3D shadowSource(0, 0, 0);

		if (level->ToElement()->Attribute("shadowsource"))
		{
			shadowSource = StrToVector3D(level->ToElement()->Attribute("shadowsource"));
		}
		
		TiXmlNode* livephaze = level->FirstChild();
		if (!livephaze)
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}

		std::vector<std::string> modelNames;
		std::vector<std::string> textureNames;
		std::vector<int> liveRanges;
		int livePhazeCount = 0;
		
		while (livephaze != NULL)
		{
			livePhazeCount++;
			if ("model3d" == type)
			{
				if (!livephaze->ToElement()->Attribute("model"))
				{
					g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
					return;
				}
				std::string model = livephaze->ToElement()->Attribute("model");
				modelNames.push_back(model);

				int liveRange = 0;
				if (livephaze->ToElement()->Attribute("liverange"))
				{
					liveRange = rtl_atoi(livephaze->ToElement()->Attribute("liverange"));
				}
				liveRanges.push_back(liveRange);
			}
			else
			{
				if (!livephaze->ToElement()->Attribute("texture"))
				{
					g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
					return;
				}
				std::string texture = livephaze->ToElement()->Attribute("texture");				
				textureNames.push_back(texture);

				int liveRange = 0;
				if (livephaze->ToElement()->Attribute("liverange"))
				{
					liveRange = rtl_atoi(livephaze->ToElement()->Attribute("liverange"));
				}
				liveRanges.push_back(liveRange);
			}

			livephaze = level->IterateChildren(livephaze);		
		}

		if ("model3d" == type)
		{
			lodGroupObj->AddModelAsLODLevel(&modelNames[0], shadowType, scale, maxDist);
		}

		level = lodgroup->IterateChildren("level", level);		
	}

	TiXmlNode* physicsmodel = lodgroup->FirstChild("physicsmodel");
	if (physicsmodel)
	{
		if (!physicsmodel->ToElement()->Attribute("type"))
		{
			g->lw.WriteLn("[ERROR] Loading ", groupFileName, "failed...");
			return;
		}

		std::string physicstype = physicsmodel->ToElement()->Attribute("type");		
		if (physicstype == "box")
		{
			lodGroupObj->SetPhysicsType(PHYSICS_OBJECT_TYPE_BOX);
		}
		else if (physicstype == "sphere")
		{
			lodGroupObj->SetPhysicsType(PHYSICS_OBJECT_TYPE_SPHERE);
		}
		else if (physicstype == "plane")
		{
				lodGroupObj->SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_PLANE);
		}
		else if (physicstype == "static_box")
		{
			lodGroupObj->SetPhysicsType(PHYSICS_OBJECT_TYPE_STATIC_BOX);
		}

	}
	lodGroupObj->Finalize();
}

void CLODGroupManager::Clear3DObjLinks()
{
	for (int i = 0; i < GetCount(); i++)
	{
		CLODGroup* lodGroup = GetObjectPointer(i);
		lodGroup->Clear3DObjLinks();
	}
}

CLODGroupManager::~CLODGroupManager()
{
	std::vector<CLODGroup *>::iterator it = m_unpickables.begin();
	std::vector<CLODGroup *>::iterator itEnd = m_unpickables.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}
}
