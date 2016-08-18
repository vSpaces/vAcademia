#pragma once

#include <windows.h>
#include <assert.h>

typedef enum _COMMAND_TYPE
{
	//out command codes 0 ..
	CMD_UNKNOWN = 0,
	CMD_IDLE,
	CMD_HANDSHAKE,
	CMD_GET_RECORD_LIST,
	CMD_EXIT,
	CMD_OPEN,
	CMD_GET_RECORD_INFO,


	//in command codes 50 ..
	CMD_ON_UNKNOWN = 50,
	CMD_ON_IDLE,
	CMD_ON_HANDSHAKE,
	CMD_ON_PLAY,
	CMD_ON_STOP,
	CMD_ON_PAUSE,
	CMD_ON_SET_PLAY_POSITION,
	CMD_ON_SET_BEGIN_SELECTION,
	CMD_ON_SET_END_SELECTION,
	CMD_ON_DELETE_SELECTION,
	CMD_ON_OPEN,
	CMD_ON_SAVE,
	CMD_ON_SAVE_AS,
	CMD_ON_GET_RECORD_LIST,
	CMD_ON_PASTE_SELECTION,
	CMD_ON_GET_RECORD_INFO,
	CMD_ON_SEEK,
	CMD_ON_UNDO,
	CMD_ON_REDO,
} COMMAND_TYPE;

class Command
{
public:
	Command();
	Command(COMMAND_TYPE type);
	~Command(void);

	COMMAND_TYPE GetType();
	BYTE * GetData();
	DWORD GetDataSize();
	void ResetSize(DWORD size);

	void AddData(BYTE * data, unsigned int size);

private:
//	COMMAND_TYPE m_type;
	BYTE* m_pData;
	DWORD m_DataSize;
	DWORD m_dataPos;
};
