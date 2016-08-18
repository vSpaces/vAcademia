#pragma once

namespace SyncManProtocol{
	class CProps{
	public:
		unsigned char *name;
		unsigned short nameLen;
		BYTE *value;
		unsigned short valueLen;
	};
	class CReceiveSyncDataIn : public CCommonPacketIn
	{
	protected:
		unsigned int iObjectID;
		unsigned int idBornRealityID;
		unsigned short iVersion;
		unsigned int flags;
		bool flagsIsSet;

		wchar_t *owner;
		bool ownerIsSet;

		unsigned int iZoneID;
		bool zoneIDIsSet;

		unsigned int iRealityID;
		bool realityIDIsSet;


		unsigned short propCount;
		CProps *props;
	public:
		CReceiveSyncDataIn(BYTE *aData, int aDataSize):CCommonPacketIn(aData, aDataSize){
			flagsIsSet = false;
			ownerIsSet = false;
			zoneIDIsSet = false;
			realityIDIsSet = false;
			propCount = 0;
			props = NULL;
		};
		~CReceiveSyncDataIn();

		bool Analyse();
	protected:
		bool AnalyseSysProperties(int &idx);
		bool AnalyseProperties(int &idx);
	
	public:
		unsigned int GetObjectID();
		unsigned int GetBornRealityID();
		unsigned short GetVersion();
		bool GetFlags(unsigned int &aValue);
		//bool GetOwner(wchar_t **aValue, unsigned short *len);
		wchar_t *GetOwner(bool &aownerIsSet);
		unsigned int GetZoneID(bool &azoneIDIsSet);
		//bool GetZoneID(unsigned int &aValue);
		short GetPropertiesCount();
		unsigned char *GetPropertyName( int anIndex, unsigned short *len);
		BYTE *GetPropertyValue( int anIndex, unsigned short *len);
	};
};