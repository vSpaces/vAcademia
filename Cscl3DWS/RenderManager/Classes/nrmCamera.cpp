// nrmCamera.cpp: implementation of the nrmCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <userdata.h>
#include "nrmCamera.h"
#include "nrm3DObject.h"
#include "HelperFunctions.h"
#include "CameraController.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

#include "nrm3DGroup.h"

bool StrToFloat(std::string & str, double & num)
{
	float tmp;
	if (sscanf(str.c_str(), "%f", &tmp))
	{
		num = tmp;
		return true;
	}
	return false;
}

nrmCamera::nrmCamera(mlMedia* apMLMedia): 
	nrmObject(apMLMedia),
	m_cameraController(NULL),
	m_camera(NULL)
{
}

CCameraController* nrmCamera::GetCameraController()
{
	return m_cameraController;
}

void nrmCamera::OnSetRenderManager()
{
	int cameraID = g->cm.AddObject(IntToStr(g->cm.GetCount()));
	m_camera = g->cm.GetObjectPointer(cameraID);
	m_camera->SetUserData(USER_DATA_NRMOBJECT, this);
}

bool nrmCamera::LoadCamera(ifile* file)
{
	ml3DPosition up;
	ml3DPosition at;
	ml3DPosition eye;
	double fov;

	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();

	int size = file->get_file_size();
	char* str = MP_NEW_ARR(char, size + 1);
	file->read((unsigned char *)str, size);
	str[size] = 0;
	std::string xml = str;
	StringReplace(xml, "<?xml version=\"1.0\" encoding=\"ASCII\"?>", "");
	MP_DELETE_ARR(str);

	TiXmlDocument doc;
	doc.Parse(xml.c_str());

	TiXmlNode* camera = doc.FirstChild("camera");

	if (!camera)
		return false;

	if ((camera->ToElement()->Attribute("eye_x")) &&
		(camera->ToElement()->Attribute("eye_y")) &&
		(camera->ToElement()->Attribute("eye_z")) &&

		(camera->ToElement()->Attribute("at_x")) &&
		(camera->ToElement()->Attribute("at_y")) &&
		(camera->ToElement()->Attribute("at_z")))
	{
		std::string eye_x = camera->ToElement()->Attribute("eye_x");
		std::string eye_y = camera->ToElement()->Attribute("eye_y");
		std::string eye_z = camera->ToElement()->Attribute("eye_z");

		std::string at_x = camera->ToElement()->Attribute("at_x");
		std::string at_y = camera->ToElement()->Attribute("at_y");
		std::string at_z = camera->ToElement()->Attribute("at_z");

		if (StrToFloat(eye_x, eye.x) && StrToFloat(eye_y, eye.y) &&	StrToFloat(eye_z, eye.z) &&
			StrToFloat(at_x, at.x) && StrToFloat(at_y, at.y) && StrToFloat(at_z, at.z))
		{
			if( pmlCamera)
			{
				pmlCamera->SetDest(at);
//				pmlCamera->SetEye(eye); // требуется добавить метод для скриптовой камеры
			}
			m_camera->SetLookAt((float)at.x, (float)at.y, (float)at.z);
			m_camera->SetEyePosition((float)eye.x, (float)eye.y, (float)eye.z);
		}
	}
	else
	{
		return false;
	}

	if ((camera->ToElement()->Attribute("up_x")) &&
		(camera->ToElement()->Attribute("up_y")) &&
		(camera->ToElement()->Attribute("up_z")))
	{
		std::string up_x = camera->ToElement()->Attribute("up_x");
		std::string up_y = camera->ToElement()->Attribute("up_y");
		std::string up_z = camera->ToElement()->Attribute("up_z");

		if (StrToFloat(up_x, up.x) && StrToFloat(up_y, up.y) && StrToFloat(up_z, up.z))
		{
			if( pmlCamera)
			{
//				pmlCamera->SetUp(up); // требуется добавить метод для скриптовой камеры
			}
			m_camera->SetUp((float)up.x, (float)up.y, (float)up.z);
		}

	}
	if (camera->ToElement()->Attribute("fov"))
	{
		std::string strFov = camera->ToElement()->Attribute("fov");

		if (StrToFloat(strFov, fov))
		{
			if( pmlCamera)
			{
				pmlCamera->SetFOV((int)fov);
			}
			m_camera->SetFov((float)fov);
		}
	}

	return true;
}
// загрузка нового ресурса
bool nrmCamera::SrcChanged()
{
	// комментарю, т.к. иначе синхронный доступ к интернет-ресурсу.
	return false;

	USES_CONVERSION;

	const wchar_t* pwcSrc=mpMLMedia->GetILoadable()->GetSrc();
	if(!pwcSrc) 
	{
		return false;
	}

	ifile* file = gRM->resLib->GetResFile(W2A(pwcSrc));
	if (!file)
	{
		return false;
	}

	if (LoadCamera(file))
	{
		gRM->resLib->DeleteResFile( file);
		return true;
	}

	gRM->resLib->DeleteResFile( file);
	return false;
}

