
#include "StdAfx.h"
#include <Assert.h>
#include "nrmImage.h"
#include "nrmCloud.h"
#include "nrmCamera.h"
#include "nrm3DObject.h"
#include <atlbase.h>
#include "GlobalSingletonStorage.h"

nrmCloud::nrmCloud(mlMedia* apMLMedia) : nrm3DObject(apMLMedia)
{
	m_content = NULL;
	m_footnote = NULL;
	m_pViewport = NULL;
	m_footnoteImage = NULL;
	m_attachedTo = NULL;
	m_isVisible = true;
	m_isShow = true;
	m_moveOutOfScreenAllowed = false;
	m_overlapAllowed = true;
	m_sortOnDepthNeeded = false;
	m_alignID = ALIGN_ID_LEFT;
	m_marginX = 0;
	m_marginY = 0;
	m_contentX = -300;
	m_contentY = -300;
	m_depth = 0;
	m_shiftAtAvatarPosZ = 0;
	m_viewVector.x = 0;
	m_viewVector.y = 0;
	m_viewVector.z = 0;
	MaxVisibleDistanceChanged();
	m_position.x = 0;
	m_position.y = 0;
	m_position.z = 0;
}

bool nrmCloud::IsSortOnDepth()
{
	return m_sortOnDepthNeeded;
}

void nrmCloud::SetDistSq(float distSq)
{
	m_distSq = distSq;
}

float nrmCloud::GetDistSq()const
{
	return m_distSq;
}

void nrmCloud::update(DWORD /*dwTime*/, void* /*data*/)
{
	if (!g->rs.GetBool(SHOW_AVATAR_CLOUDS))
	{
		if (m_content)
		if (m_content->GetIVisible())
		{
			m_content->GetIVisible()->Hide();
		}
		m_isShow = false;
		return;
	}

	if (m_content)

	{
		if (!m_isVisible)
		{
			if (m_isShow)
			{
				m_content->GetIVisible()->Hide();
				m_isShow = false;
			}
			return;
		}

		mlPoint pnt;
		pnt.x = -300;
		pnt.y = -300;

		if ((m_attachedTo == NULL) || (m_attachedTo->GetSafeMO() == NULL) || (m_attachedTo->GetSafeMO()->GetI3DObject() == NULL))
		{
			m_pViewport->GetProjection(m_position, pnt);
		}
		else
		{
			nrm3DObject* nrm3DObj = (nrm3DObject*)m_attachedTo->GetSafeMO();
			if (nrm3DObj->m_obj3d)
			{
				if (!nrm3DObj->m_obj3d->IsVisible())
				{
					if (m_isShow)
					{
						m_content->GetIVisible()->Hide();
						m_isShow = false;
					}
					return;
				}

				CVector3D _pos = nrm3DObj->m_obj3d->GetCoords();
			
				ml3DPosition pos;
				pos.x = _pos.x + m_position.x;
				pos.y = _pos.y + m_position.y;
				pos.z = _pos.z + m_position.z;

				if (0 != m_shiftAtAvatarPosZ)
				{
					AutoSetZPosition();
				}

				m_pViewport->GetProjection(pos, pnt);
			}
		}

		m_oldPos = pnt;

		if (!m_moveOutOfScreenAllowed)
		{
			if ((pnt.x < 0) || (pnt.x >= m_screenWidth) || (pnt.y < 0) || (pnt.y >= m_screenHeight))
			{
				if (m_isShow)
				{
					m_content->GetIVisible()->Hide();
					m_isShow = false;
				}
				return;
			}
		}

		bool backSide = false;
		if (m_viewVector.x != 0 || m_viewVector.y != 0)
		{
			CVector3D eye = m_pViewport->m_viewport->GetCamera()->GetEyePosition();
			CVector3D look = m_pViewport->m_viewport->GetCamera()->GetLookAt();
			CVector3D view = look - eye;
			view.z = 0;
			if ( view.Dot( CVector3D((float)m_viewVector.x, (float)m_viewVector.y, (float)m_viewVector.z)) < 0.0f)
			{
				backSide = true;
			}
		}

		if (!((m_attachedTo == NULL) || (m_attachedTo->GetSafeMO() == NULL) || (m_attachedTo->GetSafeMO()->GetI3DObject() == NULL)))
		{
			nrm3DObject* nrm3DObj = (nrm3DObject*)m_attachedTo->GetSafeMO();
			if (nrm3DObj->m_obj3d)
			{
				CVector3D _pos = nrm3DObj->m_obj3d->GetCoords();
				CalVector pos((float)(_pos.x + m_position.x), (float)(_pos.y + m_position.y), (float)(_pos.z + m_position.z));
						
				if ((m_pViewport->m_viewport->GetCamera()->GetPointDistSq(pos.x, pos.y, pos.z) > m_maxVisibleDistanceSq) ||
					(!g->cm.IsPointInFrustum(pos.x, pos.y, pos.z)) || backSide)
				{
					if (m_isShow)
					{
						m_content->GetIVisible()->Hide();
						m_isShow = false;
					}
				}
				else
				{
					if (m_occlusionQuery.GetFragmentCount() == 0)
					{
						if (m_isShow)
						{
							m_content->GetIVisible()->Hide();
							m_isShow = false;
						}
					}
					else
					{
						if (!m_isShow)
						{		
							m_content->GetIVisible()->Show();
							m_isShow = true;
						}
					}

					m_occlusionQuery.SetQueryPoint(pos.x, pos.y, pos.z);
					m_occlusionQuery.DoQuery();
				}				
			}
		}
		else
		{
			if ((m_pViewport->m_viewport->GetCamera()->GetPointDistSq((float)m_position.x, (float)m_position.y, (float)m_position.z) > m_maxVisibleDistanceSq) ||
				(!g->cm.IsPointInFrustum((float)m_position.x, (float)m_position.y, (float)m_position.z)) || backSide)
			{
				if (m_isShow)
				{
					m_content->GetIVisible()->Hide();
					m_isShow = false;
				}
			}
			else
			{
				if (m_occlusionQuery.GetFragmentCount() == 0)
				{	
					if (m_isShow)
					{
						m_content->GetIVisible()->Hide();
						m_isShow = false;
					}
				}
				else
				{
					if (!m_isShow)
					{
						m_content->GetIVisible()->Show();
						m_isShow = true;
					}
				}

				m_occlusionQuery.SetQueryPoint((float)m_position.x, (float)m_position.y, (float)m_position.z);
				m_occlusionQuery.DoQuery();
			}
		}

		int xShift = 0;
		
		switch (m_alignID)
		{
		case ALIGN_ID_LEFT:
			xShift = 0;
			break;

		case ALIGN_ID_CENTER:
			xShift = -(int)(m_contentWidth / 2);
			break;

		case ALIGN_ID_RIGHT:
			xShift = -m_contentWidth;
			break;
		}

		int contentX = pnt.x + xShift + m_marginX;
		int contentY = pnt.y - m_contentHeight + m_marginY;

		if (((contentX != m_contentX) || (contentY != m_contentY)) && (m_isShow))
		{
			m_content->SetProp("x", contentX);
			m_content->SetProp("y", contentY);
			m_contentX = contentX;
			m_contentY = contentY;
		}
	}
}

