
#include "stdafx.h"
#include "oms/oms_context.h"
#include "rmml/rmml.h"
#include "synch.h"

//typedef std::map<std::wstring, unsigned short> MapIDs;
//typedef std::pair<std::wstring, unsigned short> Pair4MapIDs; 
//typedef std::map<std::wstring, unsigned short>::const_iterator Iter4MapIDs; 

//	MapIDs mMapStrID2ID;

using std::wstring;

omsresult wsSynchronizer::CallEvent(unsigned int auID, MLBYTE* apData, MLWORD auDataLength){
	mpContext->mpInput->AddCallEvent(0,auID,apData,auDataLength,0L);
	return OMS_OK;
}

omsresult wsSynchronizer::GetObjectID(const wchar_t* apwcStringID, unsigned int &auID){
	wstring sStrID(apwcStringID);
	Iter4MapIDs mi=mMapStrID2ID.find( MAKE_MP_WSTRING(sStrID));
	if(mi == mMapStrID2ID.end()){
		// объект не зарегистрирован
		// выделяем ему новый ID
		auID=mvID2StrID.size()+1;
		mvID2StrID.push_back( MAKE_MP_WSTRING( sStrID));
		mMapStrID2ID.insert(Pair4MapIDs( MAKE_MP_WSTRING(sStrID),auID));
	}else{
		auID=mi->second;
	}
	return OMS_OK;
}

omsresult wsSynchronizer::GetObjectStringID( unsigned int auID, rmml::mlOutString &asStringID){
	if(auID >= mvID2StrID.size()) return OMS_ERROR_FAILURE;
	asStringID=mvID2StrID[auID-1];
	return OMS_OK;
}

void wsSynchronizer::NotifyGetState(MLBYTE* apData, MLDWORD alSize){
	// ??
}

omsresult wsSynchronizer::SendStateChanges(MLBYTE* apData, int aiDataLength)
{
	return OMS_OK;
}
