
#include "stdafx.h"
#include "oculus.h"
#include <vector>
#include "Service\Service_NetClient.h"
#include <Xinput.h>

using namespace std;

#pragma comment(lib, "nmath.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")

#ifdef _DEBUG
	#pragma comment(lib, "libovrd.lib")
#else
	#pragma comment(lib, "libovr.lib")
#endif

#define THRESHOLD 7000

WCHAR *GetWC(const char *c)
{
    const size_t cSize = strlen(c) + 1;
    WCHAR* wc = new WCHAR[cSize];
    mbstowcs (wc, c, cSize);
    return wc;
}

bool ItOculusMonitor(DISPLAY_DEVICE const& dd) {
	return wcsstr(dd.DeviceID, L"RTD2205") ||
		wcsstr(dd.DeviceID, L"CVT0003") ||
		wcsstr(dd.DeviceID, L"MST0030") ||
		wcsstr(dd.DeviceID, L"OVR00");
}

bool TryEnableOculusMonitor(COculus::InitError &initError) {
	DISPLAY_DEVICE dd, ddm;
	int notActiveRiftMonitors = 0;
	int activeRiftMonitors = 0;
	int activeMonitors = 0;
	for(int i = 0;
		(ZeroMemory(&dd, sizeof(dd)), dd.cb = sizeof(dd),
		EnumDisplayDevices(0, i, &dd, 0)) != 0;  i++ )
	{
		for(int j = 0;
			(ZeroMemory(&ddm, sizeof(ddm)), ddm.cb = sizeof(ddm),
			EnumDisplayDevices(dd.DeviceName, j, &ddm, 0)) != 0;  j++ )
		{
			bool active = ddm.StateFlags & DISPLAY_DEVICE_ACTIVE;
			if (active) {
				activeMonitors++;
			}
			if (ItOculusMonitor(ddm)) {
				if (active) {
					activeRiftMonitors++;
				} else {
					notActiveRiftMonitors++;
				}
			}
		}
	}

	if (notActiveRiftMonitors + activeRiftMonitors == 0) {
		initError = IOculusRiftSupport::HMD_NOT_DETECTED;
		return false;
	}
	if (activeMonitors < 3 && activeRiftMonitors > 0) {
		initError == IOculusRiftSupport::NOT_SUPPORTED_MONITOR_CLONED;
	} else {
		if (notActiveRiftMonitors > 0)
			initError = IOculusRiftSupport::MONITOR_NOT_ACTIVE;
	}
	return false;
}

bool IsDisplayCloned(const WCHAR* name) {
	DISPLAY_DEVICE dd, ddm;
	for(int i = 0;
		(ZeroMemory(&dd, sizeof(dd)), dd.cb = sizeof(dd),
		EnumDisplayDevices(0, i, &dd, 0)) != 0;  i++ )
	{
		bool isHereOurMonitor = false;
		int count = 0;
		for(int j = 0;
			(ZeroMemory(&ddm, sizeof(ddm)), ddm.cb = sizeof(ddm),
			EnumDisplayDevices(dd.DeviceName, j, &ddm, 0)) != 0;  j++ )
		{
			isHereOurMonitor |= !wcscmp(ddm.DeviceName, name);
			count++;
		}
		if (isHereOurMonitor) {
			return count > 1;
		}
	}
	return false;
}

bool IsDisplayCloned(const char* name) {
	WCHAR* wname = GetWC(name);
	bool cloned = IsDisplayCloned(wname);
	delete [] wname;
	return cloned;
}

bool GetMonitorParams(const WCHAR* name, int& w, int& h, int& x, int& y) {
	DISPLAY_DEVICE dd, ddm;
	for(int i = 0;
		(ZeroMemory(&dd, sizeof(dd)), dd.cb = sizeof(dd),
		EnumDisplayDevices(0, i, &dd, 0)) != 0;  i++ )
	{
		bool isHereOurMonitor = false;
		for(int j = 0;
			(ZeroMemory(&ddm, sizeof(ddm)), ddm.cb = sizeof(ddm),
			EnumDisplayDevices(dd.DeviceName, j, &ddm, 0)) != 0;  j++ )
		{
			isHereOurMonitor |= !wcscmp(ddm.DeviceName, name);
		}
		if (isHereOurMonitor) {			
			DEVMODE dm;
			ZeroMemory(&dm, sizeof(dm));
			dm.dmSize = sizeof(dm);
			if (EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm, 0) == FALSE)
				EnumDisplaySettingsEx(dd.DeviceName, ENUM_REGISTRY_SETTINGS, &dm, 0);
			w = dm.dmPelsWidth;
			h = dm.dmPelsHeight;
			x = dm.dmPosition.x;
			y = dm.dmPosition.y;
			return true;
		}
	}
	return false;
}

