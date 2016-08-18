
#include "StdAfx.h"
#include <Assert.h>
#include "Synch.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"
#include "TextTool.h"

#define	START_BUFFER_SIZE	8192
#define	DELTA_BUFFER_SIZE	4096

CSynch::CSynch(ISynchPlayer* synchPlayer):
	m_lastToolID(TOOL_RECTANGLE_ID),
	m_synchPlayer(synchPlayer),
	m_oldSynchDataPosition(0),
	m_isActionsExisted(false),
	m_workMode(SYNCH_PLAY),
	m_isPlaying(false),
	m_initialSize(0),
	m_allSize(0)
{	
	ClearMouseMoveData();
}

std::string CSynch::GetActionName(const unsigned char toolID)const
{
	switch (toolID)
	{
	case ACTION_ID_CHANGE_TOOL:
		return "ChangeTool";
		break;

	case ACTION_ID_CHANGE_TOOL_PARAM:
		return "ChangeToolParam";
		break;

	case ACTION_ID_RMOUSEUP:
		return "RMouseUp";
		break;

	case ACTION_ID_RMOUSEDOWN:
		return "RMouseDown";
		break;

	case ACTION_ID_LMOUSEUP:
		return "LMouseUp";
		break;

	case ACTION_ID_LMOUSEDOWN:
		return "LMouseDown";
		break;

	case ACTION_ID_MOUSEMOVE:
		return "MouseMove";
		break;

	case ACTION_ID_ONCHAR:
		return "OnChar";
		break;

	case ACTION_ID_ONINSERTTEXT:
		return "OnInsertText";
		break;

	case ACTION_ID_ONINSERTWIDETEXT:
		return "OnInsertWideText";
		break;

	case ACTION_ID_ONCHAR_EXT:
		return "OnCharExt";
		break;

	case ACTION_ID_ONWIDECHAR_EXT:
		return "OnWideCharExt";
		break;
	}

	return "";
}

void CSynch::OnSetActiveToolID(unsigned char toolID)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedChar(toolID);
	if (toolID != TOOL_UNDO_ID)
	{
		SaveSynchAction(ACTION_ID_CHANGE_TOOL, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());
	}

	switch (toolID)
	{
	case TOOL_CLEAR_SCREEN_ID:
		if (!m_isPlaying)
		{
			m_mode = MODE_CLEAR;
			m_synchData.Clear();
			m_allSize = 0;
			m_oldSynchDataPosition = 0;
			m_initialSize = 0;
			OnSetActiveToolID(m_lastToolID);
			m_isActionsExisted = false;
		}
		break;

	case TOOL_UNDO_ID:
		UndoAction();
		break;
	}

	if ((toolID != TOOL_CLEAR_SCREEN_ID) && (toolID != TOOL_UNDO_ID))
	{
		m_lastToolID = toolID;
	}
}

void CSynch::OnSetParameter(int paramID, void* param)
{
	int paramSize = GetToolParameterSize(paramID);
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedChar((unsigned char)paramID);

	switch (paramSize)
	{
	case 1:
		m_actionBuffer.AddUnsignedChar((unsigned char)(((unsigned int)param)%256));
		break;

	case 4:
		m_actionBuffer.AddUnsignedInt((unsigned int)param);
		break;
	}

	SaveSynchAction(ACTION_ID_CHANGE_TOOL_PARAM, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());
}

void CSynch::SetMode(unsigned char mode)
{
	m_workMode = mode;
}

unsigned char CSynch::GetMode()
{
	return m_workMode;
}

void CSynch::ClearMouseMoveData()
{
	m_lastMouseMoveX = -1;
	m_lastMouseMoveY = -1;
}

void CSynch::OnMouseMove(unsigned short textureX, unsigned short textureY)
{
	if ((m_lastMouseMoveX == textureX) && (m_lastMouseMoveY == textureY))
	{
		return;
	}

	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort(textureX);
	m_actionBuffer.AddUnsignedShort(textureY);
	SaveSynchAction(ACTION_ID_MOUSEMOVE, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());

	m_lastMouseMoveX = textureX;
	m_lastMouseMoveY = textureY;
}

