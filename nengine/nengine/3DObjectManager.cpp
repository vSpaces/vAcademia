
#include "StdAfx.h"
#include "TinyXML.h"
#include "RotationPack.h"
#include "HelperFunctions.h"
#include "3DObjectManager.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#ifdef DEBUG
	#include "HierarchyAttachTest.h"
	#include "SkeletonAttachTest.h"
#endif

C3DObjectManager::C3DObjectManager()
{
	ReserveMemory(12000);

	//
#ifdef DEBUG
	CHierarchyAttachTest test;
	test.Execute();

	CSkeletonAttachTest testSkeleton;
	testSkeleton.Execute();
#endif
}

void C3DObjectManager::LoadWorld(std::wstring fileName)
{
	USES_CONVERSION;
	TiXmlDocument doc( W2A(fileName.c_str()));
	bool isLoaded = g->rl->LoadXml( (void*)&doc, fileName);

	if (!isLoaded)
	{
		g->lw.WriteLn("[ERROR] Loading world failed...");
		return;
	}

	TiXmlNode* world = doc.FirstChild();

	if ((world->NoChildren()) || (world == NULL))
	{
		g->lw.WriteLn("[WARNING] Found 0 objects in the world...");
		return;
	}

	TiXmlNode* object = world->FirstChild();

	int i = 0;

	while (object != NULL)
	{
		i++;

		if ((!object->ToElement()->Attribute("type")) ||
			(!object->ToElement()->Attribute("name")))
		{
			g->lw.WriteLn("[ERROR] Loading world failed...");
			return;
		}

		std::string type = object->ToElement()->Attribute("type");
		std::string name = object->ToElement()->Attribute("name");
		std::string physics ="";
		if (object->ToElement()->Attribute("physics"))
		{
			physics = object->ToElement()->Attribute("physics");
		}		

		if ("lodgroup" == type)
		{
			if ((!object->ToElement()->Attribute("x")) ||
				(!object->ToElement()->Attribute("y")) ||
				(!object->ToElement()->Attribute("z")))
			{
				g->lw.WriteLn("[ERROR] Loading world failed...");
				return;
			}
	
			float x = StringToFloat(object->ToElement()->Attribute("x"));
			float y = StringToFloat(object->ToElement()->Attribute("y"));
			float z = StringToFloat(object->ToElement()->Attribute("z"));

			TiXmlNode* _rot = NULL;
			TiXmlNode* rot = object->FirstChild();
			while ((rot) && ((!rot->ToElement()->GetText()) || (strcmp(rot->ToElement()->GetText(), "rotationpack"))))
			{
				_rot = rot;
				rot = object->IterateChildren(rot);
			}
			
			CRotationPack* rotation = NULL;

			if (_rot)
			{
				MP_NEW_V(rotation, CRotationPack, _rot);				
			}
			else
			{
				rotation = MP_NEW(CRotationPack);
			}
			
			int lodGroupID = g->lod.GetObjectNumber(name);
			CLODGroup* lodGroup = g->lod.GetObjectPointer(lodGroupID);

			bool hasPhysics = false;
			if (physics == "true")
			{
				hasPhysics = true;
			}

			AddObject3D(lodGroup, x, y, z, rotation, hasPhysics);
		}		

		object = world->IterateChildren(object);		
	}
}

void C3DObjectManager::AddObject3D(C3DObject* obj, CLODGroup* lodGroup, float x, float y, float z,
		CRotationPack* rotation, bool /*hasPhysics*/)
{
	obj->SetRotation(rotation);
	obj->SetCoords(x, y, z);
	obj->SetLODGroup(lodGroup);
	obj->SetCoords(x, y, z);
	obj->SetRotation(rotation);
	//obj->SetPhysics(hasPhysics);

	if (x > m_maxX)
	{
		m_maxX = x;
	}

	if (x < m_minX)
	{
		m_minX = x;
	}

	if (y > m_maxY)
	{
		m_maxY = y;
	}

	if (y < m_minY)
	{
		m_minY = y;
	}

	if (z > m_maxZ)
	{
		m_maxZ = z;
	}

	if (z < m_minZ)
	{
		m_minZ = z;
	}
}

int C3DObjectManager::AddObject3D(CLODGroup* lodGroup, float x, float y, float z,
		CRotationPack* rotation, bool hasPhysics)
{
	int objectID = AddObject("obj_" + IntToStr(GetCount()));
	C3DObject* obj = GetObjectPointer(objectID);

	AddObject3D(obj, lodGroup, x, y, z, rotation, hasPhysics);

	return objectID;
}

void C3DObjectManager::GetWorldSize(float* x1, float* y1, float* z1,
									float* x2, float* y2, float* z2)
{
	*x1 = m_minX;
	*x2 = m_maxX;

	*y1 = m_minY;
	*y2 = m_maxY;

	*z1 = m_minZ;
	*z2 = m_maxZ;
}

C3DObjectManager::~C3DObjectManager()
{

}