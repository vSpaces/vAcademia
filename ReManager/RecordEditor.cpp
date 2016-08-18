#include "stdafx.h"
#include "RecordEditor.h"
#include "rmml.h"
#include "Protocol\OpenRecordIn.h"
#include "Protocol\PasteIn.h"
#include "Protocol\DeleteSelectionIn.h"
#include "Protocol\GetRecordInfoIn.h"
#include "Protocol\GetRecordInfoOut.h"
#include "Protocol\SeekIn.h"
#include "Protocol\SaveIn.h"

CRecordEditor::CRecordEditor(oms::omsContext* aContext)
{
	m_pContext = aContext;
	m_pCommandManager = new CCommandManager();
	m_pPipeConnector = new CPipeConnector(TEXT("\\\\.\\pipe\\VirtualAcademiaRecordEditorPipe"), this, m_pCommandManager);
	m_isWorking = false;
}

CRecordEditor::~CRecordEditor(void)
{
	assert(m_pPipeConnector != NULL);

	m_isWorking = false;

	m_pPipeConnector->PrepairToDestroy();
	m_pPipeConnector->Stop();

	delete m_pPipeConnector;
	delete m_pCommandManager;
}

bool CRecordEditor::GetConnectionState()
{
	return m_pPipeConnector->isConnected();
}

bool CRecordEditor::Run()
{
	m_isWorking = true;

	return m_pPipeConnector->Start();
}

void CRecordEditor::OnGetRecordList( BYTE* aData, int aDataSize)
{ 
	beginRecordRequest = 0;
	m_pContext->mpSyncMan->GetRecords(0, beginRecordRequest, 50, L"", L"", this);
}

void CRecordEditor::onReceivedRecordsInfo( sync::syncCountRecordInfo& totalCountRec, unsigned int auRecordCount, syncRecordInfo** appRecordsInfo, unsigned int auID, int aiErrorCode)
{
	CGetRecordListOut outPacket( totalCountRec, auRecordCount, appRecordsInfo);

	Command * outCommand = new Command(CMD_GET_RECORD_LIST);
	outCommand->ResetSize(outPacket.GetDataSize());
	outCommand->AddData(outPacket.GetData(), outPacket.GetDataSize());
	m_pCommandManager->AddOutCommand(outCommand);

	if (beginRecordRequest + auRecordCount < totalCountRec.totalCountRecord)
	{
		beginRecordRequest += auRecordCount;
		m_pContext->mpSyncMan->GetRecords(0, beginRecordRequest, 50, L"", L"", this);
	}
}

void CRecordEditor::onReceivedFullRecordInfo(unsigned int aiRecordObjectID,  syncRecordInfo* appRecordInfo, const wchar_t* aOutsideUrl, int aiErrorCode)
{
	CGetRecordInfoOut packet(appRecordInfo);

	Command * command = new Command(CMD_GET_RECORD_INFO);
	command->ResetSize(packet.GetDataSize());
	command->AddData(packet.GetData(), packet.GetDataSize());
	//
	m_pCommandManager->AddOutCommand(command);
}

void CRecordEditor::OnHandshake( BYTE* aData, int aDataSize)
{
	Command * command = new Command(CMD_HANDSHAKE);
	m_pCommandManager->ClearAllCommands();
	m_pCommandManager->AddOutCommand(command);
}


void CRecordEditor::OnOpenRecord( BYTE* aData, int aDataSize)
{
	COpenRecordIn packet(aData, aDataSize);
	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	bool bRet = m_pContext->mpSyncMan->PreciseURL(packet.GetURL().c_str());

	Command * command = new Command(CMD_OPEN);
	command->ResetSize(1);
	command->AddData((BYTE*)&bRet, 1);
	m_pCommandManager->AddOutCommand(command);
}

void CRecordEditor::OnPlayRecord( BYTE* aData, int aDataSize)
{
	m_pContext->mpSyncMan->Play(m_pContext->mpSM->GetCurSeanceID());
}

void CRecordEditor::OnStopRecord( BYTE* aData, int aDataSize)
{
	m_pContext->mpSyncMan->Rewind();
}

void CRecordEditor::OnPauseRecord( BYTE* aData, int aDataSize)
{
	m_pContext->mpSyncMan->Pause();
}

void CRecordEditor::OnDeleteSelection( BYTE* aData, int aDataSize)
{
	CDeleteSelectionIn packet(aData, aDataSize);

	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	m_pContext->mpSyncMan->DeleteRecordSegment( packet.GetBeginPos(), packet.GetEndPos());
}

void CRecordEditor::OnPasteSelection( BYTE* aData, int aDataSize)
{
	CPasteIn packet(aData, aDataSize);

	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	m_pContext->mpSyncMan->RecordPasteSegment(packet.GetBeginPos(), packet.GetEndPos(), packet.GetPastePos(), packet.IsCopy());
}

void CRecordEditor::OnGetRecordInfo( BYTE* aData, int aDataSize)
{
	CGetRecordInfoIn packet(aData, aDataSize);

	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	m_pContext->mpSyncMan->QueryFullRecordInfo(0, packet.GetRecordID(), this);
}

void CRecordEditor::OnSeek(BYTE* aData, int aDataSize)
{
	CSeekIn packet(aData, aDataSize);

	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	m_pContext->mpSyncMan->Seek( packet.GetSeekPos());
}

void CRecordEditor::OnSave(BYTE* aData, int aDataSize)
{
	CSaveIn packet(aData, aDataSize);

	if (!packet.Analyse())
	{
		ATLASSERT( FALSE);
		return;
	}

	m_pContext->mpSyncMan->Save( packet.GetAuthor().c_str(), packet.GetCreationTime().c_str(), packet.GetName().c_str(), packet.GetDescription().c_str());
}

void CRecordEditor::OnUndo(BYTE* aData, int aDataSize)
{
	m_pContext->mpSyncMan->Undo();
}

void CRecordEditor::OnRedo(BYTE* aData, int aDataSize)
{
	m_pContext->mpSyncMan->Redo();
}