void CSynch::OnLMouseDown(unsigned short textureX, unsigned short textureY)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort(textureX);
	m_actionBuffer.AddUnsignedShort(textureY);
	SaveSynchAction(ACTION_ID_LMOUSEDOWN, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());

	m_isActionsExisted = true;
}

void CSynch::OnLMouseUp(unsigned short textureX, unsigned short textureY)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort(textureX);
	m_actionBuffer.AddUnsignedShort(textureY);
	SaveSynchAction(ACTION_ID_LMOUSEUP, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());
}

void CSynch::OnRMouseDown(unsigned short textureX, unsigned short textureY)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort(textureX);
	m_actionBuffer.AddUnsignedShort(textureY);
	SaveSynchAction(ACTION_ID_RMOUSEDOWN, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());
}

void CSynch::OnRMouseUp(unsigned short textureX, unsigned short textureY)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort(textureX);
	m_actionBuffer.AddUnsignedShort(textureY);
	SaveSynchAction(ACTION_ID_RMOUSEUP, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());
}

void CSynch::OnChar(int keycode, int scancode)
{
	m_actionBuffer.Clear();
	m_actionBuffer.AddUnsignedShort((unsigned short)keycode);
	m_actionBuffer.AddUnsignedChar((unsigned char)scancode);
	SaveSynchAction(ACTION_ID_ONWIDECHAR_EXT, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());

	if (m_lastToolID == TOOL_TEXT_ID)
		m_isActionsExisted = true;
}

void CSynch::OnInsertText(wchar_t* text, bool _isPasteOnIconClick)
{
	m_actionBuffer.Clear();
	unsigned char isPasteOnIconClick = _isPasteOnIconClick?1:0;
	m_actionBuffer.AddUnsignedChar(isPasteOnIconClick);

	for (unsigned int i = 0; i < wcslen(text); i++)
	{	
		m_actionBuffer.AddUnsignedShort(text[i]);
	}

	m_actionBuffer.AddUnsignedShort(0);

	SaveSynchAction(ACTION_ID_ONINSERTWIDETEXT, m_actionBuffer.GetData(), m_actionBuffer.GetDataSize());

	m_isActionsExisted = true;
}

void CSynch::SaveSynchAction(unsigned char actionID, unsigned char* data, int size)
{
	if (ACTION_ID_MOUSEMOVE == actionID)
	{
		ClearMouseMoveData();
	}

	m_allSize += size;

	AddHistoryAction(actionID, data, size);

	if (m_workMode == SYNCH_PLAY)
	{
		return;
	}

	if (m_isPlaying)
	{
		return;
	}

/*	if (size + 1 + m_synchDataPosition > m_synchDataSize)
	{
		unsigned char* tmp = m_synchData;
		m_synchData = new unsigned char[m_synchDataSize + DELTA_BUFFER_SIZE];
		memcpy(m_synchData, tmp, m_synchDataSize);
		delete[] tmp;
		m_synchDataSize += DELTA_BUFFER_SIZE;
	}

	m_synchData[m_synchDataPosition++] = actionID;
	memcpy(m_synchData + m_synchDataPosition, data, size);
	m_synchDataPosition += size;*/

	m_synchData.AddData(&actionID, 1);
	if ((data) && (size > 0))
	{
		m_synchData.AddData(data, size);
	}
}

void CSynch::GetSynchActions(unsigned char** data, unsigned int* size)
{
	if (NULL == *data)
	{
		*size = /*m_synchDataPosition*/m_synchData.GetSize();
		return;
	}

	memcpy(*data, m_synchData.GetPointer(), m_synchData.GetSize());
	*size = m_synchData.GetSize();

	if (MODE_CLEAR == m_mode)
	{
		m_mode = MODE_ADD;
	}

	m_oldSynchDataPosition = m_synchData.GetSize();
}

