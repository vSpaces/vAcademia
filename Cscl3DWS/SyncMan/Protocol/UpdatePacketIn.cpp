#include "StdAfx.h"
#include "updatepacketin.h"
#include "ProtocolConsts.h"

const bool g_charTable[256] = {
	false, false, false, false, false, false, false, false, // 000
	false, false, false, false, false, false, false, false, // 008
	false, false, false, false, false, false, false, false, // 016
	false, false, false, false, false, false, false, false, // 024
	false, false, false, false, true , false, false, false, // 032
	false, false, false, false, false, false, true , false, // 040
	true , true , true , true , true , true , true , true , // 048
	true , true , false, false, false, false, false, false, // 056
	true , true , true , true , true , true , true , true , // 064
	true , true , true , true , true, true , true , true ,  // 072
	true , true , true , true , true , true , true , true , // 080
	true , true , true , false, false, false, false, true , // 088
	false, true , true , true , true , true , true , true , // 096
	true , true , true , true , true , true , true , true , // 104
	true , true , true , true , true , true , true , true , // 112
	true , true , true , false, false, false, false, false, // 120
	false, false, false, false, false, false, false, false, // 128
	false, false, false, false, false, false, false, false, // 136
	false, false, false, false, false, false, false, false, // 144
	false, false, false, false, false, false, false, false, // 152
	false, false, false, false, false, false, false, false, // 160
	false, false, false, false, false, false, false, false, // 168
	false, false, false, false, false, false, false, false, // 176
	false, false, false, false, false, false, false, false, // 184
	false, false, false, false, false, false, false, false, // 192
	false, false, false, false, false, false, false, false, // 200
	false, false, false, false, false, false, false, false, // 208
	false, false, false, false, false, false, false, false, // 216
	false, false, false, false, false, false, false, false, // 224
	false, false, false, false, false, false, false, false, // 232
	false, false, false, false, false, false, false, false, // 240
	false, false, false, false, false, false, false, false  // 248
};

bool IsPropertyOrObjectNameValid( const char* str)
{
	for (unsigned int i = 0; i < strlen( str)-1; i++)
	{
		if (!g_charTable[str[i]])
			return false;
	}
	return true;
}

const char* GetErrorPartMessage( EUpdatePacketPartError anError)
{
	if (anError == UPPE_NO_ERROR)
		return "";
	if ((anError & UPPE_HEADER) != 0)
		return "Header analyzing - ";
	if ((anError & UPPE_SYS_PROPERTIES) != 0)
		return "System properties analyzing - ";
	if ((anError & UPPE_PROPERTIES) != 0)
		return "Properties analyzing - ";
	if ((anError & UPPE_SUBOBJECTS) != 0)
		return "Sub objects analyzing - ";
	return "";
};

const char* GetErrorFieldMessage( EUpdatePacketFieldError anError)
{
	const char* errorFieldMessage[] =
	{
		"",
		"protocol version error",
		"incorrect protocol version",
		"born reality id error",
		"reality id error",
		"version error",
		"sys property count error",
		"sys property type error",
		"flag error",
		"owner error",
		"zone error",
		"connected error",
		"current realiry id error",
		"coordinate x error",
		"coordinate y error",
		"coordinate z error",
		"server time error",
		"binstate size error",
		"binstate error",
		"location error",
		"pause level error",
		"property count error",
		"property name error",
		"property name wrong error",
		"property value error",
		"subobject count error",
		"subobject name error",
		"subobject size error",
		"subobject size wrong error"
	};
	return errorFieldMessage[ anError];
}


CUpdatePacketIn::CUpdatePacketIn( BYTE *aData, int aDataSize) :
	CCommonPacketIn( aData, aDataSize),
	analizingErrorInfo( $error)
{
}

CUpdatePacketIn::~CUpdatePacketIn()
{
}

bool CUpdatePacketIn::Analyse(syncObjectState& aState)
{
	idx = 0;
	if (!CCommonPacketIn::Analyse())
		return false;
	bool isError = false;
	if (!AnalyseHeader(aState))
		isError = true;
	else if(!AnalyseBody(aState))
		isError = true;

	if (isError)
	{
		$error.message.insert( 0, "[ERROR][ANALYSE][UPDATE_PACKET_IN] ");
		return false;
	}

	return EndOfData( idx);
}

