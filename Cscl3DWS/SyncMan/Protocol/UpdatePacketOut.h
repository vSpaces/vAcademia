#pragma once
#include "SyncProtocol.h"
#include "CommonPacketOut.h"
#include <vector>

using namespace std;
using namespace Protocol;
using namespace SyncManProtocol;

class CUpdatePacketOut :
	public syncIUpdatePacketOut,
	public CCommonPacketOut
{
public:
	CUpdatePacketOut( unsigned short aVersion, unsigned int anObjectID, unsigned int aBornRealityID);
	virtual ~CUpdatePacketOut(void);

	virtual void BeginSysProperty();
	virtual void EndSysProperty();
	virtual void AddFlags( unsigned int aValue);
	virtual void AddOwner( const wchar_t* aValue);
	virtual void AddZone( unsigned int aValue);
	virtual void AddReality( unsigned int aValue);
	virtual void AddConnected( bool aValue);
	virtual void BeginProperties();
	virtual void EndProperty();
	//virtual void AddProperty( const wchar_t* aName, unsigned short aValueSize, const unsigned char* aValue);
	virtual void AddPropertyBool( const wchar_t* aName, bool aValue);
	virtual void AddPropertyInt( const wchar_t* aName, int aValue);
	virtual void AddPropertyDouble( const wchar_t* aName, double aValue);
	virtual void AddPropertyString( const wchar_t* aName, unsigned short aValueSize, wchar_t* aValue);
	virtual void CreateObject( const wchar_t* aName);
	virtual void CloseObject();

private: 
	void AddSysProperty( sysPropertyType aType, bool aValue);
	void AddSysProperty( sysPropertyType aType, unsigned int aValue);
	void AddSysProperty( sysPropertyType aType, const wchar_t* aValue);

private:
	//CDataBuffer oData;
	unsigned int objectID;
	unsigned char sysPropCount;
	//vector<unsigned char> vSysPropCount;
	unsigned int sysPropCountOffset;
	//vector<unsigned int> vSysPropCountOffset;
	unsigned short propertiesCount;
	vector<unsigned short> vPropertiesCount;
	unsigned int propertiesCountOffset;
	vector<unsigned int> vPropertiesCountOffset;
	unsigned int bornRealityIDOffset;
	unsigned int realityID;
private:
	unsigned short iObjectsHierarchyDeep;
};
