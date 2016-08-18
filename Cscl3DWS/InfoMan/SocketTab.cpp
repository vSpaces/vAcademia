// CSocketTab.cpp : Implementation of CSocketTab

#include "stdafx.h"
#include "LogTab.h"
#include "SocketTab.h"
#include "IDesktopMan.h"
#include ".\SocketTab.h"
#include <sstream>

SessionTypeMap::SessionTypeMap()
{
	for (int i = 0; i < SESSION_MAP_SIZE; ++i )
	{
		for (int j = 0; j < SESSION_TYPE_SIZE; ++j)
		{
			MP_STRING_DC_INIT(typeMap[j][i]);
		}
	}
}

CSocketTab::CSocketTab() :
	MP_MAP_INIT(typeListMap)
{
//	InitializeCriticalSection(&csTypeListMap);

//	EnterCriticalSection(&csTypeListMap);

	typeListMap["SyncServerSession"] = &syncServerTypeMap;
	typeListMap["ServiceServerSession"] = &serviceServerTypeMap;
	typeListMap["ResServerSession"] = &resTypeMap;
	typeListMap["ResXmlServerSession"] = &oResXMLTypeMap;
	typeListMap["ResUploadServerSession"] = &oResUploadSocketTypeMap;
	typeListMap["ServiceWorldServer"] = &serviceWorldServerTypeMap;
	typeListMap["VoipPlayServerSession"] = &VoipPlayServerTypeMap;
	typeListMap["PreviewServerSession"] = &PreviewServerTypeMap;
	typeListMap["UDPServerSession"] = &UDPServerTypeMap;
	typeListMap["SharingServerSession"] = &sharingServerTypeMap;

	SessionsTypeListMap::iterator it = typeListMap.begin();
	SessionsTypeListMap::iterator end = typeListMap.end();
	for ( ; it != end; ++it)
	{
		SessionTypeMap * pMap = it->second;
		for (int i = 0; i < 256; ++i)
		{
			(*pMap).typeMap[0][i] = "";
			(*pMap).typeMap[1][i] = "";
		}
	}

	syncServerTypeMap.typeMap[0][1] = "ST_SyncData";
	syncServerTypeMap.typeMap[0][2] = "ST_QueryLatentedObjectState";
	syncServerTypeMap.typeMap[0][3] = "ST_QueryObjectState";
	syncServerTypeMap.typeMap[0][4] = "ST_SyncMessage";
	syncServerTypeMap.typeMap[0][5] = "ST_StartRecord";
	syncServerTypeMap.typeMap[0][6] = "ST_StopRecord";
	syncServerTypeMap.typeMap[0][7] = "ST_QueryLogRecords";
	syncServerTypeMap.typeMap[0][8] = "ST_Play";
	syncServerTypeMap.typeMap[0][9] = "ST_Stop";
	syncServerTypeMap.typeMap[0][10] = "ST_Delete";
	syncServerTypeMap.typeMap[0][11] = "ST_UpdateRecord";
	syncServerTypeMap.typeMap[0][13] = "ST_GetSeances";
	syncServerTypeMap.typeMap[0][14] = "ST_Rewind";
	syncServerTypeMap.typeMap[0][15] = "ST_QueryFullSeanceInfo";
	syncServerTypeMap.typeMap[0][16] = "ST_DeleteObject";
	syncServerTypeMap.typeMap[0][17] = "ST_GetSeanceByURL";
	syncServerTypeMap.typeMap[0][18] = "ST_GetObjectStatus";
	syncServerTypeMap.typeMap[0][19] = "ST_Pause";
	syncServerTypeMap.typeMap[0][20] = "ST_ClientClose";
	syncServerTypeMap.typeMap[0][21] = "ST_SessionCheckAnswer";
	syncServerTypeMap.typeMap[0][22] = "ST_SetSessionDebugMode";
	syncServerTypeMap.typeMap[0][23] = "ST_AvatarChangeLocation";
	syncServerTypeMap.typeMap[0][25] = "ST_PreciseURL";
	syncServerTypeMap.typeMap[0][27] = "ST_DeleteRecords";
	syncServerTypeMap.typeMap[0][28] = "ST_GetUserList";
	syncServerTypeMap.typeMap[0][29] = "ST_Seek";
	syncServerTypeMap.typeMap[0][30] = "ST_DeleteRecordSegment";
	syncServerTypeMap.typeMap[0][31] = "ST_RecordSave";
	syncServerTypeMap.typeMap[0][32] = "ST_RecordEditorUndo";
	syncServerTypeMap.typeMap[0][33] = "ST_RecordEditorRedo";
	syncServerTypeMap.typeMap[0][34] = "ST_SetPilotLogin";
	syncServerTypeMap.typeMap[0][35] = "ST_DeleteIntoRecycleBin";
	syncServerTypeMap.typeMap[0][36] = "ST_QueryEventRecords";
	syncServerTypeMap.typeMap[0][40] = "ST_EventUserDefaultRightsUpdate";
	syncServerTypeMap.typeMap[0][41] = "ST_EventUserRightsUpdate";
	syncServerTypeMap.typeMap[0][42] = "ST_EventUserBanUpdate";
	syncServerTypeMap.typeMap[0][43] = "ST_EventGetUserRight";
	syncServerTypeMap.typeMap[0][44] = "ST_RecordEditorCut";
	syncServerTypeMap.typeMap[0][45] = "ST_RecordEditorCopy";
	syncServerTypeMap.typeMap[0][46] = "ST_RecordEditorPaste";
	syncServerTypeMap.typeMap[0][47] = "ST_RecordEditorDelete";
	syncServerTypeMap.typeMap[0][48] = "ST_RecordEditorSave";
	syncServerTypeMap.typeMap[0][49] = "ST_RecordEditorSaveAs";

	VoipPlayServerTypeMap.typeMap[0][1] = "WM_EVOVOIP_CONNECTSUCCESS";

	PreviewServerTypeMap.typeMap[1][1] = "ST_AddFrame";
	PreviewServerTypeMap.typeMap[1][2] = "ST_StopRecord";
	PreviewServerTypeMap.typeMap[1][3] = "ST_DeleteRecord";
	PreviewServerTypeMap.typeMap[1][4] = "ST_AddPrintScreen";
	PreviewServerTypeMap.typeMap[1][5] = "ST_SetPreviewAudioTime";
	PreviewServerTypeMap.typeMap[1][6] = "ST_SplashSrcReceived";
	PreviewServerTypeMap.typeMap[1][7] = "ST_DeleteRecords";
	PreviewServerTypeMap.typeMap[1][8] = "ST_SetAddChatMessage";
	PreviewServerTypeMap.typeMap[1][9] = "ST_SetPreviewDefaultImage";
	PreviewServerTypeMap.typeMap[1][10] = "ST_SetAddEvent";
	PreviewServerTypeMap.typeMap[1][11] = "ST_SetAddStatistic";

	sharingServerTypeMap.typeMap[0][1] = "ST_StartSession";
	sharingServerTypeMap.typeMap[0][2] = "ST_KeyFrameOut";
	sharingServerTypeMap.typeMap[0][3] = "ST_FrameOut";
	sharingServerTypeMap.typeMap[0][4] = "ST_MissedSessionName";
	sharingServerTypeMap.typeMap[0][5] = "ST_StartViewSession";
	sharingServerTypeMap.typeMap[0][6] = "ST_StopSession";
	sharingServerTypeMap.typeMap[0][7] = "ST_StartRecording";
	sharingServerTypeMap.typeMap[0][8] = "ST_StopRecording";
	sharingServerTypeMap.typeMap[0][9] = "ST_StopSessionView";
	sharingServerTypeMap.typeMap[0][10] = "ST_StartFilePlay";
	sharingServerTypeMap.typeMap[0][11] = "ST_ResumeRecord";
	sharingServerTypeMap.typeMap[0][12] = "ST_PauseRecord";
	sharingServerTypeMap.typeMap[0][13] = "ST_RewindRecord";
	sharingServerTypeMap.typeMap[0][14] = "ST_StartSendFrames";
	sharingServerTypeMap.typeMap[0][15] = "ST_OnlyKeyFrames";
	sharingServerTypeMap.typeMap[0][16] = "ST_AllFrames";
	sharingServerTypeMap.typeMap[0][17] = "ST_RemoteControl";
	sharingServerTypeMap.typeMap[0][18] = "ST_AllowRemoteControl";
	sharingServerTypeMap.typeMap[0][19] = "ST_DenyRemoteControl";
	sharingServerTypeMap.typeMap[0][20] = "ST_DeleteRecord";
	sharingServerTypeMap.typeMap[0][21] = "ST_HideSession";

	sharingServerTypeMap.typeMap[1][1] = "RT_SessionStarted";
	sharingServerTypeMap.typeMap[1][2] = "RT_ViewSessionStarted";
	sharingServerTypeMap.typeMap[1][3] = "RT_SessionNotFound";
	sharingServerTypeMap.typeMap[1][4] = "RT_SessionAlreadyExists";
	sharingServerTypeMap.typeMap[1][5] = "RT_FramesPackReceived";
	sharingServerTypeMap.typeMap[1][6] = "RT_SessionClosed";
	sharingServerTypeMap.typeMap[1][7] = "RT_StartRecording";
	sharingServerTypeMap.typeMap[1][8] = "RT_WaitOwner";
	sharingServerTypeMap.typeMap[1][9] = "RT_RemoteCommand";
	sharingServerTypeMap.typeMap[1][10] = "RT_AllowRemoteControl";
	sharingServerTypeMap.typeMap[1][11] = "RT_DenyRemoteControl";

	serviceServerTypeMap.typeMap[0][1] = "RT_GetService";
	serviceServerTypeMap.typeMap[0][2] = "RT_ExecMethodResults";
	serviceServerTypeMap.typeMap[0][3] = "RT_GetServiceByMethod";
	serviceServerTypeMap.typeMap[0][4] = "RT_Notification";
	serviceServerTypeMap.typeMap[0][5] = "RT_PreviewServerRequest";
	serviceServerTypeMap.typeMap[0][6] = "RT_VoipServerRequest";
	serviceServerTypeMap.typeMap[0][7] = "RT_ServiceLoadingFailed";
	serviceServerTypeMap.typeMap[0][8] = "RT_VoipServerForRecordRequest";
	serviceServerTypeMap.typeMap[0][9] = "RT_SharingServerRequest";

	serviceServerTypeMap.typeMap[1][1] = "ST_GetService";
	serviceServerTypeMap.typeMap[1][2] = "ST_ExecMethod";
	serviceServerTypeMap.typeMap[1][3] = "ST_RegisterService";
	serviceServerTypeMap.typeMap[1][4] = "ST_PreviewServerRequest";
	serviceServerTypeMap.typeMap[1][5] = "ST_VoipServerRequest";
	serviceServerTypeMap.typeMap[1][6] = "ST_RegisterRecord";
	serviceServerTypeMap.typeMap[1][8] = "ST_VoipServerForRecordRequest";
	serviceServerTypeMap.typeMap[1][9] = "ST_SetSplashSrc";
	serviceServerTypeMap.typeMap[1][11] = "ST_SharingServerRequest";

//	LeaveCriticalSection(&csTypeListMap);
}

