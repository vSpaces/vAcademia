
#include "StdAfx.h"
#include <Math.h>
#include <Assert.h>
#include "PenTool.h"
#include "UndoTool.h"
#include "LineTool.h"
#include "TextTool.h"
#include "CircleTool.h"
#include "EraserTool.h"
#include "ToolPalette.h"
#include "RectangleTool.h"
#include "CopyBlockTool.h"
#include "ToolsParameters.h"
#include "ClearScreenTool.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"
#include <atlconv.h>

#define MAX_TEXT_SIZE	5*512

CToolPalette::CToolPalette():
	m_drawingTarget(NULL),
	m_isLMouseDown(false),
	m_isRMouseDown(false),
	m_activeToolID(255),
	m_mode(MODE_ADD),
	m_ctrl(NULL),
	m_bIPadDrawing(false)
{
	m_isToolUsed = false;

	for (int k = 0; k < TOOLS_COUNT; k++)
	{
		m_tools[k] = NULL;
	}

	m_tools[TOOL_PEN_ID] = MP_NEW(CPenTool);
	m_tools[TOOL_CIRCLE_ID] = MP_NEW(CCircleTool);
	m_tools[TOOL_RECTANGLE_ID] = MP_NEW(CRectangleTool);
	m_tools[TOOL_LINE_ID] = MP_NEW(CLineTool);
	m_tools[TOOL_ERASER_ID] = MP_NEW(CEraserTool);
	m_tools[TOOL_TEXT_ID] = MP_NEW(CTextTool);
	m_tools[TOOL_CLEAR_SCREEN_ID] = MP_NEW(CClearScreenTool);
	m_tools[TOOL_COPY_BLOCK_ID] = MP_NEW(CCopyBlockTool);
	m_tools[TOOL_UNDO_ID] = MP_NEW(CUndoTool);

	CalculateLookupsIfNeeded();

	MP_NEW_V(m_synch, CSynch, this);
}

void CToolPalette::OnActivate()
{
	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->OnDeselected();
	}	

	m_activeToolID = 255;
}

CSynch* CToolPalette::GetSynch()
{
	return m_synch;
}

void CToolPalette::ClearScreen()
{
	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->OnDeselected();
	}

	if (IsToolsUsed())
	{
		m_drawingTarget->Clear();
		m_drawingTarget->SaveBackBuffer();
	}

	m_isToolUsed = false;
}

void CToolPalette::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;

	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->SetDrawingTarget(drawingTarget);
	}
}

void CToolPalette::SetParameter(int paramID, void* param)
{
	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->SetParameter(paramID, param);

		m_synch->OnSetParameter(paramID, param);
	}
}

void CToolPalette::SetCommonParameter(int paramType,  void* param)
 {
	switch (paramType)
	{
		case COMMON_OPACITY:
				switch (m_activeToolID)
				{
					case TOOL_PEN_ID:
						SetParameter(PEN_OPACITY, param);
						break;
					case TOOL_LINE_ID:
						SetParameter(LINE_OPACITY, param);
						break;
					case TOOL_CIRCLE_ID:
						SetParameter(CIRCLE_OPACITY, param);
						break;
					case TOOL_RECTANGLE_ID:
						SetParameter(RECTANGLE_OPACITY, param);
						break;
					case TOOL_ERASER_ID:
						SetParameter(ERASER_OPACITY, param);
						break;
					case TOOL_TEXT_ID:
						SetParameter(TEXT_OPACITY, param);
						break;
				}
			break;
		case COMMON_WIDTH:
				switch(m_activeToolID)
				{
					case TOOL_PEN_ID:
						SetParameter(PEN_WIDTH, param);
						break;
					case TOOL_LINE_ID:
						SetParameter(LINE_WIDTH, param);
						break;
					case TOOL_CIRCLE_ID:
						SetParameter(CIRCLE_STROKE_WIDTH, param);
						break;
					case TOOL_RECTANGLE_ID:
						SetParameter(RECTANGLE_STROKE_WIDTH, param);
						break;
					case TOOL_ERASER_ID:
						SetParameter(ERASER_WIDTH, param);
						break;
					case TOOL_TEXT_ID:
						SetParameter(TEXT_SIZE, param);
						break;
				}
			break;
	}
}

void CToolPalette::SetParameter(unsigned int toolID, int paramID, void* param)
{
	if (toolID >= TOOLS_COUNT)
	{
		return;
	}

	if (m_tools[toolID])
	{
		m_tools[toolID]->SetParameter(paramID, param);
	}
}

