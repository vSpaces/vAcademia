#pragma once
#include "BSSCCode.h"
#include <any.hpp>
#include <string>
#include <vector>
#include <float.h>
#include "UtilTypes.h"
#include "vs_wrappers.h"

namespace rmml {

	// значение для muID, которое показывает, что элемент не является объектом мира
#define OBJ_ID_UNDEF 0xFFFFFFFF

#define MLSYNCH_ZONE_CHANGED_MASK (1 << 0)
	// 1й - изменился msOwner (попытка установить себя собственником или освободить объект)
#define MLSYNCH_OWNER_CHANGED_MASK (1 << 1)
	// 2й - установлены флаги (miFlags)
#define MLSYNCH_FLAGS_SET_MASK (1 << 2)
	// 3й - изменилась реальность (muReality)
#define MLSYNCH_REALITY_SET_MASK (1 << 3)
	// 4й - изменилась координата Z
#define MLSYNCH_COORDINATE_SET_MASK (1 << 4)
	// 5й - изменилось __connected
#define MLSYNCH_CONNECTED_CHANGED_MASK (1 << 5)
	// 6й - изменилось бинарное состояние
#define MLSYNCH_BIN_STATE_CHANGED_MASK (1 << 6)
	// 7й - изменилось состояние RMML-подобъекта
#define MLSYNCH_SUB_OBJ_CHANGED_MASK (1 << 7)
	// 8й - изменилось серверное время
#define MLSYNCH_SERVERTIMEUNUSED_SET_MASK (1 << 8)
	// 9й - изменилась локация
#define MLSYNCH_LOCATION_CHANGED_MASK (1 << 9)
	// 10й - изменилась уровень паузы
#define MLSYNCH_PAUSE_LEVEL_CHANGED_MASK (1 << 10)
	// 11й - изменились координаты X и Y
#define MLSYNCH_COORDINATE_XY_SET_MASK (1 << 11)
	// 12й - изменилось серверное время
#define MLSYNCH_SERVERTIME_SET_MASK (1 << 12)
	// 13й - изменилось время исчезновения
#define MLSYNCH_DELETETIME_CHANGED_MASK (1 << 13)
	// 14й - изменилась локация общения
#define MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK (1 << 14)

