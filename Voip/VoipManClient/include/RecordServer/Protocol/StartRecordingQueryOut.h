#pragma once
#include "..\RecordServer\Protocol\CommonVoipPacketOut.h"

class CStartRecordingQueryOut : public CCommonVoipPacketOut
{
public:
	CStartRecordingQueryOut(LPCSTR alpcVoipServerAddress, const wchar_t* alwcRoomName, unsigned int aRecordID);
};

class CChangeRecordServerQueryOut : public CCommonVoipPacketOut
{
public:
	CChangeRecordServerQueryOut(LPCSTR alpcVoipServerAddress, const wchar_t* alwcRoomName, unsigned int aRecordID)
	{
		ATLASSERT( alwcRoomName);
		USES_CONVERSION;
		GetDataBuffer().addString( A2W(alpcVoipServerAddress));
		GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
		GetDataBuffer().addString( alwcRoomName);
	}
};

class CChangePlayServerQueryOut : public CCommonVoipPacketOut
{
public:
	CChangePlayServerQueryOut(LPCSTR alpcVoipServerAddress, const wchar_t* alwcRoomName, unsigned int aRecordID, unsigned int aRealityID)
	{
		ATLASSERT( alwcRoomName);
		USES_CONVERSION;
		GetDataBuffer().addString( A2W(alpcVoipServerAddress));
		GetDataBuffer().addData( &aRecordID, sizeof(aRecordID));
		GetDataBuffer().addData( &aRealityID, sizeof(aRealityID));
		GetDataBuffer().addString( alwcRoomName);
	}
};

class CSetUserNameQueryOut : public CCommonVoipPacketOut
{
public:
	CSetUserNameQueryOut(LPCSTR alpcUserName)
	{
		ATLASSERT( alpcUserName);
		USES_CONVERSION;
		GetDataBuffer().addString( A2W(alpcUserName));
	}
};

class CRecordIDQueryOut : public CCommonVoipPacketOut
{
public:
	CRecordIDQueryOut( unsigned int auiRecordID)
	{
		GetDataBuffer().add( auiRecordID);
	}
};

class CForceSynchronizationQueryOut : public CCommonVoipPacketOut
{
public:
	CForceSynchronizationQueryOut( unsigned int auiRecordID, int aiUserID)
	{
		GetDataBuffer().add( auiRecordID);
		GetDataBuffer().add( aiUserID);
	}
};

class CPauseResumeUserAudioQueryOut : public CCommonVoipPacketOut
{
public:
	CPauseResumeUserAudioQueryOut( LPCSTR alpcRoomName, unsigned int auRealityID)
	{
		ATLASSERT( alpcRoomName);
		USES_CONVERSION;
		GetDataBuffer().addString( A2W(alpcRoomName));
		GetDataBuffer().add( auRealityID);
	}
};