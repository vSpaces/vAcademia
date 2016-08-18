#pragma once
#include "imessagehandler.h"

#define FILE_INFO_QUERY_ID							1
#define FILE_READ_QUERY_ID							2
#define FILE_MULTI_INFO_QUERY_ID					3
#define FILE_ROOT_INFO_QUERY_ID						4
#define NOTIFY_NEW_FILES_INFO_LIST_ID				5
#define NOTIFY_FILE_ROOT_INFO_QUERY_ID				6
#define PACK_UPDATE_CLIENT_FILES_AND_INFO_QUERY_ID	7
#define FILE_ROOT_INFO2_QUERY_ID					8
#define PACK_CLIENT_SETUP_ID						9
//#define WRITE_UPDATER_LOG_ID						10

//class CDataBuffer2;
class CResMan;

class CResSessionHandler : public cs::imessagehandler
{
public:
	CResSessionHandler();
	virtual ~CResSessionHandler();

	void SetResMan(CResMan *aResMan);

protected:
	BEGIN_IN_MESSAGE_MAP()
		IN_MESSAGE_SYNCH(FILE_INFO_QUERY_ID, OnFileInfo)
		IN_MESSAGE_SYNCH(FILE_READ_QUERY_ID, OnFileRead)
		IN_MESSAGE_SYNCH(FILE_MULTI_INFO_QUERY_ID, OnMultiFileInfo)
		IN_MESSAGE_SYNCH(FILE_ROOT_INFO_QUERY_ID, OnRootFileInfo)		
		IN_MESSAGE2(NOTIFY_NEW_FILES_INFO_LIST_ID, OnNotifyNewFileInfoList)
		IN_MESSAGE2(NOTIFY_FILE_ROOT_INFO_QUERY_ID, OnNotifyRootFileInfo)
		IN_MESSAGE_SYNCH(PACK_UPDATE_CLIENT_FILES_AND_INFO_QUERY_ID, OnPackUpdateClientFilesAndGetInfo)
		IN_MESSAGE_SYNCH(FILE_ROOT_INFO2_QUERY_ID, OnRootFileInfo2)
		IN_MESSAGE_SYNCH(PACK_CLIENT_SETUP_ID, OnPackClientSetup)
		//IN_MESSAGE_SYNCH(WRITE_UPDATER_LOG_ID, OnWriteUpdaterLog)
	END_IN_MESSAGE_MAP()

	BEGIN_SYNCH_MESSAGE_MAP()
		IS_SYNCH(FILE_INFO_QUERY_ID)
		IS_SYNCH(FILE_READ_QUERY_ID)
		IS_SYNCH(FILE_MULTI_INFO_QUERY_ID)
		IS_SYNCH(FILE_ROOT_INFO_QUERY_ID)
		IS_SYNCH(PACK_UPDATE_CLIENT_FILES_AND_INFO_QUERY_ID)
		IS_SYNCH(PACK_CLIENT_SETUP_ID)
		//IS_SYNCH(WRITE_UPDATER_LOG_ID)
	END_SYNCH_MESSAGE_MAP()

	int OnFileInfo(BYTE* aData, int aDataSize);
	int OnFileRead(BYTE* aData, int aDataSize);
	int OnMultiFileInfo(BYTE* aData, int aDataSize);
	int OnNotifyNewFileInfoList(BYTE* aData, int aDataSize);
	// Анализ полученной информации об  об ресурсах модулей сервера (root)
	int OnRootFileInfo(BYTE* aData, int aDataSize);
	int OnNotifyRootFileInfo(BYTE* aData, int aDataSize);
	int OnPackUpdateClientFilesAndGetInfo(BYTE* aData, int aDataSize);
	int OnRootFileInfo2(BYTE* aData, int aDataSize);
	int OnPackClientSetup(BYTE* aData, int aDataSize);
	//int OnWriteUpdaterLog(BYTE* aData, int aDataSize);
private:
	CResMan *pResMan;
};