bool GetMonitorParams(const char* name, int& w, int& h, int& x, int& y) {
	WCHAR* wname = GetWC(name);
	bool res = GetMonitorParams(wname, w, h, x, y);
	delete [] wname;
	return res;
}

COculus::COculus() :
	m_HMD(nullptr)
{ }

COculus::~COculus()
{
	if (m_HMD)
    {
        ovrHmd_Destroy(m_HMD);
        m_HMD = nullptr;
    }
	ovr_Shutdown();
}
 
bool COculus::Start(InitError &initError)
{
	ovrBool result = ovr_Initialize();
	if (!result) {
		initError = FAIL_INIT;
		return false;
	}	
	
	m_HMD = ovrHmd_Create(0);

	if (!m_HMD) {
		OVR::Service::NetClient* netClient = OVR::Service::NetClient::GetInstance();
		if (!netClient->IsConnected(false, false)) {
			initError = FAIL_ON_CONNECT_TO_RUNTIME_SERVER;
			return false;
		}

		if (TryEnableOculusMonitor(initError)) {
			m_HMD = ovrHmd_Create(0);
		}
		if (!m_HMD) {
			return false;
		}
		
	} else {
		if (IsDisplayCloned(m_HMD->DisplayDeviceName)) {
			initError = NOT_SUPPORTED_MONITOR_CLONED;
			return false;
		}
	}

	if (OVR::System::DirectDisplayEnabled()) {
		initError = NOT_SUPPORTED_DIRECT_MODE;
		return false;
	}

	result = ovrHmd_ConfigureTracking(m_HMD, 
			ovrTrackingCap_Orientation |
			ovrTrackingCap_MagYawCorrection |
			ovrTrackingCap_Position, 0);

	for (int eye = 0; eye < 2; eye++)
	{
		m_eyeRenderDesc[eye] = ovrHmd_GetRenderDesc(m_HMD, (ovrEyeType)eye, m_HMD->DefaultEyeFov[eye]);
		/*ovrSizei recommendedSize = ovrHmd_GetFovTextureSize(m_HMD, (ovrEyeType)eye, m_HMD->DefaultEyeFov[eye], 1.0f);
		m_eyeRenderDesc[eye].DistortedViewport.Size = recommendedSize;*/
	}

	initError = NONE;
	return true;
}
 
bool COculus::GetSensorPos(CVector3D& pos, float& yaw, float& pitch, float& roll) const
{
	yaw = 0.0f;
	pitch = 0.0f;
	roll = 0.0f;

	ovrTrackingState ts = ovrHmd_GetTrackingState(m_HMD, ovr_GetTimeInSeconds());
	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		OVR::Posef pose = ts.HeadPose.ThePose;
		pos.x = 0; pos.y = 0; pos.z = 0; //TODO: get position
		pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
		return true;
	}

	return false;
}