bool CUpdatePacketIn::AnalyseHeader(syncObjectState& aState)
{
	bool isError = false;
	// Header
	if(isError = !memparse_object(pData, idx, protocolVersion))
		$error.SetFieldError( UPPE_HEADER, UPFE_PROTOCOL_VERSION, idx);
	else if(isError = (protocolVersion != PROTOCOL_VERSION))
		$error.SetFieldError( UPPE_HEADER, UPFE_PROTOCOL_VERSION_INCORRECT, idx);
	else
	{
		byte type;
		if(isError = !memparse_object(pData, idx, type))
			$error.SetFieldError( UPPE_HEADER, UPFE_OBJECT_ID, idx);
		aState.type = (syncObjectState::StateType) type;

		if(isError = !memparse_object(pData, idx, aState.uiID))
			$error.SetFieldError( UPPE_HEADER, UPFE_OBJECT_ID, idx);

		else if(isError = !memparse_object(pData, idx, aState.uiBornRealityID))
			$error.SetFieldError( UPPE_HEADER, UPFE_BORN_REALITY_ID, idx);

		else if(isError = !memparse_object(pData, idx, aState.uiRealityID))
			$error.SetFieldError( UPPE_HEADER, UPFE_REALITY_ID, idx);

		else if(isError = !memparse_object(pData, idx, aState.mwServerVersion))
			$error.SetFieldError( UPPE_HEADER, UPFE_VERSION, idx);

		else if(isError = !memparse_object(pData, idx, aState.mwClientVersion))
			$error.SetFieldError( UPPE_HEADER, UPFE_VERSION, idx);
		aState.mwClientVersion = aState.mwServerVersion;

		if (isError = !memparse_object(pData, idx, aState.uiHashObj))
			$error.SetFieldError( UPPE_HEADER, UPFE_VERSION, idx);
		if (isError)
			return false;

		return true;
	}
	return false;
}

bool CUpdatePacketIn::AnalyseBody( syncObjectState& aState )
{
	if(!AnalyseSysProperties(aState))
		return false;

	if(!AnalyseProperties(aState))
		return false;

	if(!AnalyseSubobjects(aState))
		return false;

	return true;
}

bool CUpdatePacketIn::AnalyseSysProperties(syncObjectState& aState)
{
	bool isError = false;

	unsigned char systemPropertyCount;
	if(isError = !memparse_object(pData, idx, systemPropertyCount))
		$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_SYS_PROPERTY_COUNT, idx);

	for (int i = 0;  !isError && i < systemPropertyCount;  i++)
	{
		byte systemPropertyType;
		if(isError = !memparse_object(pData, idx, systemPropertyType))
		{
			$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_SYS_PROPERTY_TYPE, idx);
			break;
		}

		switch ((ESysPropertyType)systemPropertyType)
		{
			case SPT_FLAGS:
				aState.sys_props.miSet |= MLSYNCH_FLAGS_SET_MASK;
				if(isError = !memparse_object(pData, idx, aState.sys_props.miFlags))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_FLAG, idx);
				break;
			case SPT_OWNER:
				aState.sys_props.miSet |= MLSYNCH_OWNER_CHANGED_MASK;
				if (isError = !memparse_string( pData, idx, aState.sys_props.msOwner))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_OWNER, idx);
				break;
			case SPT_ZONE:
				aState.sys_props.miSet |= MLSYNCH_ZONE_CHANGED_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.miZone))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_ZONE, idx);
				break;
			case SPT_CONNECTED:
				aState.sys_props.miSet|= MLSYNCH_CONNECTED_CHANGED_MASK;
				if(isError = !memparse_object(pData, idx, aState.sys_props.mbConnected))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_CONNECTED, idx);
				break;
			case SPT_REALITY:
				aState.sys_props.miSet |= MLSYNCH_REALITY_SET_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.muReality))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_CURRENT_REALIRY_ID, idx);
				break;
			case SPT_COORDINATE_Z:
				aState.sys_props.miSet |= MLSYNCH_COORDINATE_SET_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.mfCoordinateZ))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_COORDINATE_Z, idx);
				break;
			case SPT_COORDINATE_X:
				aState.sys_props.miSet |= MLSYNCH_COORDINATE_XY_SET_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.mfCoordinateX))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_COORDINATE_X, idx);
				break;
			case SPT_COORDINATE_Y:
				aState.sys_props.miSet |= MLSYNCH_COORDINATE_XY_SET_MASK;
				if(isError = !memparse_object(pData, idx, aState.sys_props.mfCoordinateY))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_COORDINATE_Y, idx);
				break;
			case SPT_SERVERTIME:
				aState.sys_props.miSet |= MLSYNCH_SERVERTIME_SET_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.serverTime))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_SERVER_TIME, idx);
				break;
			case SPT_BIN_STATE:
			{
				aState.sys_props.miSet |= MLSYNCH_BIN_STATE_CHANGED_MASK;

				unsigned int size = 0;
				BYTE count = 0;
				BYTE updatingType;
				unsigned int version;
				int binStateSize;

				if (isError = !memparse_object(pData, idx, size))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE_SIZE, idx);
				else if (isError = !memparse_object(pData, idx, count))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
				else if (isError = (count != 1))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
				else if (isError = !memparse_object(pData, idx, updatingType))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
				else if (isError = !memparse_object(pData, idx, version))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
				else if (isError = !memparse_object(pData, idx, binStateSize))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
				else
				{
					// Если сработает, звать Алекса для исправления!
					//assert(binStateSize != 0);
					unsigned char* binState = NULL;
					// закомментировано так как память выделяется в memparse_data
					/*if (binStateSize != 0)
						binState = MP_NEW_ARR( unsigned char, binStateSize);*/

					int expectedSize = sizeof(BYTE) + sizeof(BYTE) +
						sizeof(unsigned int) + sizeof(unsigned int) + binStateSize;

					if (isError = (size != expectedSize))
						$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
					else if (isError = !memparse_data(pData, idx, binStateSize, &binState))
						$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_BINSTATE, idx);
					else
						aState.sys_props.mBinState.ApllyUpdating((BSSCCode)updatingType, version, binState, binStateSize);

					if (binState != NULL)
						MP_DELETE_ARR( binState);
				}
				break;
			}
			case SPT_LOCATION:
				aState.sys_props.miSet |= MLSYNCH_LOCATION_CHANGED_MASK;
				if (isError = !memparse_string( pData, idx, aState.sys_props.msLocation))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_LOCATION, idx);
				break;
			case SPT_COMMUNICATION_AREA:
				aState.sys_props.miSet |= MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK;
				if (isError = !memparse_string( pData, idx, aState.sys_props.msCommunicationArea))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_COMMUNICATION_AREA, idx);
				break;
			case SPT_PAUSELEVEL:
				aState.sys_props.miSet |= MLSYNCH_PAUSE_LEVEL_CHANGED_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.muPauseLevel))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_PAUSE_LEVEL, idx);
				break;
			case SPT_DELETETIME:
				aState.sys_props.miSet |= MLSYNCH_DELETETIME_CHANGED_MASK;
				if (isError = !memparse_object(pData, idx, aState.sys_props.mlDeleteTime))
					$error.SetFieldError( UPPE_SYS_PROPERTIES, UPFE_DELETE_TIME, idx);
				break;
		}
	}

	if (isError)
		return false;
	else
		return true;
}