void CSynch::GetNewSynchActions(unsigned char** data, unsigned int* size, unsigned char* mode)
{
    *data = (unsigned char*)m_synchData.GetPointer() + m_oldSynchDataPosition;
	*size = m_synchData.GetSize() - m_oldSynchDataPosition;
	*mode = m_mode;

	if (MODE_CLEAR == m_mode)
	{
		m_mode = MODE_ADD;
	}

	m_oldSynchDataPosition = m_synchData.GetSize();
}

bool CSynch::IsNewState(int size)const
{
	// потенциально неверная проверка
	if (m_synchData.GetSize() + m_initialSize == size)
	{
		return false;
	}

	return true;
}

void CSynch::LogData(unsigned char* data, int size)
{
	while (size > 0)
	{
		unsigned char actionID = data[0];
		data++;
		size--;

		switch (actionID)
		{
		case ACTION_ID_CHANGE_TOOL:
			if (size > 0)
			{
				if (data[0] != TOOL_UNDO_ID)
				{
					g->lw.WriteLn("[WB] Set active tool id: ", (int)data[0]);
				}
				
				size--;
				data++;
			}
			break;

		case ACTION_ID_CHANGE_TOOL_PARAM:
			if (size > 0)
			{
				unsigned char paramID = data[0];
				size--;
				data++;

				void* param = NULL;
				int paramSize = GetToolParameterSize(paramID);
				if (size >= paramSize)
				{
					if (1 == paramSize)
					{
						param = (void*)data[0];
						data++;
						size--;
					}
					else if (4 == paramSize)
					{
						unsigned int tmp = *(unsigned int*)data;
						param = (void*)tmp;
						size -= 4;
						data += 4;
					}
					else
					{
						assert(false);
					}

					g->lw.WriteLn("[WB] Set param: ", (int)paramID);
				}
			}
			break;

		case ACTION_ID_RMOUSEUP:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				g->lw.WriteLn("[WB] rmouseup: ", x, " ", y);
			}
			break;

		case ACTION_ID_RMOUSEDOWN:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				g->lw.WriteLn("[WB] rmousedown: ", x, " ", y);
			}
			break;

		case ACTION_ID_LMOUSEUP:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				g->lw.WriteLn("[WB] lmouseup: ", x, " ", y);
			}
			break;

		case ACTION_ID_LMOUSEDOWN:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				g->lw.WriteLn("[WB] lmousedown: ", x, " ", y);
			}
			break;

		case ACTION_ID_MOUSEMOVE:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
		
				g->lw.WriteLn("[WB] mousemove: ", x, " ", y);
			}
			break;

		case ACTION_ID_ONCHAR:
			if (size >= 1)
			{
				unsigned char ch = *(unsigned char*)data;
				data++;
				size--;
				g->lw.WriteLn("[WB] on_char: ", ch);
			}
			break;
		case ACTION_ID_ONINSERTTEXT:
			if (size >= 1)
			{
				data+=1;
				size-=1;

				std::string str =  (char*)data;
				int offset = strlen((char*)data)+1;
				size -= offset;
				data += offset;

				g->lw.WriteLn("[WB] on_insert_text: ", str);
			}
			break;

		case ACTION_ID_ONCHAR_EXT:
			if (size >= 2)
			{
				unsigned char ch = *(unsigned char*)data;
				data++;
				size--;
				unsigned char ch2 = *(unsigned char*)data;
				data++;
				size--;

				g->lw.WriteLn("[WB] on_char_ext: ", ch , " , ",ch2 );
			}
			break;
		}
	}
}

