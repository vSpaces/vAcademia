
#pragma once

#include "CommonEngineHeader.h"

#include "Camera.h"
#include "Vector2D.h"
#include "3DObject.h"
#include "CommonObject.h"
#include "OcclusionQuery.h"

#define ELEMENTS_IN_4D_MATRIX		16
#define MAX_QUERIES					100

#define	GET_COORDS_UNSUPPORTED				-1
#define	GET_COORDS_FAILED					0
#define	GET_COORDS_OK						1
#define GET_COORDS_NOTHING_UNDER_MOUSE		2

class CViewport : public CCommonObject
{
public:
	CViewport();
	~CViewport();

	void SetOffset(CVector2D& offset);
	void SetOffset(int x, int y);
	CVector2D GetOffset()const;
	void GetOffset(int* x, int* y);
	int GetX()const;
	int GetY()const;

	void UpdateProjectionMatrix();

	void SetSize(CVector2D& size);
	void SetSize(const int width, const int height);
	void SetHeight(const int height);
	void SetOculusHeight(const int height);
	void SetWidth(const int width);	
	CVector2D GetSize()const;
	void GetSize(int* width, int* height);
	int GetWidth()const;
	int GetHeight()const;
	float GetAspect()const;

	void SetCamera(CCamera3D* camera);
	CCamera3D* GetCamera()const;

	int Get3DCoords(int mouseX, int mouseY, float* x, float* y, float* z);
	bool GetScreenCoords(float x, float y, float z, float* screenX, float* screenY);
	bool GetScreenCoords(float x, float y, float z, float* screenX, float* screenY, float* screenZ);

	bool GetActualScreenCoords(float x, float y, float z, float* screenX, float* screenY, float* screenZ);

	void GetZNearCoords(int mouseX, int mouseY, float& objX, float& objY, float& objZ);

	CVector3D GetSelected3DCoords()const;
	int GetSelectedX()const;
	int GetSelectedY()const;

	void SelectObjectUnderMouse(float x, float y, float z, int mouseX, int mouseY, bool isHighPrecision);
	C3DObject* GetObjectUnderMouse(bool groundOnly = false);
	C3DObject* GetObjectUnderMouse(std::vector<C3DObject *>& checkObjects);

	void Bind(bool isSelectObjectClearNeeded = false);
	void BindCamera(bool isSelectObjectClearNeeded = false, float width = 0, float height = 0);

	int GetSelectedTextureID()const;

	unsigned char GetLastGetCoordsStatus()const;

	void setPanoramaMode(bool isPanorama);

private:
	int m_lastSelectedTextureID;

	int m_oculusHeight;

	int m_x, m_y;
	int m_width, m_height;
	int m_lastWidth, m_lastHeight;
	int m_selectedX, m_selectedY;

	unsigned int m_ztextureID;

	bool m_isPanoramaMode;

	char m_lastGetCoordsStatus;

	CCamera3D* m_camera;

	CVector3D m_selected3d;

	double m_modelview[ELEMENTS_IN_4D_MATRIX];
	double m_proj[ELEMENTS_IN_4D_MATRIX];

	MP_VECTOR<C3DObject *> m_selectedObjects;
	C3DObject* m_selectedObject;

	COcclusionQuery m_queries[MAX_QUERIES];
};