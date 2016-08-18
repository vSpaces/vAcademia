#pragma once

#include "IRecordEditor.h"
#include "PipeConnector.h"
#include "imessagehandler.h"

class CRecordEditor: 
	public re::IRecordEditor,
	public sync::syncIRecordManagerCallback,
	public cs::imessagehandler
{
public:
	CRecordEditor(oms::omsContext* aContext);
	~CRecordEditor(void);

public:

	BEGIN_IN_MESSAGE_MAP()
		IN_MESSAGE2( CMD_ON_GET_RECORD_LIST, OnGetRecordList)
		IN_MESSAGE2( CMD_ON_HANDSHAKE, OnHandshake)
		IN_MESSAGE2( CMD_ON_OPEN, OnOpenRecord)
		IN_MESSAGE2( CMD_ON_PLAY, OnPlayRecord)
		IN_MESSAGE2( CMD_ON_STOP, OnStopRecord)
		IN_MESSAGE2( CMD_ON_PAUSE, OnPauseRecord)
		IN_MESSAGE2( CMD_ON_DELETE_SELECTION, OnDeleteSelection)
		IN_MESSAGE2( CMD_ON_PASTE_SELECTION, OnPasteSelection)
		IN_MESSAGE2(CMD_ON_GET_RECORD_INFO, OnGetRecordInfo)
		IN_MESSAGE2(CMD_ON_SEEK, OnSeek)
		IN_MESSAGE2(CMD_ON_SAVE, OnSave)
		IN_MESSAGE2(CMD_ON_UNDO, OnUndo)
		IN_MESSAGE2(CMD_ON_REDO, OnRedo)
	END_IN_MESSAGE_MAP()

private:
	void OnGetRecordList( BYTE* aData, int aDataSize);
	void OnHandshake( BYTE* aData, int aDataSize);
	void OnOpenRecord( BYTE* aData, int aDataSize);

	void OnPlayRecord( BYTE* aData, int aDataSize);
	void OnStopRecord( BYTE* aData, int aDataSize);
	void OnPauseRecord( BYTE* aData, int aDataSize);

	void OnDeleteSelection( BYTE* aData, int aDataSize);
	void OnPasteSelection( BYTE* aData, int aDataSize);

	void OnGetRecordInfo( BYTE* aData, int aDataSize);

	void OnSeek( BYTE* aData, int aDataSize);
	void OnSave( BYTE* aData, int aDataSize);

	void OnUndo( BYTE* aData, int aDataSize);
	void OnRedo( BYTE* aData, int aDataSize);

public: // реализация интерфейса re::IRecordEditor
	bool GetConnectionState();

public: // реализация интерфейса sync::syncIRecordManagerCallback

	void onReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo, unsigned int auID, int aiErrorCode);
	// с сервера получена полная информация о записи
	void onReceivedFullRecordInfo(unsigned int aiRecordObjectID,  syncRecordInfo* appRecordInfo, const wchar_t* aOutsideUrl, int aiErrorCode);

public:
	bool Run();

private:
	CPipeConnector*		m_pPipeConnector;
	CCommandManager*	m_pCommandManager;
	
	oms::omsContext* m_pContext;
	bool m_isWorking;

	int beginRecordRequest;
};