void COculus::SetLeftEye(bool isLeftEye) {
	m_eye = (isLeftEye) 
		? ovrEye_Left 
		: ovrEye_Right;

	int x, y, w, h;
	GetViewportParams(x, y, w, h);			                 
    glViewport(x, y, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glMultMatrixf(GetViewAdjustMatrix());
    glMultMatrixf(GetProjectionMatrix());

    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity();
}

void COculus::GetViewportParams(int& x, int& y, int& width, int& height)
{
	ovrRecti& viewport = m_eyeRenderDesc[m_eye].DistortedViewport;
	x = viewport.Pos.x;
	y = viewport.Pos.y;
	width = viewport.Size.w;
	height = viewport.Size.h;
}

bool COculus::GetDesktopParams(int& wResolution, int& hResolution, int& displayX, int& displayY)
{
	/*
	Этот способ дает неверные координаты
	wResolution = m_HMD->Resolution.w;
	hResolution = m_HMD->Resolution.h;
	displayX = m_HMD->WindowsPos.x;
	displayY = m_HMD->WindowsPos.y;
	*/

	//приходиться снова обращаться к дьяволу за помощью
	return GetMonitorParams(m_HMD->DisplayDeviceName, wResolution, hResolution, displayX, displayY);
} 

void COculus::BeginRender()
{
	ovrHmd_BeginFrameTiming(m_HMD, 0);
}

void COculus::EndRender() {
	ovrHmd_EndFrameTiming(m_HMD);
}

float* COculus::GetViewAdjustMatrix()
{
	OVR::Vector3f offset = m_eyeRenderDesc[m_eye].HmdToEyeViewOffset;
	m_viewAdjust.Identity();
	m_viewAdjust.Translation(offset);
	m_viewAdjust = m_viewAdjust.Transposed();
	return m_viewAdjust.M[0];
} 

float* COculus::GetProjectionMatrix()
{
	float f = 125000.0f;
	float n = 10.0f;
	ovrFovPort fov = m_eyeRenderDesc[m_eye].Fov;
	m_projMatrix = ovrMatrix4f_Projection(fov, n, f, true);
	m_projMatrix = m_projMatrix.Transposed();
	return m_projMatrix.M[0];
}

void COculus::GetShaderParams(ShaderParams* params)
{
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrEyeRenderDesc& eyeRenderDesc = m_eyeRenderDesc[eye];

		ovrVector2f uvScaleOffset[2];
		ovrHmd_GetRenderScaleAndOffset(
			eyeRenderDesc.Fov,
			eyeRenderDesc.DistortedViewport.Size, 
			eyeRenderDesc.DistortedViewport,
			uvScaleOffset);

		ovrMatrix4f timeWarpMatrices[2];
		ovrPosef eyePos = ovrHmd_GetHmdPosePerEye(m_HMD, (ovrEyeType)eye);
		ovrHmd_GetEyeTimewarpMatrices(m_HMD, (ovrEyeType)eye, eyePos, timeWarpMatrices);

		for (int i = 0; i < 2; ++i)
		{
			params[eye].uvScaleOffset[i].x = uvScaleOffset[i].x;
			params[eye].uvScaleOffset[i].y = uvScaleOffset[i].y;
			memcpy(
				&params[eye].timeWarpMatrices[i].m[0][0], 
				&((OVR::Matrix4f)timeWarpMatrices[i]).Transposed().M[0][0], 
				sizeof(float) * 16);
		}

		/*if (m_HMD->Type == ovrHmd_DK1) {
			params[eye].uvScaleOffset[0].y *= 0.85;
			params[eye].uvScaleOffset[1].y -= 0.075;			
		}*/
	}
}

IOculusRiftSupport::DistortionMesh* COculus::GetDistortionMesh() {
	m_distortionMesh = new DistortionMesh;

	for (int eye = 0; eye < 2; ++eye)
	{
		ovrDistortionMesh mesh;
        ovrHmd_CreateDistortionMesh(
			m_HMD, (ovrEyeType)eye, m_eyeRenderDesc[eye].Fov,
            ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp,
            &mesh);

		int dVertex = 0, dIndex = 0, index, indexNew;
		if (eye == 0)
		{
			m_distortionMesh->VertexCount = mesh.VertexCount * 2;
			m_distortionMesh->IndexCount = mesh.IndexCount * 2;
			m_distortionMesh->pVertexData = new DistortionVertex[m_distortionMesh->VertexCount];
			m_distortionMesh->pIndexData = new unsigned short[m_distortionMesh->IndexCount];
		}
		else
		{
			dVertex = mesh.VertexCount;
			dIndex = mesh.IndexCount;
		}

		for (int i = 0; i < mesh.IndexCount; ++i)
		{
			unsigned short index = mesh.pIndexData[i];
			m_distortionMesh->pIndexData[dIndex + i] = dVertex + index;			
		}
		for (int i = 0; i < mesh.VertexCount; ++i)
		{
			const ovrDistortionVertex& vertex = mesh.pVertexData[i];
			DistortionVertex& vertexNew = m_distortionMesh->pVertexData[dVertex + i];
			vertexNew.ScreenPosNDC = CVector2D(vertex.ScreenPosNDC.x, vertex.ScreenPosNDC.y);
			vertexNew.TimeWarpFactor = vertex.TimeWarpFactor;
			vertexNew.VignetteFactor = vertex.VignetteFactor;
			vertexNew.EyeType = eye;
			vertexNew.TanEyeAnglesR = CVector2D(vertex.TanEyeAnglesR.x, -vertex.TanEyeAnglesR.y);
			vertexNew.TanEyeAnglesG = CVector2D(vertex.TanEyeAnglesG.x, -vertex.TanEyeAnglesG.y);
			vertexNew.TanEyeAnglesB = CVector2D(vertex.TanEyeAnglesB.x, -vertex.TanEyeAnglesB.y);
		}

		ovrHmd_DestroyDistortionMesh(&mesh);
	}

	return m_distortionMesh;
}

void COculus::DestroyDistortionMesh() {
	delete[] m_distortionMesh->pVertexData;
	delete[] m_distortionMesh->pIndexData;
	delete m_distortionMesh;
}