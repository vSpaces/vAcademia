#pragma once

enum 
{
	/// <summary>Флаги</summary>
	FLAGS = 0,
	/// <summary>Владелец</summary>
	OWNER = 1,
	/// <summary>Зона</summary>
	ZONEID = 2,
	/// <summary>Реальность</summary>
	REALITY = 3,
	/// <summary>На связи</summary>
	CONNECTED = 4,
	/// <summary>Z координаты</summary>
	COORDINATEZ = 5,
	/// <summary>Бинарное состояние</summary>
	BINSTATE = 6,
	/// <summary>Серверное время</summary>
	SSERVERTIME = 7
};

enum EUpdatePacketPartError
{
	UPPE_NO_ERROR = 0,
	UPPE_HEADER = 0x01,
	UPPE_SYS_PROPERTIES = 0x02,
	UPPE_PROPERTIES = 0x04,
	UPPE_SUBOBJECTS = 0x08
};

enum EUpdatePacketFieldError
{
	UPFE_NO_ERROR = 0,
	UPFE_PROTOCOL_VERSION,
	UPFE_PROTOCOL_VERSION_INCORRECT,
	UPFE_OBJECT_ID,
	UPFE_BORN_REALITY_ID,
	UPFE_REALITY_ID,
	UPFE_VERSION,
	UPFE_HASHOBJ,
	UPFE_SYS_PROPERTY_COUNT,
	UPFE_SYS_PROPERTY_TYPE,
	UPFE_FLAG,
	UPFE_OWNER,
	UPFE_ZONE,
	UPFE_CONNECTED,
	UPFE_CURRENT_REALIRY_ID,
	UPFE_COORDINATE_X,
	UPFE_COORDINATE_Y,
	UPFE_COORDINATE_Z,
	UPFE_SERVER_TIME,
	UPFE_BINSTATE_SIZE,
	UPFE_BINSTATE,
	UPFE_LOCATION,	
	UPFE_PAUSE_LEVEL,
	UPFE_PROPERTY_COUNT,
	UPFE_PROPERTY_NAME,
	UPFE_PROPERTY_NAME_WRONG,
	UPFE_PROPERTY_VALUE,
	UPFE_SUBOBJECT_COUNT,
	UPFE_SUBOBJECT_NAME,
	UPFE_SUBOBJECT_SIZE,
	UPFE_SUBOBJECT_SIZE_WRONG,
	UPFE_DELETE_TIME,
	UPFE_COMMUNICATION_AREA,
	UPFE_MAX
};

struct CUpdatePacketError
{
	unsigned short protocolVersion;
	EUpdatePacketPartError partError;
	EUpdatePacketFieldError fieldError;
	int errorOffset;
	MP_STRING message;

	CUpdatePacketError();
	void SetPartError( EUpdatePacketPartError aPartError);
	void SetFieldError( EUpdatePacketPartError aPartError, EUpdatePacketFieldError aFieldError, int anOffset);
};

class CUpdatePacketIn : public CCommonPacketIn
{
private:
	unsigned short protocolVersion;
	int idx;
	CUpdatePacketError $error;

public:
	CUpdatePacketIn(BYTE *aData, int aDataSize);
	virtual ~CUpdatePacketIn();

	// Анализирует пакет, заполняет состояние объекта
	virtual bool Analyse(syncObjectState& state);

	// Описание ошибки анализа пакета
	const CUpdatePacketError& analizingErrorInfo;

protected:
	virtual bool AnalyseHeader(syncObjectState& aState);
	virtual bool AnalyseBody(syncObjectState& aState);
	virtual bool AnalyseSysProperties(syncObjectState& aState);
	virtual bool AnalyseProperties(syncObjectState& aState);
	virtual bool AnalyseSubobjects(syncObjectState& aState);
};