	// что надо отсылать при выдаче полного состояния, которое на сервер посылаем
#define MLSYNCH_FULL_STATE_MASK ( 0 \
	| MLSYNCH_FLAGS_SET_MASK              /* является признаком полного состояния */ \
	| MLSYNCH_OWNER_CHANGED_MASK          /* собственик */ \
	| MLSYNCH_ZONE_CHANGED_MASK           /* зона */ \
	| MLSYNCH_COORDINATE_SET_MASK         /* координата Z */ \
	| MLSYNCH_BIN_STATE_CHANGED_MASK      /* бинарное состояние */ \
	| MLSYNCH_SUB_OBJ_CHANGED_MASK        /* подобъекты */ \
	| MLSYNCH_LOCATION_CHANGED_MASK       /* идентификатор */ \
	| MLSYNCH_COORDINATE_XY_SET_MASK      /* координаты X, Y */ \
	| MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK/* идентификатор */ \
	)

	//	| MLSYNCH_DELETETIME_CHANGED_MASK     /* время исчезновения */ \

	// остальные системные сврйства с клиента не посылаются - они приходят с сервера:
	//   - MLSYNCH_REALITY_SET_MASK         - реальность (muReality)
	//   - MLSYNCH_CONNECTED_CHANGED_MASK
	//   - MLSYNCH_PAUSE_LEVEL_CHANGED_MASK - уровень паузы
	//   - MLSYNCH_SERVERTIME_SET_MASK      - серверное время

	enum ESysPropertyType {
		SPT_FLAGS = 0,
		SPT_OWNER,
		SPT_ZONE,
		SPT_REALITY,
		SPT_CONNECTED,
		SPT_COORDINATE_Z,
		SPT_BIN_STATE,
		SPT_SERVERTIMEUNUSED,
		SPT_LOCATION,
		SPT_PAUSELEVEL,
		SPT_COORDINATE_X,
		SPT_COORDINATE_Y,
		SPT_SERVERTIME,
		SPT_DELETETIME,
		SPT_COMMUNICATION_AREA,
		SPT_MAX
	};

	inline void AddString( std::wstring& aString, const std::wstring& anIndent, const std::wstring& anAddon)
	{
		aString += anIndent;
		aString += anAddon;
	}

	inline void AddString( std::wstring& aString, const std::wstring& anIndent, const wchar_t* anAddon)
	{
		aString += anIndent;
		aString += anAddon;
	}

	inline void AddString( std::wstring& aString, const char* anAddon)
	{
		size_t length = strlen( anAddon);
		wchar_t* wsAttrValue = MP_NEW_ARR( wchar_t, length + 1);

		va_mbstowcs( wsAttrValue, anAddon, length);
		/*#if _MSC_VER >= 1500//vs2008
		size_t convertedChars = 0;
		mbstowcs_s( &convertedChars, wsAttrValue, length, anAddon, length);
		#else
		mbstowcs(wsAttrValue, anAddon, length);
		#endif*/
		wsAttrValue[ length] = 0;

		aString += wsAttrValue;

		MP_DELETE_ARR( wsAttrValue);
	}

	inline void AddString( std::wstring& aString, const std::wstring& anIndent, const std::string& anAddon)
	{
		aString += anIndent;

		AddString( aString, anAddon.c_str());
	}

	inline void AddString( std::wstring& aString, const std::wstring& anIndent, const char* anAddon)
	{
		aString += anIndent;

		AddString( aString, anAddon);
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, const wchar_t* aValue)
	{
		aString += anIndent;
		aString += aKey;
		aString += L": ";
		aString += aValue;
		aString += L"\n";
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const std::string& aKey, const wchar_t* aValue)
	{
		AddString( aString, anIndent, aKey);
		aString += L": ";
		aString += aValue;
		aString += L"\n";
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, const std::wstring aValue)
	{
		AddPair( aString, anIndent, aKey, aValue.c_str());
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, const std::string aValue)
	{
		aString += anIndent;
		aString += aKey;
		aString += L": ";
		AddString( aString, aValue.c_str());
		aString += L"\n";
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, int aValue, int aRadix = 10)
	{
		wchar_t buffer[ 50];
		AddPair( aString, anIndent, aKey, va_itow( aValue, buffer, aRadix, va_arrsize_in_bytes(buffer)));
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, unsigned __int64 aValue, int aRadix = 10)
	{
		wchar_t buffer[ 50];
		AddPair( aString, anIndent, aKey, va_ui64tow( aValue, buffer, aRadix, va_arrsize_in_bytes(buffer)));
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, unsigned int aValue, int aRadix = 10)
	{
		wchar_t buffer[ 50];
		AddPair( aString, anIndent, aKey, va_ultow( aValue, buffer, aRadix, va_arrsize_in_bytes(buffer)));
	}

	inline void AddPair( std::wstring& aString, const std::wstring& anIndent, const wchar_t* aKey, float aValue)
	{
		wchar_t number[ 80];
#if _MSC_VER >= 1500 //vs2008
		swprintf( number, va_arrsize_in_bytes(number), L"%f", aValue);
#else
		swprintf( number, L"%f", aValue);
#endif
		AddPair( aString, anIndent, aKey, number);
	}

	// Бинарное состояние синхранизируемого объекта
	class syncObjectBinState
	{
	public:
		syncObjectBinState()
		{
			updatingType = BSSCC_CLEAR;
			version = 0;
			binStateSize = 0;
			binStateCapacity = 0;
			binState = NULL;
		}

		syncObjectBinState(const syncObjectBinState& aSrc)
		{
			updatingType = BSSCC_CLEAR;
			version = 0;
			binStateSize = 0;
			binStateCapacity = 0;
			binState = NULL;
			*this = aSrc;
		}

		virtual ~syncObjectBinState()
		{
			if (binState != NULL)
				MP_DELETE_ARR( binState);
		}

		void ApllyUpdating(BSSCCode aUpdatingType, unsigned int aVersion, const BYTE* aData, int aDataSize)
		{
			switch (aUpdatingType)
			{
			case BSSCC_REPLACE_ALL:
				{
					ReplaceAll(aVersion, aData, aDataSize);
					break;
				}
			case BSSCC_APPEND:
				{
					Append(aVersion, aData, aDataSize);
					break;
				}
			case BSSCC_CLEAR:
				{
					Clear(aVersion);
					break;
				}
			}
		}

		void ApllyUpdating(const syncObjectBinState& aUpdatingState)
		{
			BSSCCode newUpdatingType;
			unsigned int newVersion;
			BYTE* newBinState;
			int newBinStateSize;
			aUpdatingState.GetFullState(newUpdatingType, newVersion, newBinState, newBinStateSize);
			ApllyUpdating(newUpdatingType, newVersion, newBinState, newBinStateSize);
		}

		void GetFullState(BSSCCode& aUpdatingType, unsigned int& aVersion, BYTE*& aBinState, int& aBinStateSize) const
		{
			aUpdatingType = updatingType;
			aVersion = version;
			aBinState = binState;
			aBinStateSize = binStateSize;
		}

		unsigned int GetVersion() const
		{
			return version;
		}

		int GetSize() const
		{
			return binStateSize;
		}

		unsigned int GetChecksum()
		{
			unsigned int checkSum = 0;
			for (int i = 0; i < binStateSize; i++)
			{
				checkSum += binState[i];
			}
			return checkSum;
		}

		syncObjectBinState& operator=(const syncObjectBinState& aSrc)
		{
			if (this != &aSrc)
			{
				Clear(0);
				ApllyUpdating(aSrc);
			}
			return *this;
		}

	private:
		BSSCCode updatingType;
		unsigned int version;
		BYTE* binState;
		int binStateSize;
		int binStateCapacity;

		void Grow(int aNeedSize)
		{
			if (binStateCapacity < aNeedSize)
			{
				int newCapacity = aNeedSize + 3000;
				BYTE* pData = MP_NEW_ARR( BYTE, newCapacity);
				if (binState != NULL)
				{
					memcpy( pData, binState, binStateSize);
					MP_DELETE_ARR( binState);
				}
				binState = pData;
				binStateCapacity = newCapacity;
			}
		}

		void Clear(unsigned int aVersion)
		{
			updatingType = BSSCC_CLEAR;
			version = aVersion;
			binStateSize = 0;
		}

		void Append(unsigned int aVersion, const BYTE* aData, int aDataSize)
		{
			if (updatingType != BSSCC_REPLACE_ALL)
				updatingType = BSSCC_APPEND;
			version = aVersion;
			CopyBinState(aData, aDataSize);
		}

		void ReplaceAll(unsigned int aVersion, const BYTE* aData, int aDataSize)
		{
			updatingType = BSSCC_REPLACE_ALL;
			version = aVersion;
			binStateSize = 0;
			CopyBinState(aData, aDataSize);
		}

		void CopyBinState(const BYTE* aData, int aDataSize)
		{
			int newSize = binStateSize + aDataSize;
			Grow(newSize);
			memcpy(binState + binStateSize, aData, aDataSize);
			binStateSize = newSize;
		}
	};

	struct mlSysSynchProps
	{
	public:
		// побитовая маска, определяющая какие свойства заданы
		int miSet;
		//// флаги
		// 0-й флаг – если установлен, то это объект с назначаемым сервером собственником (SOA)
#define MLSYNCH_SERVER_OWNER_ASSIGNATION_MASK 1
		// 1-й флаг – если установлен, то свойства может менять только собственник, если он установлен (если нет, то никто не может менять)
#define MLSYNCH_OWNER_ONLY_SYNCHR_MASK 2
		// 2-й флаг - если установлен, то объект должен исчезнуть из мира вместе с исчезновением хозяина (например, аватар)
#define MLSYNCH_EXCLUSIVE_OWN_MASK 4
		int miFlags;
		// собственникMLSYNCH_OWNER_ONLY_SYNCHR_MASK 2
		MP_WSTRING msOwner;
		// идентификатор зоны
		int miZone; 
		// идентификатор реальности
		unsigned int muReality;
		// уровень паузы
		unsigned int muPauseLevel; 
		// всегда true. Если меняется с true на false, то надо удалить из этой реплики мира
		bool mbConnected;
		// идентификатор локации
		MP_WSTRING msLocation;
		// идентификатор л общения
		MP_WSTRING msCommunicationArea;

#define MLSYNCH_UDEF_X FLT_MIN
#define MLSYNCH_UDEF_Y FLT_MIN
#define MLSYNCH_UDEF_Z FLT_MIN

		// координаты x, y, z
		float mfCoordinateX, mfCoordinateY, mfCoordinateZ;
		// серверное время
		unsigned __int64 serverTime;
		// бинарное состояние
		syncObjectBinState mBinState;
		// время исчезновения (соответствует серверному времени)
		unsigned __int64 mlDeleteTime;

		bool operator==( const mlSysSynchProps& anOtherSysProps) const
		{
			if (this == &anOtherSysProps)
				return true;
			if (miSet != anOtherSysProps.miSet)
				return false;
			if ((miSet & MLSYNCH_CONNECTED_CHANGED_MASK) != 0 && mbConnected != anOtherSysProps.mbConnected)
				return false;
			if ((miSet & MLSYNCH_SERVERTIME_SET_MASK) != 0 && serverTime != anOtherSysProps.serverTime)
				return false;
			if ((miSet & MLSYNCH_FLAGS_SET_MASK) != 0 && miFlags != anOtherSysProps.miFlags)
				return false;
			if ((miSet & MLSYNCH_OWNER_CHANGED_MASK) != 0 && msOwner != anOtherSysProps.msOwner)
				return false;
			if ((miSet & MLSYNCH_ZONE_CHANGED_MASK) != 0 && miZone != anOtherSysProps.miZone)
				return false;
			if ((miSet & MLSYNCH_REALITY_SET_MASK) != 0 && muReality != anOtherSysProps.muReality)
				return false;
			if ((miSet & MLSYNCH_COORDINATE_XY_SET_MASK) != 0 && mfCoordinateX != anOtherSysProps.mfCoordinateX)
				return false;
			if ((miSet & MLSYNCH_COORDINATE_XY_SET_MASK) != 0 && mfCoordinateY != anOtherSysProps.mfCoordinateY)
				return false;
			if ((miSet & MLSYNCH_COORDINATE_SET_MASK) != 0 && mfCoordinateZ != anOtherSysProps.mfCoordinateZ)
				return false;
			if ((miSet & MLSYNCH_PAUSE_LEVEL_CHANGED_MASK) != 0 && muPauseLevel != anOtherSysProps.muPauseLevel)
				return false;
			if ((miSet & MLSYNCH_DELETETIME_CHANGED_MASK) != 0 && mlDeleteTime != anOtherSysProps.mlDeleteTime)
				return false;
			if ((miSet & MLSYNCH_LOCATION_CHANGED_MASK) != 0 && msLocation != anOtherSysProps.msLocation)
				return false;
			if ((miSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK) != 0 && msCommunicationArea != anOtherSysProps.msCommunicationArea)
				return false;

			return true;
		}

		bool operator!=( const mlSysSynchProps& anOtherSysProps) const
		{
			return !(*this == anOtherSysProps);
		}

		mlSysSynchProps():
			MP_WSTRING_INIT(msLocation),
			MP_WSTRING_INIT(msCommunicationArea),				
			MP_WSTRING_INIT(msOwner)
		{
			Reset();
		}

		mlSysSynchProps(const mlSysSynchProps& aSrc):
			MP_WSTRING_INIT(msLocation),
			MP_WSTRING_INIT(msCommunicationArea),
			MP_WSTRING_INIT(msOwner)
		{
			*this = aSrc;
		}

		~mlSysSynchProps(){
		}

		void Reset()
		{
			miSet = 0;
			miFlags = 0;
			msOwner = L"";
			miZone = -1;
			muReality = 0;
			muPauseLevel = 0;
			mbConnected = true;
			msLocation = L"";
			mfCoordinateX = MLSYNCH_UDEF_X;
			mfCoordinateY = MLSYNCH_UDEF_Y;
			mfCoordinateZ = MLSYNCH_UDEF_Z;
			serverTime = 0;
			mBinState.ApllyUpdating(BSSCC_CLEAR, 0, NULL, 0);
			mlDeleteTime = 0;
			msCommunicationArea = L"";
		}

		void SetFlags( int aFlags)
		{
			miFlags = aFlags;
			miSet |= MLSYNCH_FLAGS_SET_MASK;
		}

		void SetOwner( std::wstring anOwner)
		{
			msOwner = anOwner;
			miSet |= MLSYNCH_OWNER_CHANGED_MASK;
		}

		void SetZone( int aZone)
		{
			miZone = aZone;
			miSet |= MLSYNCH_ZONE_CHANGED_MASK;
		}

		void SetLocation( const std::wstring& aLocation)
		{
			// если локация определена на клиенте, то не надо ее брать с сервера
			/* BDima 01.07.2010. Не понятно. Как изменять локацию чужих аватаров?
			if(!(miSet & MLSYNCH_LOCATION_CHANGED_MASK)){
			msLocation = aLocation;
			miSet |= MLSYNCH_LOCATION_CHANGED_MASK;
			}
			*/
			msLocation = aLocation;
			miSet |= MLSYNCH_LOCATION_CHANGED_MASK;
		}

		void SetCommunicationArea( const std::wstring& aCommunicationArea)
		{			
			msCommunicationArea = aCommunicationArea;
			miSet |= MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK;
		}

		void SetReality( unsigned int aReality)
		{
			muReality = aReality;
			miSet |= MLSYNCH_REALITY_SET_MASK;
		}

		void SetConnected( unsigned char aConnected)
		{
			mbConnected = aConnected?true:false;
			miSet |= MLSYNCH_CONNECTED_CHANGED_MASK;
		}

		bool IsConnected() const
		{
			if ((miSet || MLSYNCH_CONNECTED_CHANGED_MASK) == 0)
				return true;
			if (mbConnected)
				return true;
			else
				return false;
		}

		void SetCoordinateZ( float aCoordinateZ)
		{
			mfCoordinateZ = aCoordinateZ;
			miSet |= MLSYNCH_COORDINATE_SET_MASK;
		}

		bool SetCoordinateXY( float aCoordinateX, float aCoordinateY)
		{
			bool bIsChangedXY = false;
			if (mfCoordinateX != aCoordinateX)
				bIsChangedXY = true;
			if (mfCoordinateY != aCoordinateY)
				bIsChangedXY = true;

			mfCoordinateX = aCoordinateX;
			mfCoordinateY = aCoordinateY;
			miSet |= MLSYNCH_COORDINATE_XY_SET_MASK;

			return bIsChangedXY;
		}


		void SetPauseLevel( unsigned int aPauseLevel)
		{
			muPauseLevel = aPauseLevel;
			miSet |= MLSYNCH_PAUSE_LEVEL_CHANGED_MASK;
		}

		void SetServerTime( unsigned __int64 aServerTime)
		{
			serverTime = aServerTime;
			miSet |= MLSYNCH_SERVERTIME_SET_MASK;
		}

		void SetBinState( const syncObjectBinState& aBinState)
		{
			mBinState.ApllyUpdating(aBinState);
			miSet |= MLSYNCH_BIN_STATE_CHANGED_MASK;
		}

		void SetBinState( BSSCCode aUpdatingType, unsigned int aVersion, const BYTE* aData, int aDataSize)
		{
			mBinState.ApllyUpdating(aUpdatingType, aVersion, aData, aDataSize);
			miSet |= MLSYNCH_BIN_STATE_CHANGED_MASK;
		}

		void SetDeleteTime( unsigned __int64 aDeleteTime)
		{
			mlDeleteTime = aDeleteTime;
			miSet |= MLSYNCH_DELETETIME_CHANGED_MASK;
		}

		mlSysSynchProps& operator=(const mlSysSynchProps& aSrc){
			if (this == &aSrc)
				return *this;
			miSet = aSrc.miSet;
			miFlags = aSrc.miFlags;
			msOwner = aSrc.msOwner;
			miZone = aSrc.miZone;
			muReality = aSrc.muReality;
			mbConnected = aSrc.mbConnected;
			msLocation = aSrc.msLocation;
			mfCoordinateX = aSrc.mfCoordinateX;
			mfCoordinateY = aSrc.mfCoordinateY;
			mfCoordinateZ = aSrc.mfCoordinateZ;
			serverTime = aSrc.serverTime;
			muPauseLevel = aSrc.muPauseLevel;
			mBinState = aSrc.mBinState;
			mlDeleteTime = aSrc.mlDeleteTime;
			msCommunicationArea = aSrc.msCommunicationArea;

			return *this;
		}

		bool isEqual(const wchar_t* apch1, const wchar_t* apch2){
			for(;;){
				if(*apch1!=*apch2) return false;
				if(*apch1==L'\0') return true;
				apch1++, apch2++;
			}
		}

		int update( const mlSysSynchProps& aSrc, int anAllowedSet)
		{
			int iSet = aSrc.miSet & anAllowedSet;

			if (iSet & MLSYNCH_FLAGS_SET_MASK)
				SetFlags( aSrc.miFlags);

			if (iSet & MLSYNCH_OWNER_CHANGED_MASK)
			{
				if (!isEqual(msOwner.c_str(), aSrc.msOwner.c_str()))
					SetOwner( aSrc.msOwner);
				else
					iSet &= ~MLSYNCH_OWNER_CHANGED_MASK;
			}

			if (iSet & MLSYNCH_ZONE_CHANGED_MASK)
				SetZone( aSrc.miZone);

			if (iSet & MLSYNCH_LOCATION_CHANGED_MASK)
				SetLocation( aSrc.msLocation);

			if (iSet & MLSYNCH_REALITY_SET_MASK)
				SetReality( aSrc.muReality);

			if (iSet & MLSYNCH_CONNECTED_CHANGED_MASK)
				SetConnected( aSrc.mbConnected);

			if (iSet & MLSYNCH_COORDINATE_SET_MASK)
				SetCoordinateZ( aSrc.mfCoordinateZ);

			if (iSet & MLSYNCH_COORDINATE_XY_SET_MASK)
				SetCoordinateXY( aSrc.mfCoordinateX, aSrc.mfCoordinateY);

			if (iSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
				SetBinState( aSrc.mBinState);

			if (iSet & MLSYNCH_SERVERTIME_SET_MASK)
				SetServerTime( aSrc.serverTime);

			if (iSet & MLSYNCH_PAUSE_LEVEL_CHANGED_MASK)
				SetPauseLevel( aSrc.muPauseLevel);

			if (iSet & MLSYNCH_DELETETIME_CHANGED_MASK)
				SetDeleteTime( aSrc.mlDeleteTime);

			if (iSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
				SetCommunicationArea( aSrc.msCommunicationArea);

			return iSet;
		}

		unsigned char GetSysPropsCount() const
		{
			unsigned char ucSysPropCnt = 0;
			if(miSet){
				if(miSet & MLSYNCH_FLAGS_SET_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_OWNER_CHANGED_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_ZONE_CHANGED_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_REALITY_SET_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_COORDINATE_SET_MASK)
					ucSysPropCnt++;
				if (miSet & MLSYNCH_COORDINATE_XY_SET_MASK)
					ucSysPropCnt+=2;
				if(miSet& MLSYNCH_SERVERTIME_SET_MASK)
					ucSysPropCnt++;
				if (miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_LOCATION_CHANGED_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_DELETETIME_CHANGED_MASK)
					ucSysPropCnt++;
				if(miSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
					ucSysPropCnt++;
			}
			return ucSysPropCnt;
		}

		void ToString( std::wstring& aString, const wchar_t* anIndent) const
		{
			std::wstring indent;
			if (anIndent != NULL)
				indent = anIndent;

			aString += L"{\n";
			indent += L"  ";

			if(miSet & MLSYNCH_FLAGS_SET_MASK)
				AddPair( aString, indent, L"flags", miFlags);
			if(miSet & MLSYNCH_OWNER_CHANGED_MASK)
				AddPair( aString, indent, L"owner", msOwner);
			if(miSet & MLSYNCH_ZONE_CHANGED_MASK)
				AddPair( aString, indent, L"zone", miZone);
			if(miSet & MLSYNCH_REALITY_SET_MASK)
				AddPair( aString, indent, L"reality", muReality);
			if(miSet & MLSYNCH_COORDINATE_SET_MASK)
				AddPair( aString, indent, L"z", mfCoordinateZ);
			if (miSet & MLSYNCH_COORDINATE_XY_SET_MASK)
			{
				AddPair( aString, indent, L"x", mfCoordinateX);
				AddPair( aString, indent, L"y", mfCoordinateY);
			}
			if(miSet& MLSYNCH_SERVERTIME_SET_MASK)
				AddPair( aString, indent, L"serverTime", serverTime);
			if (miSet & MLSYNCH_BIN_STATE_CHANGED_MASK)
				AddPair( aString, indent, L"binStateSize", mBinState.GetSize());
			if(miSet & MLSYNCH_LOCATION_CHANGED_MASK)
				AddPair( aString, indent, L"location", msLocation);
			if(miSet & MLSYNCH_DELETETIME_CHANGED_MASK)
				AddPair( aString, indent, L"deleteTime", mlDeleteTime);
			if(miSet & MLSYNCH_COMMUNICATION_AREA_CHANGED_MASK)
				AddPair( aString, indent, L"communicationArea", msCommunicationArea);

			indent.resize( indent.size() - 2);
			AddString( aString, indent, L"}");
		}
	};


	// 1й - изменился msOwner (попытка установить себя собственником или освободить объект)
#define SYSP_OWNER_CHANGED_MASK (1 << 1)
	// 5й - изменилось __connected
#define SYSP_CONNECTED_CHANGED_MASK (1 << 5)
	// 7й - изменилось состояние RMML-подобъекта
#define SYSP_SUB_OBJ_CHANGED_MASK (1 << 7)
	// 8й - изменилось серверное время
#define SYSP_SERVERTIME_SET_MASK (1 << 8)


	enum syncObjValueType
	{
		SYVT_UNDEFINED = 0, // неопределенное значение (void)
		SYVT_NULL,     // пустое значение (null)
		SYVT_INT,      // 31битное целое число (int)
		SYVT_DOUBLE,   // вещественное число (double)
		SYVT_STRING,   // строка в UNICODE (std::wstring)
		SYVT_OBJECT,   // подобъект типа syncObjectState
		SYVT_OBJLINK,  // подобъект типа Link (std::string)
		SYVT_BIN,      // бинарные данные 
	};

	struct syncObjectState;

	// синхронизируемое свойство 
	struct syncObjectProp
	{
		// строковый идентификатор свойства в dot-нотации
		MP_STRING name;

		// Значение свойства
		CByteVector binValue;

		syncObjectProp():
			MP_VECTOR_INIT(binValue),
			MP_STRING_INIT(name)
		{
		}
		syncObjectProp(const std::string& name_):
			MP_VECTOR_INIT(binValue),
			MP_STRING_INIT(name)
		{
			this->name = name_;
		}

		bool operator==( const syncObjectProp& aProperty) const
		{
			if (this == &aProperty)
				return true;
			if (name != aProperty.name)
				return false;
			if (binValue != aProperty.binValue)
				return false;
			return true;
		}

		bool operator!=( const syncObjectProp& aProperty) const
		{
			return !(*this == aProperty);
		}

		void ToString( std::wstring& aString, const wchar_t* anIndent) const
		{
			std::wstring indent;
			if (anIndent != NULL)
				indent = anIndent;

			AddPair( aString, indent, name, L"{...}");
		}
	};

	// Вектор синхронизируемых свойств
	typedef MP_VECTOR<syncObjectProp> syncObjectPropVector;

	// Вектор состояний подобъектов
	typedef MP_VECTOR<syncObjectState> syncObjectStateVector;

	// Результат установки синхронизируемых свойств в значения, полученные с сервера
	enum ESynchStateError
	{
		SSE_NO_ERRORS,
		SSE_WRONG_STATE,
		SSE_OBJECT_NOT_FOUND,
		SSE_NOT_SYNCRONIZED,
		SSE_DELETE_WRONG_REALITY_AVATAR,
		SSE_DELETE_DISCONECTED,
		SSE_DELETE_CHANGED_REALITY_AVATAR,
		SSE_DELETE_NOT_FOUND,
		SSE_REJECTED,
		SSE_HASHERROR,
		SSE_OBSOLETE	// опоздавший пакет. Например повторно приходит полное состояние, которое у меня уже есть
	};

	// структура, содержащая данные о значениях свойств объектов, подлежащих синхронизации
	struct syncObjectState
	{
		enum StateType
		{
			ST_FULL = 0,		// 0 - полное состояние объекта (creation_info должно быть не пустым). Данное состояние применяется в соответствиями с правилами clientVersion, servertVersion
			ST_UPDATE,			// 1 - пакет обновления
			ST_NOTFOUND,		// 2 - объект на сервере не найден
			ST_CONFIRMATION,	// 3 - ответ на обновление, посланное с клиента на сервер
			ST_FORCE			// 4 - полное состояние объекта, присланное после откидывания состояния клиента
		};

		StateType type;	

		// версия объекта (приходит с данными с сервера синхронизации)
#define MLSYNCH_UDEF_VER 0xFFFF
		// серверная версия состояния объекта 
		unsigned short mwServerVersion;
		// клиентскаая версия состояния объекта 
		unsigned short mwClientVersion;

		MP_STRING string_id;	// строковый идентификатор (под)объекта в dot-нотации

		unsigned int uiID;
		unsigned int uiBornRealityID;
		unsigned int uiRealityID;

		MP_WSTRING creation_info;// информация, необходимая для создания объекта в JSON-нотации

		// системные свойства
		mlSysSynchProps sys_props;

		// синхронизируемые свойства
		syncObjectPropVector props;

		// подобъекты
		syncObjectStateVector sub_objects;

		unsigned int uiHashObj;

		//-
		syncObjectState()
#ifdef MEMORY_PROFILING
			: MP_VECTOR_INIT(props),
			MP_VECTOR_INIT(sub_objects),
			MP_WSTRING_INIT(creation_info),
			MP_STRING_INIT(string_id)
#endif
		{
			uiID = OBJ_ID_UNDEF;
			type = ST_UPDATE;
		}

		syncObjectState(const syncObjectState& other):
			MP_VECTOR_INIT(props),
			MP_VECTOR_INIT(sub_objects),
			MP_WSTRING_INIT(creation_info),
			MP_STRING_INIT(string_id)
		{
			type = other.type;	
			mwServerVersion = other.mwServerVersion;
			mwClientVersion = other.mwClientVersion;
			string_id = other.string_id;
			uiID = other.uiID;
			uiBornRealityID = other.uiBornRealityID;
			uiRealityID = other.uiRealityID;
			creation_info = other.creation_info;
			sys_props = other.sys_props;
			props = other.props;
			sub_objects = other.sub_objects;
			uiHashObj = other.uiHashObj;
		}

		void SetFull(){ type = ST_FULL; }

		// является ли состояние полным?
		bool IsFullState() const
		{
			if (type == ST_FULL || type == ST_FORCE)
				return true;
			else
				return false;
		}

		// является ли состояние отброшенным
		bool IsForceState() const
		{
			if (type == ST_FORCE)
				return true;
			else
				return false;
		}

		void SetNotFound(){ type = ST_NOTFOUND; }

		// является ли это сообщением сервера о том, 
		// что никакой инфы об объекте на сервере нет?
		bool IsNotFound() const { return (type == ST_NOTFOUND ? true : false); }

		bool operator==( const syncObjectState& aState) const
		{
			if (this == &aState) // Сравниваем два указателя - вдруг это один и тот же объект
				return true;

			if (mwServerVersion != aState.mwServerVersion)
				return false;
			if (mwClientVersion != aState.mwClientVersion)
				return false;
			if (type != aState.type)
				return false;
			if (string_id != aState.string_id)
				return false;
			if (uiID != aState.uiID)
				return false;
			if (uiBornRealityID != aState.uiBornRealityID)
				return false;
			if (uiRealityID != aState.uiRealityID)
				return false;
			if( uiHashObj != aState.uiHashObj)
				return false;
			if (sys_props != aState.sys_props)
				return false;
			if (props != aState.props)
				return false;
			if (sub_objects != aState.sub_objects)
				return false;
			return true;
		}

		bool operator!=( const syncObjectState& aState) const
		{
			return !(*this == aState);
		}

		void ToString( std::wstring& aString, const wchar_t* anIndent) const
		{
			std::wstring indent;
			if (anIndent != NULL)
				indent = anIndent;

			aString += L"{\n";
			indent += L"  ";

			if (uiID != OBJ_ID_UNDEF)
			{
				wchar_t tmpString[ 100];
#if _MSC_VER >= 1500 //vs2008
				swprintf( tmpString, va_arrsize_in_bytes(tmpString), L"objectID: %u, bornRealityID: %u, realityID: %u\n",
					uiID, uiBornRealityID, uiRealityID);
#else
				swprintf( tmpString, L"objectID: %u, bornRealityID: %u, realityID: %u\n",
					uiID, uiBornRealityID, uiRealityID);
#endif
				AddString( aString, indent, tmpString);
			}
			else
				AddPair( aString, indent, L"name", string_id);

			wchar_t tmpString1[ 20];
#if _MSC_VER >= 1500 //vs2008
			swprintf( tmpString1, va_arrsize_in_bytes(tmpString1), L"hashObj: %u\n", uiHashObj);
#else
			swprintf( tmpString1, L"hashObj: %u\n", uiHashObj);
#endif	
			AddString( aString, indent, tmpString1);

			if (sys_props.GetSysPropsCount() > 0)
			{
				AddString( aString, indent, L"sysProperties: ");
				sys_props.ToString( aString, indent.c_str());
				AddString( aString, indent, L"\n");
			}

			if (props.size() > 0)
			{
				AddString( aString, indent, L"sysProperties: {\n");
				indent += L"  ";
				syncObjectPropVector::const_iterator it = props.begin();
				for (;  it != props.end();  it++)
					it->ToString( aString, indent.c_str());
				indent.resize( indent.size() - 2);
				AddString( aString, indent, L"}\n");
			}

			if (sub_objects.size() > 0)
			{
				AddString( aString, indent, L"subobjects: {\n");
				indent += L"  ";
				syncObjectStateVector::const_iterator it = sub_objects.begin();
				for (;  it != sub_objects.end();  it++)
					it->ToString( aString, indent.c_str());
				indent.resize( indent.size() - 2);
				AddString( aString, indent, L"}\n");
			}

			indent.resize( indent.size() - 2);
			AddString( aString, indent, L"}");
		}
	};


}