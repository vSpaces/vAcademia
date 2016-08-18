#pragma once
#ifdef WIN32
#define SYNCMAN_NO_VTABLE __declspec(novtable)
#else
#define SYNCMAN_NO_VTABLE
#endif

namespace SyncManProtocol
{
	enum sysPropertyType{
		/// <summary>Флаги</summary>
		SYS_Flags,
		/// <summary>Владелец</summary>
		SYS_Owner,
		/// <summary>Зона</summary>
		SYS_ZoneID,
		/// <summary>Реальность</summary>
		SYS_Reality,
		/// <summary>На связи</summary>
		SYS_Connected
	};

	enum syncUpdateType{
		UT_BOOL,
		UT_INT,
		UT_DOUBLE,
		UT_STRING,
		UT_OBJECT
	};

	struct SYNCMAN_NO_VTABLE syncIPropertyList;

	struct SYNCMAN_NO_VTABLE syncIPropertyInfo
	{
		virtual bool GetNameLength(unsigned short& aValue) = 0;
		virtual bool GetNameData( wchar_t* &aValue) = 0;
		virtual bool GetType( unsigned char& aValue) = 0;
		virtual bool GetValueBool(bool &aValue) = 0;
		virtual bool GetValueInt(int &aValue) = 0;
		virtual bool GetValueDouble(double &aValue) = 0;
		virtual bool GetValueStringLength(unsigned short &aValue) = 0;
		virtual bool GetValueString(const wchar_t* &aValue) = 0;
		//virtual bool GetValueObject( syncIPropertyList *aProperties, int &aPropertiesCount) = 0;
		virtual bool GetValueObject( syncIPropertyList* &aProperties) = 0;
	};

	struct SYNCMAN_NO_VTABLE syncIPropertyList
	{
		virtual unsigned short GetCount() = 0;
		virtual syncIPropertyInfo *GetProperty(int auIndex) = 0;
	};

	struct SYNCMAN_NO_VTABLE syncIUpdatePacketIn
	{
		virtual unsigned int GetObjectID() = 0;
		virtual unsigned int GetBornRealityID() = 0;
		virtual unsigned short GetVersion() = 0;
		virtual bool GetFlags( unsigned int& aValue) = 0;
		virtual bool GetOwnerLength(unsigned short& aValue) = 0;
		virtual bool GetOwnerData( wchar_t* &aValue) = 0;
		virtual bool GetZoneID( unsigned int& aValue) = 0;
		virtual bool GetRealityID( unsigned int& aValue) = 0;
		virtual bool GetConnected( bool& aValue) = 0;
		virtual bool GetProperties(syncIPropertyList* &aPropertyList) = 0;
		virtual bool Release()=0;
	};

	struct SYNCMAN_NO_VTABLE syncIUpdatePacketOut
	{
		virtual void BeginSysProperty() = 0;
		virtual void EndSysProperty() = 0;
		virtual void AddFlags( unsigned int aValue) = 0;
		virtual void AddOwner( const wchar_t* aValue) = 0;
		virtual void AddZone( unsigned int aValue) = 0;
		virtual void AddReality( unsigned int aValue) = 0;
		virtual void AddConnected( bool isConnected) = 0;
		virtual void BeginProperties() = 0;
		virtual void EndProperty() = 0;
		virtual void AddPropertyBool( const wchar_t* aName, bool aValue) = 0;
		virtual void AddPropertyInt( const wchar_t* aName, int aValue) = 0;
		virtual void AddPropertyDouble( const wchar_t* aName, double aValue) = 0;
		virtual void AddPropertyString( const wchar_t* aName, unsigned short aValueSize, wchar_t* aValue) = 0;
		virtual void CreateObject( const wchar_t* aName) = 0;
		virtual void CloseObject() = 0;
	};
}