bool CUpdatePacketIn::AnalyseProperties(syncObjectState& aState)
{
	bool isError = false;

	unsigned short propertyCount = 0;
	if (isError = !memparse_object(pData, idx, propertyCount))
		$error.SetFieldError( UPPE_PROPERTIES, UPFE_PROPERTY_COUNT, idx);

	for (int i = 0;  !isError && i < propertyCount;  i++)
	{
		syncObjectProp objectProperty;
		if (isError = !memparse_string(pData, idx, objectProperty.name))
			$error.SetFieldError( UPPE_PROPERTIES, UPFE_PROPERTY_NAME, idx);
		else if (isError = !IsPropertyOrObjectNameValid(objectProperty.name.c_str()))
			$error.SetFieldError( UPPE_PROPERTIES, UPFE_PROPERTY_NAME_WRONG, idx);
		else if (isError = !memparse_object(pData, idx, objectProperty.binValue))
			$error.SetFieldError( UPPE_PROPERTIES, UPFE_PROPERTY_VALUE, idx);
		else
			aState.props.push_back( objectProperty);
	}

	if (isError)
		return false;
	else
		return true;
}

class CUpdateSubpacketIn : public CUpdatePacketIn
{
public:
	CUpdateSubpacketIn( BYTE *aData, int aDataSize) : CUpdatePacketIn( aData, aDataSize)
	{
	}

protected:
	virtual bool AnalyseHeader(syncObjectState& aState)
	{
		return true;
	};
};

bool CUpdatePacketIn::AnalyseSubobjects( syncObjectState& aState)
{
	if (EndOfData( idx))
		return true;

	bool isError = false;

	unsigned short subobjectCount = 0;
	if (isError = !memparse_object(pData, idx, subobjectCount))
		$error.SetFieldError( UPPE_SUBOBJECTS, UPFE_SUBOBJECT_COUNT, idx);

	for(int i = 0;  !isError && i < subobjectCount;  i++)
	{
		syncObjectState substate;
		int subpacketSize = 0;

		if (isError = !memparse_string(pData, idx, substate.string_id))
			$error.SetFieldError( UPPE_SUBOBJECTS, UPFE_SUBOBJECT_NAME, idx);
		else if (isError = !memparse_object(pData, idx, subpacketSize))
			$error.SetFieldError( UPPE_SUBOBJECTS, UPFE_SUBOBJECT_SIZE, idx);
		else
		{
			CUpdateSubpacketIn subpacket( pData + idx, subpacketSize);
			if (isError = !subpacket.Analyse(substate))
			{
				$error.SetPartError( UPPE_SUBOBJECTS);
				ATLASSERT( FALSE);
			}
			else
			{
				aState.sub_objects.push_back( substate);
				idx += subpacketSize;
			}
		}
	}

	if (isError)
		return false;
	else
		return true;
}

CUpdatePacketError::CUpdatePacketError() :
	partError( UPPE_NO_ERROR),
	fieldError( UPFE_NO_ERROR),
	MP_STRING_INIT(message)
{
}

void CUpdatePacketError::SetPartError( EUpdatePacketPartError aPartError)
{
	partError = (EUpdatePacketPartError) (partError | aPartError);
	message.insert( 0, GetErrorPartMessage( aPartError));
}

void CUpdatePacketError::SetFieldError( EUpdatePacketPartError aPartError, EUpdatePacketFieldError aFieldError, int anOffset)
{
	SetPartError( aPartError);
	fieldError = aFieldError;
	message += GetErrorFieldMessage( aFieldError);
	errorOffset = anOffset;
}
