
#ifndef _synch_h___
#define _synch_h___

#include <map>

typedef MP_MAP<MP_WSTRING, unsigned short> MapIDs;
typedef std::pair<MP_WSTRING, unsigned short> Pair4MapIDs; 
typedef MapIDs::const_iterator Iter4MapIDs; 

class wsSynchronizer: public rmml::mlISinchronizer
{
	oms::omsContext* mpContext;
	MapIDs mMapStrID2ID;
	MP_VECTOR<MP_WSTRING> mvID2StrID;
public:
	wsSynchronizer(oms::omsContext* apContext):
				MP_MAP_INIT( mMapStrID2ID), MP_VECTOR_INIT( mvID2StrID)
	{
		mpContext=apContext;
		mpContext->mpSynch=this;
	}
	~wsSynchronizer(){
		mvID2StrID.clear();
	}

// реализация mlISinchronizer
	omsresult CallEvent(unsigned int auID, MLBYTE* apData, MLWORD auDataLength);
	omsresult GetObjectID(const wchar_t* apwcStringID, unsigned int &auID);
	omsresult GetObjectStringID( unsigned int auID, rmml::mlOutString &asStringID);
	void NotifyGetState(MLBYTE* apData, MLDWORD alSize);
	omsresult SendStateChanges(MLBYTE* apData, int aiDataLength);
};

#endif