bool nrmCloud::IsVisible()const
{
	return m_isShow;
}

void nrmCloud::SetVisible(bool isShow)
{
	m_isShow = isShow;

	if (GetMLMedia())
	if (m_content)
	if (m_content->GetBooleanProp("visible") != m_isShow)
	{
		if (m_isShow)
		{
			m_content->GetIVisible()->Show();
		}
		else
		{
			m_content->GetIVisible()->Hide();
		}
	}
}

void nrmCloud::SetDepth(float depth)
{
	if (m_depth == depth)
	{
		return;
	}

	if (m_content)
	{
		m_depth = depth;
		m_content->SetProp("depth", depth);
	}
}

void nrmCloud::SetScreenSize(int width, int height)
{
	m_screenWidth = width;
	m_screenHeight = height;
}

void nrmCloud::SetActiveViewport(nrmViewport* pViewport)
{
	m_pViewport = pViewport;
}

moICloud* nrmCloud::GetICloud()
{
	return (moICloud*)this;
}

void nrmCloud::OnSetRenderManager()	
{
	AddLink(m_pRenderManager, this);
}

void nrmCloud::VisibleChanged()
{
	if (GetMLMedia())
	{
		m_isVisible = GetMLMedia()->GetICloud()->GetVisibility();

		if (!m_isVisible)
		{
			if (m_content)
			{
				m_content->GetIVisible()->Hide();
				m_isShow = false;
			}
		}
	}
}