// реализация moI3DObject
ml3DPosition nrmCamera::GetPos(){
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
#pragma warning(push)
#pragma warning(disable : 4239)
	CVector3D& eye = m_camera->GetEyePosition();
#pragma warning(pop)
	pos.x = eye.x;
	pos.y = eye.y;
	pos.z = eye.z;
	return pos;
}

ml3DPosition nrmCamera::GetDest(){
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
#pragma warning(push)
#pragma warning(disable : 4239)
	CVector3D& at = m_camera->GetLookAt();
#pragma warning(pop)
	pos.x = at.x;
	pos.y = at.y;
	pos.z = at.z;
	return pos;
}

ml3DPosition nrmCamera::GetDestOculus(){
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
#pragma warning(push)
#pragma warning(disable : 4239)
	CVector3D atOculus = m_camera->GetExtractedLookAt();
#pragma warning(pop)
	pos.x = atOculus.x;
	pos.y = atOculus.y;
	pos.z = atOculus.z;
	return pos;
}

ml3DPosition nrmCamera::GetUp(){
	ml3DPosition pos;
	ML_INIT_3DPOSITION(pos);
#pragma warning(push)
#pragma warning(disable : 4239)
	CVector3D up = m_camera->GetUp();
#pragma warning(pop)
	pos.x = up.x;
	pos.y = up.y;
	pos.z = up.z;
	return pos;
}

void nrmCamera::PosChanged(ml3DPosition &pos)
{
	m_camera->SetEyePosition((float)pos.x, (float)pos.y, (float)pos.z);
}

void nrmCamera::RotationChanged(ml3DRotation &/*rot*/)
{
	// do nothing
}

float nrmCamera::GetZLevel()
{
	return m_cameraTarget.GetAddZ();
}

ml3DScale nrmCamera::GetScale(){
	ml3DScale scl;
	ML_INIT_3DSCALE(scl);
	// ??
	scl.x = 1.0f;
	scl.y = 1.0f;
	scl.z = 1.0f;
	return scl;
}

void nrmCamera::ScaleChanged(ml3DScale &/*scl*/)
{
	// do nothing
}

ml3DRotation nrmCamera::GetRotation(){
	ml3DRotation rot;
	ML_INIT_3DROTATION(rot);
	// ??
	rot.x = 0.0f;
	rot.y = 0.0f;
	rot.z = 1.0f;
	rot.a = 0.0f;
	return rot;
}

void nrmCamera::VisibleChanged()
{
	// do nothing
}

void nrmCamera::SetDefaultCam()
{
	CVector3D up(0.0f, 0.0f, 1.0f);
    CVector3D eye(0.0f, 0.0f, 0.0f);
	CVector3D at(300.0f, -100.0f, -144.0f);

	m_camera->Set(up, eye, at);
}

void nrmCamera::onKeyDown()
{
}

void nrmCamera::onKeyUp()
{
}

CCameraController*	nrmCamera::GetMovingController()
{
	if (!m_cameraController)
	{
		m_cameraController = MP_NEW(CCameraController);
		m_pRenderManager->RegisterController(m_cameraController);
		m_cameraController->SetCamera(m_camera);	
	}
	
	return m_cameraController;
}

void nrmCamera::LinkToChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	mlMedia* pmlLink = pmlCamera->GetLinkTo();
	if (!pmlLink) 
	{
		m_cameraTarget.SetViewTarget(NULL);
		GetMovingController()->SetCameraTarget(&m_cameraTarget);
	}
	else
	{
		nrm3DObject* po = (nrm3DObject*)pmlLink->GetSafeMO();
		if ((((unsigned)po)&~3)!=0)
		{
			m_cameraTarget.SetViewTarget(po->m_obj3d);
			GetMovingController()->SetCameraTarget(&m_cameraTarget);
		}
	}

	if (GetMLMedia())
	{
		if (GetMLMedia()->GetBooleanProp("firstView"))
		{
			m_camera->SetUserData(USER_DATA_TYPE, (void *)1);
		}
	}
}

void nrmCamera::FOVChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	if (!pmlCamera)	
	{
		return;
	}

	m_camera->SetFov((float)pmlCamera->GetFOV());
}

void nrmCamera::FixedPositionChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	if (!pmlCamera)	
	{
		return;
	}

	GetMovingController()->SetFixedPosition(pmlCamera->GetFixedPosition());
}

void nrmCamera::DestChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	if (!pmlCamera)	
	{
		return;
	}

	ml3DPosition pos = pmlCamera->GetDest();
	m_camera->SetLookAt((float)pos.x, (float)pos.y, (float)pos.z);
}