unsigned short CToolPalette::GetTextureX(float x)
{
	return (unsigned short)(x * m_drawingTarget->GetWidth());
}

unsigned short CToolPalette::GetTextureY(float y)
{
	return (unsigned short)(y * m_drawingTarget->GetHeight());
}

unsigned short CToolPalette::GetScaledTextureX(float x)
{
	return (unsigned short)(x * m_drawingTarget->GetWidth());
}

unsigned short CToolPalette::GetScaledTextureY(float y)
{
	return (unsigned short)(y * m_drawingTarget->GetHeight());
}
 
void CToolPalette::OnRMouseUp(float x, float y)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if ((m_tools[m_activeToolID]) && (m_drawingTarget))
	{
		unsigned short textureX = GetScaledTextureX(x);
		unsigned short textureY = GetScaledTextureY(y);
		m_tools[m_activeToolID]->OnRMouseUp(textureX, textureY);

		textureX = GetTextureX(x);
		textureY = GetTextureY(y);

		m_synch->OnRMouseUp(textureX, textureY);
	}

	m_isRMouseDown = false;
}

void CToolPalette::OnRMouseDown(float x, float y)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if ((m_tools[m_activeToolID]) && (m_drawingTarget))
	{
		unsigned short textureX = GetScaledTextureX(x);
		unsigned short textureY = GetScaledTextureY(y);
		m_tools[m_activeToolID]->OnRMouseDown(textureX, textureY);

		textureX = GetTextureX(x);
		textureY = GetTextureY(y);

		m_synch->OnRMouseDown(textureX, textureY);
	}

	m_isRMouseDown = true;
}

void CToolPalette::OnLMouseUp(float x, float y)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if ((m_tools[m_activeToolID]) && (m_drawingTarget))
	{
		unsigned short textureX = GetScaledTextureX(x);
		unsigned short textureY = GetScaledTextureY(y);
		m_tools[m_activeToolID]->OnLMouseUp(textureX, textureY);

		textureX = GetTextureX(x);
		textureY = GetTextureY(y);

		m_synch->OnLMouseUp(textureX, textureY);
	}

	m_isLMouseDown = false;
}

void CToolPalette::OnLMouseDown(float x, float y)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if ((m_tools[m_activeToolID]) && (m_drawingTarget))
	{
		unsigned short textureX = GetScaledTextureX(x);
		unsigned short textureY = GetScaledTextureY(y);
		m_tools[m_activeToolID]->OnLMouseDown(textureX, textureY);

		textureX = GetTextureX(x);
		textureY = GetTextureY(y);

		m_synch->OnLMouseDown(textureX, textureY);

		m_isToolUsed = true;
	}

	m_isLMouseDown = true;
}

void CToolPalette::OnMouseMove(float x, float y)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if ((m_tools[m_activeToolID]) && (m_drawingTarget))
	{
		unsigned short textureX = GetScaledTextureX(x);
		unsigned short textureY = GetScaledTextureY(y);
		m_tools[m_activeToolID]->OnMouseMove(textureX, textureY);

		textureX = GetTextureX(x);
		textureY = GetTextureY(y);

		if (((m_isLMouseDown) && (m_tools[m_activeToolID]->GetSynchRequiredType() & SYNCH_MOVE_WHEN_LDOWN)) || 
			((m_isRMouseDown) && (m_tools[m_activeToolID]->GetSynchRequiredType() & SYNCH_MOVE_WHEN_RDOWN)))
		{
			m_synch->OnMouseMove(textureX, textureY);
		}
	}
}

void CToolPalette::OnChar(int keycode, int scancode)
{
	if( m_bIPadDrawing)
		return;
	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->OnChar(keycode, scancode);

		if (keycode == 26)
		{
			UndoAction();
			m_isToolUsed = true;
		}
		else if (m_tools[m_activeToolID]->GetSynchRequiredType() & SYNCH_KEYS)
		{
			if (m_activeToolID == TOOL_TEXT_ID)
				m_isToolUsed = true;

			m_synch->OnChar(keycode, scancode);
		}
	}

}
void CToolPalette::OnKeyUp(int /*keycode*/)
{

}

void CToolPalette::OnLMouseDblClick(float /*x*/, float /*y*/)
{

}

void CToolPalette::OnRMouseDblClick(float /*x*/, float /*y*/)
{

}