void CSynch::PlaySynchActionsInternal(unsigned char* data, int size, int mode, bool isPerform)
{
	while (size > 0)
	{
		unsigned char actionID = data[0];
		data++;
		size--;

		/*if ((actionID >= 0) && (actionID < ACTION_COUNT))
		{
			g->lw.WriteLn("PLAY ACTION: ", GetActionName(actionID));
		}
		else
		{
			g->lw.WriteLn("INVALID ACTION: ", IntToStr(actionID));
		}*/

		switch (actionID)
		{
		case ACTION_ID_CHANGE_TOOL:
			if (size > 0)
			{
				if ((data[0] != TOOL_UNDO_ID) || (mode != PLAY_MODE_UNDO))
				{
					if (isPerform)
					{
						m_synchPlayer->SetActiveToolID(data[0]);
					}
				}
				
				size--;
				data++;
			}
			break;

		case ACTION_ID_CHANGE_TOOL_PARAM:
			if (size > 0)
			{
				unsigned char paramID = data[0];
				size--;
				data++;
				
				void* param = NULL;
				int paramSize = GetToolParameterSize(paramID);
				if (size >= paramSize)
				{
					if (1 == paramSize)
					{
						param = (void*)data[0];
						data++;
						size--;
					}
					else if (4 == paramSize)
					{
						unsigned int tmp = *(unsigned int*)data;
						param = (void*)tmp;
						size -= 4;
						data += 4;
					}
					else
					{
						assert(false);
						break;
					}

					if (isPerform)
					{
						m_synchPlayer->SetParameter(paramID, param);
					}
				}
			}
			break;

		case ACTION_ID_RMOUSEUP:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnRMouseUp(x, y);
					}
					OnRMouseUp(x, y);
				}
			}
			break;

		case ACTION_ID_RMOUSEDOWN:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnRMouseDown(x, y);
					}
					OnRMouseDown(x, y);
				}
			}
			break;

		case ACTION_ID_LMOUSEUP:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnLMouseUp(x, y);
					}
					OnLMouseUp(x, y);
				}
			}
			break;

		case ACTION_ID_LMOUSEDOWN:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnLMouseDown(x, y);
					}
					OnLMouseDown(x, y);
				}
			}
			break;

		case ACTION_ID_MOUSEMOVE:
			if (size >= 4)
			{
				unsigned short x = *(unsigned short*)data;
				data += 2;
				unsigned short y = *(unsigned short*)data;
				data += 2;
				size -= 4;
		
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnMouseMove(x, y);
					}
					OnMouseMove(x, y);
				}
			}
			break;

		case ACTION_ID_ONCHAR:
			if (size >= 1)
			{
				unsigned char ch = *(unsigned char*)data;
				data++;
				size--;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnChar(ch, ch);
					}
					OnChar(ch, ch);
				}
			}
			break;

		case ACTION_ID_ONCHAR_EXT:
			if (size >= 2)
			{
				unsigned char ch = *(unsigned char*)data;
				data++;
				size--;
				unsigned char ch2 = *(unsigned char*)data;
				data++;
				size--;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnChar(ch, ch2);
					}
					OnChar(ch, ch2);
				}
			}
			break;

		case ACTION_ID_ONWIDECHAR_EXT:
			if (size >= 3)
			{
				wchar_t ch = *(wchar_t*)data;
				data += 2;
				size -= 2;
				unsigned char ch2 = *(unsigned char*)data;
				data++;
				size--;
				if (m_synchPlayer->GetActiveTool())
				{
					if (isPerform)
					{
						m_synchPlayer->GetActiveTool()->OnChar(ch, ch2);
					}
					OnChar(ch, ch2);
				}
			}
			break;

		case ACTION_ID_ONINSERTTEXT:
			if (size >= 1)
			{
				bool isPasteOnIconClick = (*(unsigned char*)data == 1)?true:false;
				data+=1;
				size-=1;

				if (m_synchPlayer->GetActiveTool())
				{
					wchar_t* data_ = CharToWide((char*)data);
					if (isPerform)
					{					
						ITool*  activeTool = m_synchPlayer->GetActiveTool();
						((CTextTool*)activeTool)->PasteText(data_, isPasteOnIconClick);						
					}
					OnInsertText(data_, isPasteOnIconClick);
					MP_DELETE_ARR(data_);
				}

				int offset = strlen((char*)data)+1;
				size -= offset;
				data += offset;
			}
			break;
		
		case ACTION_ID_ONINSERTWIDETEXT:
			if (size >= 1)
			{
				bool isPasteOnIconClick = (*(unsigned char*)data == 1)?true:false;
				data+=1;
				size-=1;

				if (m_synchPlayer->GetActiveTool())
				{					
					if (isPerform)
					{					
						m_synchPlayer->SetActiveToolID(TOOL_TEXT_ID);
						ITool*  activeTool = m_synchPlayer->GetActiveTool();						
						((CTextTool*)activeTool)->PasteText((wchar_t*)data, isPasteOnIconClick);						
					}
					OnInsertText((wchar_t*)data, isPasteOnIconClick);					
				}

				int offset = wcslen((wchar_t*)data)+1;
				size -= offset * 2;
				data += offset * 2;
			}
			break;
		}
	}

	if (g_isPlaying)
	if (m_synchPlayer->GetActiveTool())
	{
		m_synchPlayer->GetActiveTool()->OnEndRestoreState();
	}
}

