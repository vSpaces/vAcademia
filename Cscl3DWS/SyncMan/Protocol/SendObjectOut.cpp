#include "stdafx.h"
#include "SendObjectOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SyncManProtocol{

CSendObjectOut::CSendObjectOut(){
	sysPropCount = 0;
	sysPropCountOffset = 0;
	propertiesCount = 0;
	propertiesCountOffset = 0;
}

void CSendObjectOut::AddHeader(const rmml::syncObjectState& aState)
{
	// Write header
	data.add(PROTOCOL_VERSION);
	BYTE type = aState.type;
	data.add(type);
	data.add(aState.uiID);
	data.add(aState.uiBornRealityID);
	data.add(aState.uiRealityID);
	data.add(aState.mwServerVersion);
	data.add(aState.mwClientVersion);
	data.add(aState.uiHashObj);
}

void CSendObjectOut::Create(const syncObjectState& aState)
{
	data.clear();
	AddHeader(aState);
	CreatePacket(aState);
}

void CSendObjectOut::AddSysProperties(const syncObjectState& aState)
{
	byte count = aState.sys_props.GetSysPropsCount();
	data.add(count);
	// Пишем протокол
	if (aState.sys_props.miSet)
	{
		if(aState.sys_props.miSet & MLSYNCH_FLAGS_SET_MASK)
		{	
			data.add((byte) SPT_FLAGS);
			data.add(aState.sys_props.miFlags);
		}
		if(aState.sys_props.miSet & MLSYNCH_OWNER_CHANGED_MASK)
		{
			data.add((byte) SPT_OWNER);
			int size = wcslen(aState.sys_props.msOwner.c_str());
			data.addData(sizeof(unsigned short), (BYTE*)&size);
			data.addData(size*2, (BYTE*)aState.sys_props.msOwner.c_str());
		}
		if(aState.sys_props.miSet & MLSYNCH_ZONE_CHANGED_MASK)
		{
			data.add((byte) SPT_ZONE);
			data.add(aState.sys_props.miZone);
		}
		if(aState.sys_props.miSet & MLSYNCH_COORDINATE_SET_MASK)
		{
			data.add((byte) SPT_COORDINATE_Z);
			data.add(aState.sys_props.mfCoordinateZ);
		}
		if (aState.sys_props.miSet & MLSYNCH_COORDINATE_XY_SET_MASK)
		{
			data.add((byte) SPT_COORDINATE_X);
			data.add(aState.sys_props.mfCoordinateX);
			data.add((byte) SPT_COORDINATE_Y);
			data.add(aState.sys_props.mfCoordinateY);
		}
		if(aState.sys_props.miSet & MLSYNCH_SERVERTIME_SET_MASK)
		{
			data.add((byte) SPT_SERVERTIME);
			data.add(aState.sys_props.serverTime);
		}
		if (aState.sys_props.miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
		{
			data.add((unsigned char) SPT_BIN_STATE);
			BSSCCode updatingType;
			unsigned int version;
			BYTE* binState;
			int binStateSize;
			aState.sys_props.mBinState.GetFullState(updatingType, version, binState, binStateSize);

			unsigned int size = sizeof(BYTE) + sizeof(BYTE) +
				sizeof(unsigned int) + sizeof(unsigned int) + binStateSize;
			data.add(size);
			data.add((BYTE)1);
			data.add((BYTE)updatingType);
			data.add(version);
			data.add((unsigned int)binStateSize, (BYTE*)binState);
		}
		if(aState.sys_props.miSet & MLSYNCH_LOCATION_CHANGED_MASK)
		{
			data.add((byte) SPT_LOCATION);
			data.addStringUnicode(wcslen(aState.sys_props.msLocation.c_str()), (BYTE*)aState.sys_props.msLocation.c_str());
		}
		if(aState.sys_props.miSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
		{
			data.add((byte) SPT_COMMUNICATION_AREA);
			data.addStringUnicode(wcslen(aState.sys_props.msCommunicationArea.c_str()), (BYTE*)aState.sys_props.msCommunicationArea.c_str());
		}
		if(aState.sys_props.miSet & MLSYNCH_DELETETIME_CHANGED_MASK)
		{
			data.add((byte) SPT_DELETETIME);
			data.add(aState.sys_props.mlDeleteTime);
		}
	}

}

void CSendObjectOut::AddProperties(const syncObjectState& aState)
{
	data.add((unsigned short)aState.props.size());
	std::vector<syncObjectProp>::const_iterator props_iter = aState.props.begin();
	for(props_iter=aState.props.begin(); props_iter!=aState.props.end(); props_iter++){
		syncObjectProp pProp = (*props_iter);
		// Храним в ASCII как было чтобы не париться. 
		data.add((unsigned short)strlen((*props_iter).name.c_str()),(BYTE *)(*props_iter).name.c_str());
		data.add(pProp.binValue);
	}
}

void CSendObjectOut::AddSubObjects(const syncObjectState& aState)
{
	data.add((unsigned short)aState.sub_objects.size());
	std::vector<syncObjectState>::const_iterator sub_object_iter = aState.sub_objects.begin();
	for(sub_object_iter=aState.sub_objects.begin(); sub_object_iter!=aState.sub_objects.end(); sub_object_iter++){
		syncObjectState pSub_object = (*sub_object_iter);
		// Храним в ASCII как было чтобы не париться. 
		data.add((unsigned short)strlen((*sub_object_iter).string_id.c_str()),(BYTE *)(*sub_object_iter).string_id.c_str());
		int iSubpacketSizePos = data.getPos();
		unsigned int uiSubpacketSize = 0;
		data.add(uiSubpacketSize);
		CreatePacket(pSub_object);
		int currentPos = data.getPos();
		uiSubpacketSize = currentPos - iSubpacketSizePos - 4;
		data.setPos( iSubpacketSizePos);
		data.add(uiSubpacketSize);
		data.setPos( 0);
	}
}

void CSendObjectOut::CreatePacket(const syncObjectState& aState)
{
	// Добавляем системные свойства
	AddSysProperties(aState);

	// Добавляем обычные свойства
	AddProperties(aState);

	// Добавляем подобъекты
	AddSubObjects(aState);
}



unsigned char* CSendObjectOut::GetData()
{
	return (BYTE*)(data);
}

unsigned int CSendObjectOut::GetDataSize()
{
	return data.getLength();
}

}