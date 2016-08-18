
#include "StdAfx.h"
#include <assert.h>
#include "Viewport.h"
#include "UserData.h"
#include <algorithm>
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define DEFAULT_ASPECT		(4.0f / 3.0f)

CViewport::CViewport():
	m_x(0), 
	m_y(0),
	m_width(0), 
	m_height(0),
	m_lastWidth(0),
	m_lastHeight(0),
	m_camera(NULL),
	m_oculusHeight(0),
	m_selectedObject(NULL),
	MP_VECTOR_INIT(m_selectedObjects),
	m_lastGetCoordsStatus(GET_COORDS_NOTHING_UNDER_MOUSE)
{
	m_isPanoramaMode = false;
}

void CViewport::SetOffset(CVector2D& offset)
{
    m_x = (int)offset.x;
	m_y = (int)offset.y;
}

void CViewport::SetOffset(int x, int y)
{
	m_x = x;
	m_y = y;
}

CVector2D CViewport::GetOffset()const
{
	return CVector2D((float)m_x, (float)m_y);
}

void CViewport::GetOffset(int* x, int* y)
{
	*x = m_x;
	*y = m_y;
}

int CViewport::GetX()const
{
	return m_x;
}

int CViewport::GetY()const
{
	return m_y;
}

void CViewport::SetSize(CVector2D& size)
{
	m_width = (int)size.x;
	m_height = (int)size.y;

	assert(m_width >= 0);
	assert(m_height >= 0);
}

void CViewport::SetHeight(const int height)
{
	m_height = height;
}

void CViewport::SetOculusHeight(const int height)
{
	m_oculusHeight = height;
}

void CViewport::SetWidth(const int width)
{
	m_width = width;
}

void CViewport::SetSize(const int width, const int height)
{
	m_width = width;
	m_height = height;

	assert(m_width >= 0);
	assert(m_height >= 0);
}

CVector2D CViewport::GetSize()const
{
	return CVector2D((float)m_width, (float)m_height);
}

void CViewport::GetSize(int* width, int* height)
{
	*width = m_width;
	*height = m_height;
}

int CViewport::GetWidth()const
{
	return m_width;
}

int CViewport::GetHeight()const
{
	return m_height;
}

void CViewport::Bind(bool isSelectObjectClearNeeded)
{
	if ((0 == m_width) && (0 == m_height))
	{
		g->stp.SetViewport(m_x, m_y, g->stp.GetWidth(), g->stp.GetHeight());
		BindCamera(isSelectObjectClearNeeded, (float)g->stp.GetWidth(), (float)g->stp.GetHeight());
	}
	else
	{
		g->stp.SetViewport(m_x, m_y, m_width, m_height);
		BindCamera(isSelectObjectClearNeeded, (float)m_width, (float)m_height);
	}	
}

void CViewport::UpdateProjectionMatrix()
{
	m_lastWidth = m_width + 1;
}

void CViewport::BindCamera(bool isSelectObjectClearNeeded, float width, float height)
{
	float* matrix = NULL;
	if (m_camera)
	{
		g->cm.SetActiveCamera(m_camera);
		m_camera->Bind(width / height);		
		matrix = m_camera->GetMatrix();
	}

	if (isSelectObjectClearNeeded)
	{
		m_selectedObject = NULL;
	}

	
	if (!matrix)
	{
		GLFUNC(glGetDoublev)(GL_MODELVIEW_MATRIX, &m_modelview[0]);
	}
	else
	{
		for (int k = 0; k < 16; k++)
		{
			m_modelview[k] = (double)matrix[k];
		}
	}
	
	if ((m_lastWidth != m_width) || (m_lastHeight != m_height) || (g->rs.IsOculusRiftEnabled()))
	{
		GLFUNC(glGetDoublev)(GL_PROJECTION_MATRIX, &m_proj[0]);
		m_lastWidth = m_width;
		m_lastHeight = m_height;
	}
}

void CViewport::SetCamera(CCamera3D* camera)
{
	m_camera = camera;
}

CCamera3D* CViewport::GetCamera()const
{
	return m_camera;
}

