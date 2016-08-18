
#include "StdAfx.h"
#include "OcclusionQuery.h"
#include "GlobalSingletonStorage.h"
#include "miniglut.h"

#define QUERY_NOT_COMPLETED		0
#define QUERY_COMPLETED			1

int g_simultaneouslyOQCount = 0;

COcclusionQuery::COcclusionQuery():
	m_queryID(0xFFFFFFFF),
	m_fragmentCount(0),
	m_type(OCCLUSION_POINT),
	m_status(QUERY_NOT_COMPLETED),
	m_isResultAvailable(false),
	m_isQueryStarted(false),
	m_x(0.0f), 
	m_y(0.0f), 
	m_z(0.0f),
	m_xSize(0.0f), 
	m_ySize(0.0f), 
	m_zSize(0.0f)
{
	g->occlm.RegisterQuery(this);	
}

void COcclusionQuery::SetQueryPoint(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;

	m_type = OCCLUSION_POINT;
}

void COcclusionQuery::SetQueryBoundingBox(float x, float y, float z, float xSize, float ySize, float zSize)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_xSize = xSize;
	m_ySize = ySize;
	m_zSize = zSize;

	m_type = OCCLUSION_BOUNDING_BOX;
}

void COcclusionQuery::DoQuery()
{
	if (!g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		return;
	}

	if (g_simultaneouslyOQCount >= 1)
	{
		return;	
	}

	if (g->ne.isIconicAndIntel)
	{
		m_status = QUERY_NOT_COMPLETED;
		return;
	}

	bool isCheckOcclusion = !g->rs.GetBool(SHOW_CHECK_OCCLUSION_TARGETS);

	if (isCheckOcclusion)
	{
		StartQuery();

		GLFUNC(glColorMask)(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	}
	else
	{
		g->stp.SetState(ST_ZTEST, false);
		g->stp.PrepareForRender();
	}

	switch (m_type)
	{
	case OCCLUSION_POINT:
		if (!isCheckOcclusion)
		{
			g->stp.SetColor(255, 255, 255);
		}
		GLFUNC(glPointSize)(25);
		GLFUNC(glDisable)(GL_TEXTURE_2D);
		GLFUNC(glBegin)(GL_POINTS);
		GLFUNC(glVertex3f)(m_x, m_y, m_z);
		GLFUNC(glEnd)();
		GLFUNC(glEnable)(GL_TEXTURE_2D);
		break;

	case OCCLUSION_BOUNDING_BOX:
		g->stp.PushMatrix();
		g->stp.SetState(ST_CULL_FACE, true);
		GLFUNC(glTranslatef)(m_x, m_y, m_z);
		g->stp.PrepareForRender();
		GLFUNC(glScalef)(m_xSize, m_ySize, m_zSize);
		miniglut::glutSolidCube(1.0f);
		g->stp.PopMatrix();
		break;
	}

	if (isCheckOcclusion)
	{
		GLFUNC(glColorMask)(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		
		EndQuery();	
	}
	else
	{
		g->stp.SetState(ST_ZTEST, true);
		g->stp.PrepareForRender();
	}	

	m_status = QUERY_COMPLETED;
}

void COcclusionQuery::StartQuery()
{
	m_status = QUERY_NOT_COMPLETED;

	if (!g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		return;
	}

	if (g_simultaneouslyOQCount >= 1)
	{
		return;	
	}

	if (g->ne.isIconicAndIntel)
	{
		m_status = QUERY_NOT_COMPLETED;
		return;
	}

	g_simultaneouslyOQCount++;

	if (0xFFFFFFFF == m_queryID)
	{
		GLFUNC(glGenQueriesARB)(1, &m_queryID);
	}

	m_isResultAvailable = false;
	GLFUNC(glBeginQueryARB)(GL_SAMPLES_PASSED_ARB, m_queryID);
	m_isQueryStarted = true;

	m_status = QUERY_COMPLETED;
}

void COcclusionQuery::EndQuery()
{
	if (!m_isQueryStarted)
	{
		return;
	}	

	if (!g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		return;
	}

	if (m_status == QUERY_NOT_COMPLETED)
	{
		return;
	}

	if (g->ne.isIconicAndIntel)
	{
		m_status = QUERY_NOT_COMPLETED;
		return;
	}

	g_simultaneouslyOQCount--;

	GLFUNC(glEndQueryARB)(GL_SAMPLES_PASSED_ARB);
	m_isQueryStarted = false;
}

int COcclusionQuery::GetFragmentCount()
{
	if (!g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		return 1;
	}

	if (m_status == QUERY_NOT_COMPLETED)
	{
		return 1;
	}

	if (0xFFFFFFFF == m_queryID)
	{
		return 0;
	}

	if (g->ne.isIconicAndIntel)
	{
		m_status = QUERY_NOT_COMPLETED;
		return 1;
	}

	if (!m_isResultAvailable)
	{
		GLFUNC(glGetQueryObjectuivARB)(m_queryID, GL_QUERY_RESULT_ARB, &m_fragmentCount);
	}
	
	return m_fragmentCount;
}

bool COcclusionQuery::IsResultAvailable()
{
	if (!g->ei.IsExtensionSupported(GLEXT_arb_occlusion_query))
	{
		return true;
	}

	if (0xFFFFFFFF == m_queryID)
	{
		return false;
	}

	if (m_status == QUERY_NOT_COMPLETED)
	{
		return false;
	}

	if (g->ne.isIconicAndIntel)
	{
		m_status = QUERY_NOT_COMPLETED;
		return true;
	}

	/*if (!g->rs.GetBool(OCCLUSION_QUERY_ENABLED))
	{
		return true;
	}*/

	if (!m_isResultAvailable)
	{
		unsigned int isResultAvailable = 0;
		GLFUNC(glGetQueryObjectuivARB)(m_queryID, GL_QUERY_RESULT_AVAILABLE_ARB, &isResultAvailable);
		m_isResultAvailable = (isResultAvailable > 0);
		if (m_isResultAvailable)
		{
			GLFUNC(glGetQueryObjectuivARB)(m_queryID, GL_QUERY_RESULT_ARB, &m_fragmentCount);
		}
	}
	
	return m_isResultAvailable;
}

void COcclusionQuery::UpdateQueryObject()
{
	m_queryID = 0xFFFFFFFF;
	m_status = QUERY_NOT_COMPLETED;
}

COcclusionQuery::~COcclusionQuery()
{
	g->occlm.UnregisterQuery(this);
	if (m_queryID != 0xFFFFFFFF)
	{
		GLFUNC(glDeleteQueriesARB)(1, &m_queryID);
	}
}