void CSocketTab::SetContext( oms::omsContext *apContext)
{
	mpOmsContext= apContext;
	currentSession = NULL;
	m_building = false;
	m_writeLog = false;
}

LRESULT CSocketTab::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos( 0, &rcDefault, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

//	m_testTree.Attach(GetDlgItem( IDC_TREE_SOCKETS));
	m_resultsEdit.Attach(GetDlgItem( IDC_SOCKET_SESSION_VIEW));
	m_startStopWriteLogBtn.Attach(GetDlgItem( IDC_BUTTON_UPDATE_SESSIONS));

	m_building = true;
	BuildTree();
	m_building = false;

	bHandled = FALSE;
	return 0;
}

void CSocketTab::BuildTree()
{
	std::string info = mpOmsContext->mpComMan->GetSessionsInfoList();
	m_resultsEdit.SetWindowText(info.c_str());
}

LRESULT CSocketTab::OnBnClickedStartWriteLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_writeLog = !m_writeLog;
	if (m_writeLog)
		m_startStopWriteLogBtn.SetWindowText("Stop Write Log");
	else
		m_startStopWriteLogBtn.SetWindowText("Start Write Log");

	mpOmsContext->mpComMan->changeLogWriteEnabled(m_writeLog);
		return 0;
}

void CSocketTab::Update()
{
	BuildTree();
}

void CSocketTab::TraceSessionPacketInfo(std::string& asSessionName, BYTE packetType, DWORD size, bool isOut)
{
	std::string log = asSessionName + ": ";
	std::string strType = "unknown";
	SessionsTypeListMap::iterator it;
	std::stringstream type;
	type << (unsigned int)packetType;

//	EnterCriticalSection(&csTypeListMap);
	if ( (it = typeListMap.find(asSessionName)) != typeListMap.end())
	{
		if ((*(it->second)).typeMap[isOut ? 1 : 0][packetType] != "")
			strType = (*(it->second)).typeMap[isOut ? 1 : 0][packetType];
	}
//	LeaveCriticalSection(&csTypeListMap);
	log += strType + "(" + type.str() + ")";
	log += "\t";
	log += "size = ";
	std::stringstream s;
	s << size;
	log += s.str();
	log += "\t";
	log += isOut ? "OUT" : "IN";
	mpOmsContext->mpLogWriter->WriteLn(log);
}