/*myUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
    const GLdouble modelMatrix[16], 
    const GLdouble projMatrix[16],
    const GLint viewport[4],
    GLdouble *objx, GLdouble *objy, GLdouble *objz)
{
    double in[4];
    double out[4];

	CMatrix4D a, b;
	for (int i = 0; i < 16; i++)
	{
		a.m[i%4][(int)((i - i%4) / 4)] = modelMatrix[i];
		b.m[i%4][(int)((i - i%4) / 4)] = projMatrix[i];
	}

	CMatrix4D c = b * a;
	if (!c.Invert())
	{
		return GL_FALSE;
	}
    
    in[0]=winx;
    in[1]=winy;
    in[2]=winz;
    in[3]=1.0;

    // Map x and y from window coordinates 
    in[0] = (in[0] - viewport[0]) / viewport[2];
    in[1] = (in[1] - viewport[1]) / viewport[3];

    // Map to range -1 to 1
    in[0] = in[0] * 2 - 1;
    in[1] = in[1] * 2 - 1;
    in[2] = in[2] * 2 - 1;

	CVector4D tmp2((float)in[0], (float)in[1], (float)in[2], (float)in[3]);
	CVector4D tmp = c.TransformPoint(tmp2);

	out[0] = (double)tmp.x;
	out[1] = (double)tmp.y;
	out[2] = (double)tmp.z;
	out[3] = (double)tmp.w;
	
    if (out[3] == 0.0) return(GL_FALSE);
    out[0] /= out[3];
    out[1] /= out[3];
    out[2] /= out[3];
    *objx = out[0];
    *objy = out[1];
    *objz = out[2];
    return(GL_TRUE);
}*/

int CViewport::Get3DCoords(int mouseX, int mouseY, float* x, float* y, float* z)
{
	if ((mouseX < 0) || (mouseY < 0) || (mouseX >= m_width) || (mouseY >= m_height))
	{
		m_lastGetCoordsStatus = GET_COORDS_FAILED;
		return GET_COORDS_FAILED;
	}

	int viewport[4];	
	viewport[0] = m_x;
	viewport[1] = m_y;
	viewport[2] = m_width;
	viewport[3] = m_height;

	if (g->rs.IsOculusRiftEnabled())
	{
		if (mouseX > m_width / 2)
		{			
			viewport[0] += m_width / 2;
		}
		viewport[2] = m_width / 2;
		viewport[3] = m_oculusHeight;
	}
	
	mouseY = m_height - 1 - mouseY;
	
	float pixelZ = -3.0f;
	GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);

	if ((pixelZ == -3.0f) || (pixelZ == 0.0f))
	{
		m_lastGetCoordsStatus = GET_COORDS_UNSUPPORTED;
		return GET_COORDS_UNSUPPORTED;
	}

	double objX, objY, objZ;
	int res = GLFUNCR(gluUnProject(mouseX, mouseY, pixelZ, m_modelview, m_proj, viewport, &objX, &objY, &objZ));

	/*double objX2, objY2, objZ2;
	myUnProject(mouseX, mouseY, pixelZ, m_modelview, m_proj, viewport, &objX2, &objY2, &objZ2);*/

	if (1.0f == pixelZ)
	{
		m_lastGetCoordsStatus = GET_COORDS_NOTHING_UNDER_MOUSE;
		return GET_COORDS_NOTHING_UNDER_MOUSE;
	}

	if (1 == res)
	{
		*x = (float)objX;
		*y = (float)objY;
		*z = (float)objZ;

		m_lastGetCoordsStatus = GET_COORDS_OK;
		return GET_COORDS_OK;
	}
	else
	{
		m_lastGetCoordsStatus = GET_COORDS_FAILED;
		return GET_COORDS_FAILED;
	}
}

unsigned char CViewport::GetLastGetCoordsStatus()const
{
	return m_lastGetCoordsStatus;
}

void CViewport::GetZNearCoords(int mouseX, int mouseY, float& objX, float& objY, float& objZ)
{
	int viewport[4];	
	viewport[0] = m_x;
	viewport[1] = m_y;
	viewport[2] = m_width;
	viewport[3] = m_height;

	mouseY = m_height - 1 - mouseY;

	double _objX, _objY, _objZ;
	GLFUNC(gluUnProject)(mouseX, mouseY, 0.0f, m_modelview, m_proj, viewport, &_objX, &_objY, &_objZ);
	objX = (float)_objX;
	objY = (float)_objY;
	objZ = (float)_objZ;
}

bool CViewport::GetScreenCoords(float x, float y, float z, float* screenX, float* screenY)
{
	float tmp;

	return GetScreenCoords(x, y, z, screenX, screenY, &tmp);
}