ITool* CToolPalette::GetActiveTool()
{
	if (m_activeToolID < TOOLS_COUNT)
	{
		return m_tools[m_activeToolID];
	}
	else
	{
		return NULL;
	}
}

unsigned char CToolPalette::GetActiveToolID()
{
	return m_activeToolID;
}

void CToolPalette::SetActiveToolID(unsigned char toolID)
{
	if (toolID >= TOOLS_COUNT)
	{
		return;
	}

	if (m_activeToolID != toolID)
	{
		if (m_activeToolID < TOOLS_COUNT)
		if (m_tools[m_activeToolID])
		{
			m_tools[m_activeToolID]->OnDeselected();
		}	

		m_activeToolID = toolID;

		if (m_tools[m_activeToolID])
		{
			m_tools[m_activeToolID]->SetDrawingTarget(m_drawingTarget);
			m_tools[m_activeToolID]->OnSelected();			
		}
		
		m_synch->OnSetActiveToolID(m_activeToolID);

		if ((m_activeToolID == TOOL_TEXT_ID) && (!g_isPlaying))
		{
			SetParameter(TEXT_NEW_CURSOR_POSITION, (void *)true);				
		}

		if (m_activeToolID == TOOL_CLEAR_SCREEN_ID)
		{
			m_isToolUsed = false;
		}
	}
}

void CToolPalette::RedoAction(unsigned int toolID, void* data)
{
	if (toolID >= TOOLS_COUNT)
	{
		return;
	}

	if (m_tools[toolID])
	{
		m_tools[toolID]->RedoAction(data);
	}
}

void CToolPalette::RedoAction(void* data)
{
	if (m_activeToolID < TOOLS_COUNT)
	if (m_tools[m_activeToolID])
	{
		m_tools[m_activeToolID]->RedoAction(data);
	}
}

bool CToolPalette::IsNeedMouseEvents()
{
	return (m_synch->GetMode() == SYNCH_RECORD);
}

bool CToolPalette::IsToolsUsed()
{
	return m_isToolUsed || m_synch->IsActionsExist();
}

void CToolPalette::UndoAction()
{
	unsigned char lastToolID = GetActiveToolID();
	SetActiveToolID(TOOL_UNDO_ID);
	SetActiveToolID(lastToolID);
}

void CToolPalette::PasteText(bool isPasteOnIconClick)
{
	if (m_activeToolID == TOOL_TEXT_ID)
	{
		HGLOBAL hData;
		if ( OpenClipboard( g->ne.ghWnd))
		{
			hData = GetClipboardData( CF_UNICODETEXT);
			wchar_t* chBuffer = (wchar_t*)GlobalLock(hData);

			std::wstring result = L"";

			if (chBuffer)
			{
				int size = (wcslen(chBuffer) > MAX_TEXT_SIZE) ? MAX_TEXT_SIZE : wcslen(chBuffer);

				wchar_t* newchBuffer = MP_NEW_ARR(wchar_t, size + 1);
			
				memcpy(newchBuffer, chBuffer, (size + 1) * sizeof(wchar_t));

				newchBuffer[size] = '\0';

				result = newchBuffer;		

				MP_DELETE_ARR(newchBuffer);
			}

			GlobalUnlock(hData);
			CloseClipboard();

			if (result.size() == 0)
			{
				return;
			}

			((CTextTool*)m_tools[TOOL_TEXT_ID])->PasteText((wchar_t *)result.c_str(), isPasteOnIconClick);
			m_synch->OnInsertText((wchar_t *)result.c_str(), isPasteOnIconClick);
			m_isToolUsed = true;
		}
	}
}

void CToolPalette::OnEndRestore()
{
	m_drawingTarget->UnbindToApplyGeometry();
}

void CToolPalette::SetMouseController(IMouseController* ctrl)
{
	m_ctrl = ctrl;
}

void CToolPalette::SetIPadDrawing( bool am_bIPadDrawing)
{
	m_bIPadDrawing = am_bIPadDrawing;
}

bool CToolPalette::GetIPadDrawing()
{
	return m_bIPadDrawing;
}

CToolPalette::~CToolPalette()
{
	if (m_synch)
	{
		MP_DELETE(m_synch);
	}

	if (m_drawingTarget)
	{
		m_drawingTarget->Destroy();
	}

	for (int k = 0; k < TOOLS_COUNT; k++)
	if (m_tools[k])
	{
		m_tools[k]->Destroy();
	}

	if (m_ctrl)
	{
		m_ctrl->RemoveMouseListenerFromLastUsed(this);
	}
}