int CSynch::GetAllSynchSize()
{
	return m_allSize;
}

void CSynch::PlaySynchActions(unsigned char* data, int size, int mode, bool anIpadState )
{
	if ((mode == PLAY_MODE_RESTORE_STATE) || (mode == PLAY_MODE_UNDO))
	{
		m_historyData.Clear();
		m_historyOffsetPosition.Clear();
		m_initialSize = size;
		g_isPlaying = true;

		if (mode == PLAY_MODE_RESTORE_STATE)
		{
			m_allSize = size;
	}
	}
	else
	{
		m_allSize += size;
	}

	m_isPlaying = true;

	if (size != 0)
	if (g_isPlaying)
	{
		m_historyData.Clear();
		m_historyOffsetPosition.Clear();

		m_synchData.Clear();
		m_oldSynchDataPosition = 0;

		m_isPlaying = false;
		m_synchPlayer->ClearScreen();
		m_isPlaying = true;
		m_isActionsExisted = false;

		if (anIpadState)
			m_isPlaying = false;
		PlaySynchActionsInternal(data, size, mode, true);
	}
	else
	{
		if (anIpadState)
			m_isPlaying = false;
		PlaySynchActionsInternal(data, size, mode, true);
	}

	m_isPlaying = false;

	if ((mode == PLAY_MODE_RESTORE_STATE) || (mode == PLAY_MODE_UNDO))
	{
		g_isPlaying = false;
		m_synchPlayer->OnEndRestore();
	}
}

void CSynch::AddHistoryAction(int actionID, void* data, unsigned int size)
{
	if ((ACTION_ID_ONCHAR == actionID) || (ACTION_ID_LMOUSEDOWN == actionID) || (ACTION_ID_ONINSERTTEXT == actionID)
		|| ((ACTION_ID_CHANGE_TOOL == actionID) && (*(unsigned char*)data == TOOL_CLEAR_SCREEN_ID)) )
	{
		int offset = m_historyData.GetSize();
		m_historyOffsetPosition.AddData(&offset, 4);
	}

	m_historyData.AddData(&actionID, 1);
	if ((data) && (size > 0))
	{
		m_historyData.AddData(data, size);
	}
}

void CSynch::UndoAction()
{
	if (m_historyOffsetPosition.GetSize() == 0)
	{
		return;
	}

	int endOffset = 0;
	bool res = m_historyOffsetPosition.PopData(&endOffset, 4);
	if (!res)
	{
		assert(false);
		m_historyOffsetPosition.Clear();
		m_historyData.Clear();
		return;
	}
	
	if (!g_isPlaying)
	{
		m_isPlaying = true;
		m_synchPlayer->ClearScreen();
		m_isActionsExisted = false;
		m_isPlaying = false;
	}

	if (endOffset > 0)
	{
		unsigned char* tmp = MP_NEW_ARR(unsigned char, endOffset);
		memcpy(tmp, (unsigned char *)m_historyData.GetPointer(), endOffset);
		m_historyOffsetPosition.Clear();
		m_historyData.Clear();

		m_mode = MODE_CLEAR;
		m_synchData.Clear();
		m_oldSynchDataPosition = 0;
		m_initialSize = 0;
		PlaySynchActionsInternal(tmp, endOffset, /*PLAY_MODE_UNDO*/PLAY_MODE_RESTORE_STATE, !g_isPlaying);

		MP_DELETE_ARR(tmp);
	}
}

bool CSynch::IsActionsExist()const
{
	return m_isActionsExisted;
}

CSynch::~CSynch() 
{
}