bool CViewport::GetActualScreenCoords(float x, float y, float z, float* screenX, float* screenY, float* screenZ)
{
	int viewport[4];	
	viewport[0] = m_x;
	viewport[1] = m_y;
	viewport[2] = m_width;
	viewport[3] = m_height;

	double resX, resY, resZ;

	double proj[16];
	double modelview[16];

	GLFUNC(glGetDoublev)(GL_PROJECTION_MATRIX, &proj[0]);
	GLFUNC(glGetDoublev)(GL_MODELVIEW_MATRIX, &modelview[0]);

	int res = GLFUNCR(gluProject(x, y, z, modelview, proj, viewport, &resX, &resY, &resZ));

	if (1 == res)
	{
		*screenX = (float)resX;
		*screenY = (float)(m_height - resY);
		*screenZ = (float)resZ;

		return true;
	}
	else
	{
		return false;
	}
}

bool CViewport::GetScreenCoords(float x, float y, float z, float* screenX, float* screenY, float* screenZ)
{
	int viewport[4];	
	viewport[0] = m_x;
	viewport[1] = m_y;

	if (g->rs.IsOculusRiftEnabled())
	{		
		viewport[2] = m_width / 2;
		viewport[3] = m_oculusHeight;
	}
	else
	{
		viewport[2] = m_width;
		viewport[3] = m_height;
	}

	double resX, resY, resZ;

	int res = GLFUNCR(gluProject(x, y, z, m_modelview, m_proj, viewport, &resX, &resY, &resZ));

	if (1 == res)
	{
		*screenX = (float)resX;
		*screenY = (float)(m_height - resY);
		*screenZ = (float)resZ;

		return true;
	}
	else
	{
		return false;
	}
}

C3DObject* CViewport::GetObjectUnderMouse(bool groundOnly)
{
	if (!g->rs.GetBool(PRECISION_OBJECT_PICKING))
	if (g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		if (m_selectedObjects.size() > 1)
		{
			m_selectedObject = NULL;
	
			for (unsigned int k = 0; k < m_selectedObjects.size(); k++){
				if(groundOnly && !((int)(m_selectedObjects[k]->GetUserData(USER_DATA_LEVEL)) == LEVEL_GROUND))
					continue;
				if (m_queries[k].GetFragmentCount() != 0)
				{
					m_selectedObject = m_selectedObjects[k];				
				}
			}

			if (!m_selectedObject)
			{
				m_selectedObject = m_selectedObjects[0];
			}
		}
	}

	return m_selectedObject;
}

C3DObject* CViewport::GetObjectUnderMouse(std::vector<C3DObject *>& checkObjects)
{
	m_selectedObject = NULL;

	if (g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		for (unsigned int k = 0; k < m_selectedObjects.size(); k++)
		if (m_queries[k].GetFragmentCount() != 0)
		{
			std::vector<C3DObject *>::iterator it = std::find(checkObjects.begin(), checkObjects.end(), m_selectedObjects[k]);
			if (it != checkObjects.end())
			{
				m_selectedObject = m_selectedObjects[k];
			}			
		}				
	}

	return m_selectedObject;
}

CVector3D CViewport::GetSelected3DCoords()const
{
	return m_selected3d;
}

int CViewport::GetSelectedX()const
{
	return m_selectedX;
}

int CViewport::GetSelectedY()const
{
	return m_selectedY;
}

