
#include "StdAfx.h"
#include "UserData.h"
#include "nrmViewport.h"
#include "MouseController.h"

#include "GlobalSingletonStorage.h"
#include "RMContext.h"

CMouseController::CMouseController():
	m_lastX(0.0f),
	m_lastY(0.0f),
	m_isLMouseDown(false),
	m_isRMouseDown(false),
	m_isMouseMove(false),
	m_isMouseHandled(true),
	m_lastCheckDeltaTime(0),
	m_lastMouseListener(NULL),
	m_lastLastMouseListener(NULL),
	m_isOptimizationExists(false),
	m_isMouseHandledAlready(false),
	m_isLMouseDownForLastListener(false),
	m_isRMouseDownForLastListener(false),	
	MP_VECTOR_INIT(m_mouseListeners)
{
	gRM->SetMouseController(this);
}

void CMouseController::SetLastMouseListener(IMouseListener* mouseListener)
{
	m_lastMouseListener = mouseListener;
	if (m_lastMouseListener)
	{
		m_lastMouseListener->SetMouseController(this);
	}
}

void CMouseController::SetLastLastMouseListener(IMouseListener* mouseListener)
{
	m_lastLastMouseListener = mouseListener;
	if (m_lastLastMouseListener)
	{
		m_lastLastMouseListener->SetMouseController(this);
	}
}

void CMouseController::RemoveMouseListenerFromLastUsed(IMouseListener* mouseListener)
{
	if (m_lastMouseListener == mouseListener)
	{
		SetLastMouseListener(NULL);		
	}

	if (m_lastLastMouseListener == mouseListener)
	{
		SetLastLastMouseListener(NULL);		
	}
}

void CMouseController::HandleMouse(bool isHandled)
{
	m_isMouseHandled = isHandled;
}

void CMouseController::EnableMouseOptimization()
{
	if (m_isOptimizationExists)
	{
		g->rs.SetInt(UNDER_MOUSE_CHECK_DELTA_TIME, m_lastCheckDeltaTime);
		m_isOptimizationExists = false;
	}
}

void CMouseController::DisableMouseOptimization()
{
	if (!m_isOptimizationExists)
	{
		m_lastCheckDeltaTime = g->rs.GetInt(UNDER_MOUSE_CHECK_DELTA_TIME);
		g->rs.SetInt(UNDER_MOUSE_CHECK_DELTA_TIME, 1);
		m_isOptimizationExists = true;
	}
}

void CMouseController::NeedUnderMouseCheck()
{
	if (g->scm.GetActiveScene())
	{
		g->scm.GetActiveScene()->NeedUnderMouseCheck();
	}
}

void CMouseController::OnMouseWheel(const int delta)
{
	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnMouseWheel(delta);		
	}
}

void CMouseController::OnLMouseDown(const int x, const int y)
{
	NeedUnderMouseCheck();

	m_isLMouseDown = true;

	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnLMouseDown(x, y);		
	}
}

void CMouseController::OnLMouseUp(const int x, const int y)
{
	NeedUnderMouseCheck();

	m_isLMouseDown = false;

	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnLMouseUp(x, y);		
	}
}

void CMouseController::OnRMouseDown(const int /*x*/, const int /*y*/)
{
	NeedUnderMouseCheck();

	m_isRMouseDown = true;
}

void CMouseController::OnRMouseUp(const int /*x*/, const int /*y*/)
{
	NeedUnderMouseCheck();

	m_isRMouseDown = false;
}

void CMouseController::OnMouseMove(const int x, const int y)
{
	m_isMouseMove = true;

	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnMouseMove(x, y);		
	}
}

void CMouseController::OnKeyUp(const int key, const bool isAlt, const bool isCtrl, const bool isShift)
{
	if (m_lastMouseListener)
	{
		m_lastMouseListener->OnKeyUp(key);
	}
	else if (m_lastLastMouseListener)
	{
		if (m_lastLastMouseListener->IsNeedMouseEvents())
		{
			m_lastLastMouseListener->OnKeyUp(key);
		}
		else
		{
			SetLastLastMouseListener(NULL);			
		}
	}

	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnKeyUp(key, isAlt, isCtrl, isShift);		
	}
}

void CMouseController::OnLMouseDblClick(const int /*x*/, const int /*y*/)
{

}

void CMouseController::OnRMouseDblClick(const int /*x*/, const int /*y*/)
{

}

void CMouseController::UpdateMouseEvent(IMouseListener* mouseListener, float x, float y, bool painting) 
{
	m_isMouseHandledAlready = true;

	if (mouseListener)
	if (!mouseListener->IsNeedMouseEvents())
	{
		return;
	}

	if ((x < 0.0f) || (x > 1.0f) || (y < 0.0f) || (y > 1.0f))
	{
		return;
	}

	if (!mouseListener)
	{
		if ((m_lastMouseListener))
		{
			EnableMouseOptimization();

			if (m_isLMouseDownForLastListener)
			{
				m_lastMouseListener->OnLMouseUp(m_lastX, m_lastY);
			}

			if (m_isRMouseDownForLastListener)
			{
				m_lastMouseListener->OnRMouseUp(m_lastX, m_lastY);
			}

			SetLastLastMouseListener(m_lastMouseListener);
			SetLastMouseListener(NULL);			
		}

		return;
	}
	else
	{
		DisableMouseOptimization();
	}

	if (m_lastMouseListener != mouseListener)
	{
		m_isLMouseDownForLastListener = false;
		m_isRMouseDownForLastListener = false;
	}

	if (!m_isMouseHandled)
	{
		return;
	}

	if (painting)
		UpdateMouseEventForPaint(mouseListener, x, y);
	else
		UpdateMouseEventForSharing(mouseListener, x, y);

	SetLastMouseListener(mouseListener);	
	m_lastX = x;
	m_lastY = y;
}

