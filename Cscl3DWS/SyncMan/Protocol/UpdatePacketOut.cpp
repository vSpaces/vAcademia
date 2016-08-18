#include "StdAfx.h"
#include "updatepacketout.h"

CUpdatePacketOut::CUpdatePacketOut( unsigned short aVersion, unsigned int anObjectID, unsigned int aBornRealityID)
{
	data.clear();
	data.add(anObjectID);
	data.add(aBornRealityID);
	data.add(aVersion);
	iObjectsHierarchyDeep = 0;
	propertiesCount = 0;
}

CUpdatePacketOut::~CUpdatePacketOut()
{
}

void CUpdatePacketOut::BeginSysProperty()
{
	sysPropCount = 0;
	sysPropCountOffset = data.getLength();
	data.add( sysPropCount);
}

void CUpdatePacketOut::EndSysProperty()
{
	data.setData((BYTE*)&sysPropCount, 1, sysPropCountOffset);
}

void CUpdatePacketOut::AddSysProperty( sysPropertyType aType, bool aValue)
{
	if (propertiesCount > 0)
		return;
	data.add( (byte) aType);
	data.add( aValue);
	sysPropCount++;
}

void CUpdatePacketOut::AddSysProperty( sysPropertyType aType, unsigned int aValue)
{
	if (propertiesCount > 0)
		return;
	data.add( (byte) aType);
	data.add( aValue);
	sysPropCount++;
}

void CUpdatePacketOut::AddSysProperty( sysPropertyType aType, const wchar_t* aValue)
{
	if (propertiesCount > 0)
		return;
	int iLen = wcslen(aValue);
	if(iLen<1)
		return;
	data.add( (byte) aType);
	data.addStringUnicode(iLen, (BYTE*)aValue);
	sysPropCount++;
}

void CUpdatePacketOut::AddFlags( unsigned int aValue)
{
	AddSysProperty(SYS_Flags, aValue);
}

void CUpdatePacketOut::AddOwner( const wchar_t* aValue)
{
	AddSysProperty(SYS_Owner, aValue);
}

void CUpdatePacketOut::AddZone( unsigned int aValue)
{
	AddSysProperty(SYS_ZoneID, aValue);
}

void CUpdatePacketOut::AddReality( unsigned int aValue)
{
	AddSysProperty(SYS_Reality, aValue);
}

void CUpdatePacketOut::AddConnected( bool aValue)
{
	AddSysProperty(SYS_Connected, aValue);
}

void CUpdatePacketOut::BeginProperties()
{
	propertiesCount = 0;
	propertiesCountOffset = data.getLength();
	data.add( propertiesCount);
}

void CUpdatePacketOut::EndProperty()
{
	data.setData((BYTE*)&propertiesCount, 1, propertiesCountOffset);
}

void CUpdatePacketOut::AddPropertyBool( const wchar_t* aName, bool aValue)
{
	data.addStringUnicode(wcslen(aName), (BYTE*)aName);
	data.add((BYTE)UT_BOOL);
	data.add(aValue);
	propertiesCount++;
}

void CUpdatePacketOut::AddPropertyInt( const wchar_t* aName, int aValue)
{
	data.addStringUnicode(wcslen(aName), (BYTE*)aName);
	data.add((BYTE)UT_INT);
	data.add(aValue);
	propertiesCount++;
}

void CUpdatePacketOut::AddPropertyDouble( const wchar_t* aName, double aValue)
{
	data.addStringUnicode(wcslen(aName), (BYTE*)aName);
	data.add((BYTE)UT_DOUBLE);
	data.add(aValue);
	propertiesCount++;
}

void CUpdatePacketOut::AddPropertyString( const wchar_t* aName, unsigned short aValueSize, wchar_t* aValue)
{
	data.addStringUnicode(wcslen(aName), (BYTE*)aName);
	data.add((BYTE)UT_STRING);
	data.addStringUnicode(aValueSize, (BYTE*)aValue);
	propertiesCount++;
}

void CUpdatePacketOut::CreateObject( const wchar_t* aName)
{
	data.addStringUnicode(wcslen(aName), (BYTE*)aName);
	data.add((BYTE)UT_OBJECT);
	iObjectsHierarchyDeep++;
	propertiesCount++;
	vPropertiesCount.push_back(propertiesCount);
	propertiesCount = 0;
	vPropertiesCountOffset.push_back(propertiesCountOffset);
	BeginProperties();
	//propertiesCountOffset = data.GetBufferSize();
}

void CUpdatePacketOut::CloseObject()
{
	if(iObjectsHierarchyDeep<1)
		return;
	EndProperty();
	iObjectsHierarchyDeep--;
	propertiesCount = vPropertiesCount[iObjectsHierarchyDeep];
	vPropertiesCount.pop_back();
	propertiesCountOffset = vPropertiesCountOffset[iObjectsHierarchyDeep];
	vPropertiesCountOffset.pop_back();
}

