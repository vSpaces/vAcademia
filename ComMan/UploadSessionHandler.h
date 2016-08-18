#pragma once
#include "imessagehandler.h"

#define FILE_INFO_QUERY_ID			1
#define FILE_WRITE_QUERY_ID			2
#define FILE_REMOVE_QUERY_ID		3
#define FILE_REMOVE_DIR_QUERY_ID	4

//class CDataBuffer2;
class CResMan;

class CUploadSessionHandler : public cs::imessagehandler
{
public:
	CUploadSessionHandler();
	virtual ~CUploadSessionHandler();

	void SetResMan(CResMan *aResMan);

protected:
	BEGIN_IN_MESSAGE_MAP()
		IN_MESSAGE_SYNCH(FILE_INFO_QUERY_ID, OnResInfo)
		IN_MESSAGE_SYNCH(FILE_WRITE_QUERY_ID, OnFileWrite)
		IN_MESSAGE_SYNCH(FILE_REMOVE_QUERY_ID, OnFileRemove)
		IN_MESSAGE_SYNCH(FILE_REMOVE_DIR_QUERY_ID, OnDirRemove)
	END_IN_MESSAGE_MAP()

	BEGIN_SYNCH_MESSAGE_MAP()
		IS_SYNCH(FILE_INFO_QUERY_ID)
		IS_SYNCH(FILE_WRITE_QUERY_ID)
		IS_SYNCH(FILE_REMOVE_QUERY_ID)
		IS_SYNCH(FILE_REMOVE_DIR_QUERY_ID)
	END_SYNCH_MESSAGE_MAP()

	int OnResInfo(BYTE* aData, int aDataSize);
	int OnFileWrite(BYTE* aData, int aDataSize);
	int OnFileRemove(BYTE* aData, int aDataSize);
	int OnDirRemove(BYTE* aData, int aDataSize);

private:
	CResMan *pResMan;

};