void CMouseController::UpdateMouseEventForPaint(IMouseListener* mouseListener, float x, float y)
{
	if (m_isLMouseDown)
	{
		if (!m_isLMouseDownForLastListener)
		{
			mouseListener->OnLMouseDown(x, y);
			m_isLMouseDownForLastListener = true;
		}
		else
		{
			mouseListener->OnMouseMove(x, y);
		}
	}
	else if (m_isRMouseDown)
	{
		if (!m_isRMouseDownForLastListener)
		{
			mouseListener->OnRMouseDown(x, y);
			m_isRMouseDownForLastListener = true;
		}
		else
		{
			mouseListener->OnMouseMove(x, y);
		}
	}
	else
	{
		if (m_isLMouseDownForLastListener)
		{
			mouseListener->OnLMouseUp(x, y);
			m_isLMouseDownForLastListener = false;
		}

		if (m_isRMouseDownForLastListener)
		{
			mouseListener->OnRMouseUp(x, y);
			m_isRMouseDownForLastListener = false;
		}
	}
}

void CMouseController::UpdateMouseEventForSharing(IMouseListener* mouseListener, float x, float y)
{
	if (m_isLMouseDown)
	{
		if (!m_isLMouseDownForLastListener)
		{
			mouseListener->OnLMouseDown(x, y);
			m_isLMouseDownForLastListener = true;
		}
		else
		{
			mouseListener->OnMouseMove(x, y);
		}
	}
	else if (m_isRMouseDown)
	{
		if (!m_isRMouseDownForLastListener)
		{
			mouseListener->OnRMouseDown(x, y);
			m_isRMouseDownForLastListener = true;
		}
		else
		{
			mouseListener->OnMouseMove(x, y);
		}
	}
	else if (m_isMouseMove)
	{
		mouseListener->OnMouseMove(x, y);
		m_isMouseMove = false;
	}
	else
	{
		if (m_isLMouseDownForLastListener)
		{
			mouseListener->OnLMouseUp(x, y);
			m_isLMouseDownForLastListener = false;
		}

		if (m_isRMouseDownForLastListener)
		{
			mouseListener->OnRMouseUp(x, y);
			m_isRMouseDownForLastListener = false;
		}
	}
}

void CMouseController::OnKeyDown(int key, int scancode, bool isAlt, bool isCtrl, bool isShift)
{
	if (IsKeyboardInputNeeded())
	if (m_lastMouseListener)
	{
		m_lastMouseListener->OnChar(key, scancode);
	}
	else if (m_lastLastMouseListener)
	{
		if (m_lastLastMouseListener->IsNeedMouseEvents())
		{
			m_lastLastMouseListener->OnChar(key, scancode);
		}
		else
		{
			SetLastLastMouseListener(NULL);			
		}
	}

	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if ((*it)->IsActive())
	{
		(*it)->OnKeyDown(key, scancode, isAlt, isCtrl, isShift);		
	}
}

bool CMouseController::IsKeyboardInputNeeded()
{
	return ((m_lastMouseListener != NULL) || (m_lastLastMouseListener != NULL));
}

void CMouseController::OnNewFrame()
{
	m_isMouseHandledAlready = false;
}

void CMouseController::UpdateMouseInfoIfNeeded()
{
	if (!m_lastMouseListener)
	{
		return;
	}

	if (m_isMouseHandledAlready)
	{
		return;
	}

	for (int i = 0; i < g->scm.GetCount(); i++)
	{
		CScene* scene = g->scm.GetObjectPointer(i);
		if (scene->IsDeleted())
		{
			continue;
		}

		if (scene->GetViewport())
		{
			mlMedia* mlViewport = ((nrmObject*)scene->GetViewport()->GetUserData(USER_DATA_NRMOBJECT))->GetMLMedia();
			if (mlViewport)
			{
				if (mlViewport->GetSafeMO())
				{	
					nrmViewport* viewport = (nrmViewport*)mlViewport->GetSafeMO();
					mlPoint aMouseXY;
					POINT tmp;
					GetCursorPos(&tmp);
					ScreenToClient(g->ne.ghWnd, &tmp);
					aMouseXY.x = tmp.x;
					aMouseXY.y = tmp.y;
					viewport->GetObjectUnderMouse(aMouseXY);
				}
			}
		}
	}
}

void CMouseController::DisableKeyboardInput()
{
	SetLastMouseListener(NULL);
	SetLastLastMouseListener(NULL);
}

void CMouseController::AddMouseListener(ISimpleEventsListener* mouseListener)
{
	m_mouseListeners.push_back(mouseListener);
}

void CMouseController::DeleteMouseListener(ISimpleEventsListener* mouseListener)
{
	std::vector<ISimpleEventsListener *>::iterator it = m_mouseListeners.begin();
	std::vector<ISimpleEventsListener *>::iterator itEnd = m_mouseListeners.end();

	for ( ; it != itEnd; it++)
	if (*it == mouseListener)
	{
		m_mouseListeners.erase(it);
		break;
	}
}

CMouseController::~CMouseController()
{
}