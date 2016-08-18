
#pragma once

#include "ResizeableBuffer.h"
#include "SynchBuffer.h"
#include "ISynchPlayer.h"

#define MODE_ADD		0
#define MODE_CLEAR		1

#define SYNCH_PLAY		1
#define SYNCH_RECORD	2

#define TOOL_PEN_ID				0
#define TOOL_CIRCLE_ID			1
#define TOOL_RECTANGLE_ID		2
#define TOOL_LINE_ID			3
#define TOOL_ERASER_ID			4
#define TOOL_TEXT_ID			5
#define TOOL_COPY_BLOCK_ID		6
#define TOOL_CLEAR_SCREEN_ID	7
#define TOOL_UNDO_ID			8

#define COMMON_WIDTH			0
#define COMMON_OPACITY			1

#define ACTION_ID_CHANGE_TOOL			0
#define ACTION_ID_CHANGE_TOOL_PARAM		1
#define ACTION_ID_RMOUSEUP				2
#define ACTION_ID_RMOUSEDOWN			3
#define ACTION_ID_LMOUSEUP				4
#define ACTION_ID_LMOUSEDOWN			5
#define ACTION_ID_MOUSEMOVE				6
#define ACTION_ID_ONCHAR				7
#define ACTION_ID_ONINSERTTEXT			8
#define ACTION_ID_ONCHAR_EXT			9
#define ACTION_ID_ONINSERTWIDETEXT		10
#define ACTION_ID_ONWIDECHAR_EXT		11

#define ACTION_COUNT					12

#define	PLAY_MODE_UNDO					0
#define	PLAY_MODE_RESTORE_STATE			1
#define	PLAY_MODE_ADD					2

class CSynch
{
public:
	CSynch(ISynchPlayer* synchPlayer);
	~CSynch();

	std::string GetActionName(const unsigned char toolID)const;

	void OnMouseMove(unsigned short textureX, unsigned short textureY);
	void OnLMouseDown(unsigned short textureX, unsigned short textureY);
	void OnLMouseUp(unsigned short textureX, unsigned short textureY);
	void OnRMouseDown(unsigned short textureX, unsigned short textureY);
	void OnRMouseUp(unsigned short textureX, unsigned short textureY);
	void OnChar(int keycode, int scancode);
	void OnInsertText(wchar_t* text, bool _isPasteOnIconClick);

	void OnSetParameter(int paramID, void* param);
	void OnSetActiveToolID(unsigned char toolID);

	void SetMode(unsigned char mode);
	unsigned char GetMode();

	void SaveSynchAction(unsigned char actionID, unsigned char* data, int size);
	void GetSynchActions(unsigned char** data, unsigned int* size);
	void GetNewSynchActions(unsigned char** data, unsigned int* size, unsigned char* mode);
	void PlaySynchActions(unsigned char* data, int size, int mode = PLAY_MODE_ADD, bool anIpadState=false);

	void AddHistoryAction(int actionID, void* data, unsigned int size);
	void UndoAction();

	bool IsNewState(int size)const;
	bool IsActionsExist()const;

	void LogData(unsigned char* data, int size);

	int GetAllSynchSize();

private:
	void PlaySynchActionsInternal(unsigned char* data, int size, int mode, bool isPerform);

	void ClearMouseMoveData();

	bool m_isPlaying;
	bool m_isActionsExisted;
	int m_initialSize;

	unsigned char m_mode;

	unsigned char m_workMode;

	unsigned int m_oldSynchDataPosition;

	unsigned char m_lastToolID;

	unsigned int m_allSize;

	CResizeableBuffer m_synchData;
	CResizeableBuffer m_historyData;
	CResizeableBuffer m_historyOffsetPosition;

	ISynchPlayer* m_synchPlayer;
	CSynchBuffer m_actionBuffer;

	int m_lastMouseMoveX, m_lastMouseMoveY;
};