void nrmCloud::ContentChanged()
{
	if (GetMLMedia())
	{
		bool isFirstSet = (!m_content);
		m_content = GetMLMedia()->GetICloud()->GetContent();
		if (m_content)
		{	
			if (isFirstSet)
			{
				m_content->SetProp("x", "-300");
				m_content->SetProp("y", "-300");
			}

			m_contentWidth = (int)(m_content->GetDoubleProp("width"));
			m_contentHeight = (int)m_content->GetDoubleProp("height");
		}
	}
}

/*ml3DPosition nrmCloud::GetAbsPos()
{
	if (!m_attachedTo)
	{
		return m_position;
	}
	else
	{
		ml3DPosition tmp = m_attachedTo->GetSafeMO()->GetI3DObject()->GetPos();
		tmp.x += m_position.x;
		tmp.y += m_position.y;
		tmp.z += m_position.z;
		return tmp;
	}
}*/

ml3DPosition nrmCloud::GetPos()
{
	return m_position;
}

void nrmCloud::SetPos(ml3DPosition &aPos)
{
	m_position = aPos;
}

void nrmCloud::PosChanged(ml3DPosition &pos)
{
	m_position = pos;
}

void nrmCloud::FootnoteChanged()
{
	if (GetMLMedia())
	{
		m_footnote = GetMLMedia()->GetICloud()->GetFootnote();
		assert(m_footnote);

		if (m_footnote)
		if (m_footnote->GetSafeMO()->GetIImage())
		{
			m_footnoteImage = (nrmImage *)m_footnote->GetSafeMO()->GetIImage();
		}
	}
}

void nrmCloud::AlignChanged()
{
	if (GetMLMedia())
	{
		USES_CONVERSION;
		CComString align = W2A(GetMLMedia()->GetICloud()->GetAlign());

		if ("left" == align)
		{
			m_alignID = ALIGN_ID_LEFT;
		} 
		else if ("center" == align)
		{
			m_alignID = ALIGN_ID_CENTER;
		} 
		else if ("right" == align)
		{
			m_alignID = ALIGN_ID_RIGHT;
		}
		else
		{
			assert(false);
		}
	}
}

void nrmCloud::MoveOutOfScreenChanged()
{
	if (GetMLMedia())
	{
		m_moveOutOfScreenAllowed = GetMLMedia()->GetICloud()->GetMoveOutOfScreen();
	}
}

void nrmCloud::AllowOverlapChanged()
{
	if (GetMLMedia())
	{
		m_overlapAllowed = GetMLMedia()->GetICloud()->GetAllowOverlap();
	}
}

void nrmCloud::SortOnDepthChanged()
{
	if (GetMLMedia())
	{
		m_sortOnDepthNeeded = GetMLMedia()->GetICloud()->GetSortOnDepth();
	}
}

void nrmCloud::MarginXChanged()
{
	if (GetMLMedia())
	{
		m_marginX = GetMLMedia()->GetICloud()->GetMarginX();
	}
}

void nrmCloud::MarginYChanged()
{
	if (GetMLMedia())
	{
		m_marginY = GetMLMedia()->GetICloud()->GetMarginY();
	}
}

void nrmCloud::AttachedToChanged(mlMedia* pmlMedia)
{
	m_attachedTo = pmlMedia;
}

void nrmCloud::MaxVisibleDistanceChanged()
{
	if (GetMLMedia())
	{
		m_maxVisibleDistance = (float)GetMLMedia()->GetICloud()->GetMaxVisibleDistance();
		m_maxVisibleDistanceSq = m_maxVisibleDistance * m_maxVisibleDistance;
	}
}

void nrmCloud::ViewVecChanged(ml3DPosition &viewVec)
{
	m_viewVector = viewVec;
	m_viewVector.z = 0;
}

bool nrmCloud::SrcChanged()
{
	return true;
}	

void nrmCloud::SetSpecParameters(int adeltaNoChangePos, int ashiftAtAvatarPosZ)
{
	m_deltaNoChangePos = adeltaNoChangePos;
	m_shiftAtAvatarPosZ = ashiftAtAvatarPosZ;	
}

void nrmCloud::AutoSetZPosition()
{
	nrm3DObject* nrm3DObj = (nrm3DObject*)m_attachedTo->GetSafeMO();
	if ((!nrm3DObj) || (!nrm3DObj->m_obj3d))
	{
		return;
	}

	int delta = (int)nrm3DObj->m_obj3d->GetHeight();
	if (delta > m_position.z || m_position.z > delta + m_deltaNoChangePos)
	{
		m_position.z = delta + m_shiftAtAvatarPosZ;
	}
}

nrmCloud::~nrmCloud()
{
	m_pRenderManager->DeleteLink( this);
}