void nrmCamera::DestOculusChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	if (!pmlCamera)	
	{
		return;
	}

	ml3DPosition pos = pmlCamera->GetDestOculus();
	CVector3D look(pos.x, pos.y, pos.z);
	m_camera->SetOculusLookAt(look);
}

void nrmCamera::UpChanged()
{
	mlICamera*	pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	if (!pmlCamera)	
	{
		return;
	}

	ml3DPosition pos = pmlCamera->GetUp();
	m_camera->SetUp((float)pos.x, (float)pos.y, (float)pos.z);
}

void nrmCamera::ResetDestOculus()
{
	m_camera->ResetOculusLookAt();
}

void nrmCamera::MaxLinkDistanceChanged()
{
	mlICamera* pmlCamera = (mlICamera*)GetMLMedia()->GetICamera();
	float dMaxLinkDistance = (float)pmlCamera->GetMaxLinkDistance();
	
	if ((m_camera) && (GetMovingController()))
	{
		GetMovingController()->OnMaxLinkDistanceChanged(dMaxLinkDistance);
	}
}

bool nrmCamera::Get3DPropertiesInfo(mlSynchData &/*aData*/)
{ 
	return true;
}

bool nrmCamera::Get3DProperty(char* /*apszName*/,char* /*apszSubName*/, mlOutString &/*sVal*/)
{
	return false;
}

bool nrmCamera::Set3DProperty(char* /*apszName*/,char* /*apszSubName*/,char* /*apszVal*/)
{
	return true;
}

int nrmCamera::moveTo(ml3DPosition /*aPos3D*/, int /*aiDuaration*/, bool /*abCheckCollisions*/)
{
	return -1;
}

void nrmCamera::SetNearPlane(float nearPlane)
{
	m_camera->SetNearPlane(nearPlane);
}

int nrmCamera::destMoveTo(ml3DPosition /*aPos3D*/, int /*aiDuaration*/, bool /*abCheckCollisions*/)
{
	return -1;
}

void nrmCamera::Rotate(int aiDirection, int aiTime)
{
	if (!m_cameraController || !m_camera)
		return;

#define MUL_KOEF 0.02f

	m_cameraController->Rotate(aiDirection, (float)aiTime * MUL_KOEF);
}

void nrmCamera::Move(int aiDirection, int aiTime)
{
	if (!m_cameraController || !m_camera)
		return;

	m_cameraController->Move(aiDirection, aiTime);
}

ml3DPosition nrmCamera::getAbsolutePosition()
{
	ATLASSERT(FALSE);
	return GetPos();
}

ml3DRotation nrmCamera::getAbsoluteRotation()
{
	ATLASSERT(FALSE);
	return GetRotation();
}

void nrmCamera::PropIsSet()
{
}

void nrmCamera::OnCameraChangedTo(nrmCamera* cam)
{
	if ((cam) && (m_cameraController))
	{
		m_cameraController->OnCameraChangedTo(cam->GetMovingController());
	}
}

void nrmCamera::ClearOffset()
{
	m_cameraTarget.SetViewOffset(CVector3D(0, 0, 0));
}

bool nrmCamera::HasOffset()
{
	CVector3D ofs = m_cameraTarget.GetViewOffset();

	return ((ofs.x != 0.0f) || (ofs.y != 0.0f) || (ofs.z != 0.0f));
}

void nrmCamera::CorrectPosition()
{
	if (m_cameraController)
	{
		//m_cameraController->Update(0);
	}
}

void nrmCamera::PlayTrackFile(const wchar_t* apwcFilePath)
{
	if( apwcFilePath == NULL || *apwcFilePath == 0)
	{
		GetMovingController();
		m_cameraController->StopTrackPlaying();
		return;
	}

	res::resIResource* m_res = NULL;
	if( OMS_OK == GetRenderManager()->GetResourceManager()->OpenResource( apwcFilePath, m_res, RES_LOCAL))
	{
		unsigned long	size = m_res->GetSize();
		if( size > 0)
		{
			byte* trackFileData = MP_NEW_ARR(byte, size);
			if( trackFileData)
			{
				unsigned int errorCode = 0;
				unsigned int readed = m_res->Read( trackFileData, size, &errorCode);
				if( readed == size)
				{
					GetMovingController();
					m_cameraController->PlayCameraTrack( trackFileData, size);
				}
				MP_DELETE_ARR(trackFileData);
			}
		}
		m_res->Close();
		m_res = NULL;
	}
}

nrmCamera::~nrmCamera()
{
	if (m_cameraController)
	{
		m_pRenderManager->UnregisterController(m_cameraController);
		MP_DELETE(m_cameraController);
	}
}