void CViewport::SelectObjectUnderMouse(float x, float y, float z, int mouseX, int mouseY, bool isHighPrecision)
{
	if ((mouseX < 0) || (mouseY < 0) || (mouseX >= m_width) || (mouseY >= m_height))
	{
		m_selectedObjects.clear();
		m_selectedObject = NULL;
		return;
	}

	m_selectedX = mouseX;
	m_selectedY = mouseY;
	m_selected3d.Set(x, y, z);

	mouseY = m_height - 1 - mouseY;

	m_selectedObjects.clear();

	if (!((g->rs.GetBool(PRECISION_OBJECT_PICKING)) && (isHighPrecision)))
	{
		std::vector<C3DObject*>::iterator iter = g->o3dm.GetLiveObjectsBegin();
		std::vector<C3DObject*>::iterator iterEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; iter != iterEnd; iter++)
		{	
			if (!(*iter)->IsVisibleNow())
			{
				continue;
			}			

			if ((int)(*iter)->GetUserData(USER_DATA_LEVEL) == LEVEL_CAMERA_COLLISION_ONLY)
			{
				continue;
			}

			if ((*iter)->IsInBoundingBox(x, y, z, 25.0f))
			{
				CLODGroup *pLodGroup = (*iter)->GetLODGroup();
				if (pLodGroup != NULL && !pLodGroup->IsUnpickable())
				{
					m_selectedObjects.push_back(*iter);
				}				
			}
		}

		if (m_selectedObjects.size() == 0)
		{
			m_selectedObject = NULL;
			return;
		}
		else if (m_selectedObjects.size() == 1)
		{
			m_selectedObject = m_selectedObjects[0];
			return;
		}
	}

	GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	GLFUNC(glDepthFunc)(GL_LEQUAL);
	GLFUNC(glEnable)(GL_SCISSOR_TEST);
	GLFUNC(glScissor)(mouseX - 1, mouseY - 1, 3, 3);	

	std::vector<C3DObject*>::iterator it = m_selectedObjects.begin();
	std::vector<C3DObject*>::iterator itEnd = m_selectedObjects.end();

	GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
	g->stp.SetState(ST_ZTEST, true);

	int detectPathID = 0;

	if ((g->rs.GetBool(PRECISION_OBJECT_PICKING)) && (isHighPrecision))
	{
		m_selectedObject = NULL;
		detectPathID = 1;
		float pixelZ, oldPixelZ;
		GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);
		oldPixelZ = pixelZ;

		it = g->o3dm.GetLiveObjectsBegin();
		itEnd = g->o3dm.GetLiveObjectsEnd();

		for ( ; it != itEnd; it++)
		if (((*it)->IsVisibleNow()) && ((int)(*it)->GetUserData(USER_DATA_LEVEL) != LEVEL_CAMERA_COLLISION_ONLY))
		{
			(*it)->GetLODGroup()->RenderObjectSafely(*it);			

			GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);

			if (pixelZ != oldPixelZ)
			{				
				m_selectedObject = *it;
			}

			oldPixelZ = pixelZ;
		}		
		}
	else if (g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		detectPathID = 2;
		int k = 0;

		for ( ; it != itEnd; it++, k++)
		{
			m_queries[k].StartQuery();
			
			(*it)->GetLODGroup()->RenderObjectSafely(*it);			

			m_queries[k].EndQuery();
		}
	}
	else
	{
		detectPathID = 3;
		float pixelZ, oldPixelZ;
		GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);
		oldPixelZ = pixelZ;

		for ( ; it != itEnd; it++)
		{
			(*it)->GetLODGroup()->RenderObjectSafely(*it);			

			GLFUNC(glReadPixels)(GLint(mouseX), GLint(mouseY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixelZ);

			if (pixelZ != oldPixelZ)
			{
				m_selectedObject = *it;
			}

			oldPixelZ = pixelZ;
		}
	}

	if ((g->rs.GetBool(PRECISION_OBJECT_PICKING)) && (isHighPrecision))
	{
		if (2 == detectPathID)
		{
			m_selectedObject = GetObjectUnderMouse();
		}
		if (m_selectedObject)
		{
			GLFUNC(glClear)(GL_DEPTH_BUFFER_BIT);
			m_lastSelectedTextureID = m_selectedObject->GetLODGroup()->DetectObjectMaterial(m_selectedObject, mouseX, mouseY);
		}
	}

	GLFUNC(glDisable)(GL_SCISSOR_TEST);
	GLFUNC(glDepthFunc)(GL_LESS);
	GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

int CViewport::GetSelectedTextureID()const
{
	return m_lastSelectedTextureID;
}

float CViewport::GetAspect()const
{
	if (g->rs.GetBool(CAVE_SUPPORT))
	{
		return 1.0f;
	}

	if (g->ne.GetWorkMode() == WORK_MODE_SNAPSHOTRENDER)
	{
		if (m_isPanoramaMode)
			return 1.0f;
		else
			return DEFAULT_ASPECT;
	}

	if ((0 == m_width) && (0 == m_height))
	{
		return ((float)g->stp.GetWidth() / (float)g->stp.GetHeight());
	}
	else
	{
		return ((float)m_width / (float)m_height);
	}
}

void CViewport::setPanoramaMode(bool isPanorama)
{
	m_isPanoramaMode = isPanorama;
}

CViewport::~CViewport()
{
}