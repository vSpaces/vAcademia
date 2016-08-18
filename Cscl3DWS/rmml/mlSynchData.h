#pragma once

#define SDPUT_IMPL(T) \
	mlSynchData& operator<<(T data){ \
		if(miSetIdx >= 0){ \
			*((T*)(mpData+miSetIdx))=data; \
		}else{ \
			if(miRealSize < (miSize+(int)sizeof(T))){ \
				grow(miSize+sizeof(T)); \
			} \
			*((T*)(mpData+miSize))=data; \
			miSize+=sizeof(T); \
		} \
		return *this; \
	}

#define SDGET_IMPL(T) \
	mlSynchData& operator>>(T &data){ \
		if((miGetIdx+(int)sizeof(T))>miRealSize) return *this; \
		data=*((T*)(mpData+miGetIdx)); \
		miGetIdx+=(int)sizeof(T); \
		return *this; \
	}

#ifdef JSVAL_VOID
struct Id_Jso{
	int id;
	JSObject* jso;
	Id_Jso(int aid, JSObject* ajso){ id=aid, jso=ajso; }
};
typedef MP_VECTOR<Id_Jso*> Id2Jso;
typedef std::vector<Id_Jso*>::iterator Id2JsoIter;
struct JsoRef{
	JSObject* jso;
	MP_WSTRING prop;
	int id;

	JsoRef():
		MP_WSTRING_INIT(prop)
	{
		jso = NULL;
		id = 0;
	}

	JsoRef(const JsoRef& other):
		MP_WSTRING_INIT(prop)
	{
		jso = other.jso;
		id = other.id;
		prop = other.prop;
	}
};
typedef MP_LIST<JsoRef*> JsoRefs;
typedef std::list<JsoRef*>::iterator JsoRefsIter;
#endif

#ifdef SYNCHDATA_INCLUDE
#define SYNCHDATA_DECL
#else
#define SYNCHDATA_DECL RMML_DECL
#endif

class SYNCHDATA_DECL mlSynchData
{
	friend class mlSceneManager;

public:
	enum JSOutMode{
		JSOM_STATE,
		JSOM_TREE,
		JSOM_STATE2,
		JSOM_SYNCHM3,
	};
private:
	unsigned char* mpData;	// буфер 
	bool mbAssigned;	// память под буфер не выделялась самим mlSynchData
	int miGetIdx;	// позиция, с которой считывать следующие данные
	int miSize;		// объем данных, помещенных в буфер (в случае приаттаченного буфера == 0)
	int miRealSize;	// объем выделенной памяти
	int miSetIdx;	// используется для того, чтобы можно было помещать значения не в конец буфера как обычно, 
					// а установить позицию (>=0), в которую помещать следующее значение
	char* mpStr;	// указатель на последнюю считанную ascii-строку 
					// (перестает быть валидным при считывании следующей строки)
	unsigned long muStrAllocSize;	// размер выделенного под ascii-строку буфера 
									// (чтобы не выделять память при считывании следующей строки если буфер уже достаточной длины)
	wchar_t* mpWStr;// указатель на последнюю считанную unicode-строку 
					// (перестает быть валидным при считывании следующей строки)
	unsigned long muWStrAllocSize;	// размер выделенного под unicode-строку буфера 
									// (чтобы не выделять память при считывании следующей строки если буфер уже достаточной длины)
	void grow(unsigned int size);		// выделить память под буфер для записи данных (size - требуемый общий объем)
	void allocstr(unsigned int size);	// выделить память для буфера под ascii-строку 
	void allocwstr(unsigned int size);	// выделить память для буфера под unicode-строку 
	bool mbJS;
	#ifdef JSVAL_VOID
	JSContext* mcx;
//	bool mbSavingRefs;
	#pragma warning( disable : 4251 )
	Id2Jso* pId2Jso;
	JsoRefs* pJsoRefs;
	JSOutMode meJSOutMode;
int miDepth;
	#endif
public:

	mlSynchData::mlSynchData(bool abJS=false){
		mpData=NULL;
		mbAssigned=false;
		miGetIdx=0;
		miSize=0;
		miRealSize=0;
		miSetIdx=-1;
		mpStr=NULL;
		mpWStr=NULL;
		mbJS=abJS;
		#ifdef JSVAL_VOID
		if( mbJS)
		{
			mcx=NULL;
		//	mbSavingRefs=false;
			pId2Jso=NULL;
			pJsoRefs=NULL;
		}
		meJSOutMode=JSOM_STATE;
miDepth = 0;
		pId2Jso = NULL;
		pJsoRefs = NULL;
		#endif
	}

	mlSynchData::mlSynchData(void* pvoid, int size){
		mpData=(unsigned char*)pvoid;
		mbAssigned=true;
		miGetIdx=0;
		miSetIdx = -1;
		miSize=0;
		miRealSize=size;
		mpStr=NULL;
		mpWStr=NULL;
		#ifdef JSVAL_VOID
		pId2Jso = NULL;
		pJsoRefs = NULL;
		#endif
	}

	mlSynchData::~mlSynchData();
	
	int size()const{ if(mbAssigned) return miRealSize; return miSize; }
	void* data()const{ return mpData; }
//	void assign(void* pvoid, int size){  }
	void reset(){ if(!mbAssigned) miSize=0; miGetIdx=0; miSetIdx=-1; }
	int getPos()const{ return (miSize == 0 && miGetIdx > 0)?miGetIdx:miSize; }
	bool inTheEnd(){ 
		// если записывали в буфер, а теперь считываем, то (getPos() >= size()) работает неверно, поэтому...
		if(!mbAssigned && miSize > 0)
			return (miGetIdx >= miSize);
		return (getPos() >= size()); 
	}
	bool MayBeRead( int aiSize) {
		if(!mbAssigned && miSize > 0)
			return (miGetIdx + aiSize <= miSize);
		return (getPos() + aiSize <= size()); 
	}
	void setPos(int aiPos=-1){
		miGetIdx=miSetIdx=aiPos; 
		if(aiPos==getPos())	miSetIdx=-1;
		if(miGetIdx < 0) miGetIdx=0;
	}
	void* getDataInPos(int aiPos=-1){
		if(aiPos < 0) return mpData+miGetIdx;
		return mpData+aiPos;
	}
	void skip(int aiBytes){
		if((miGetIdx+aiBytes)>miRealSize) return; 
		miGetIdx+=aiBytes;
	}
	// урезает размер данных до указанного
	void truncate(int aiSize){
		if(aiSize > 0 && miSize > aiSize)
			miSize = aiSize;
	}
	void clear();
	unsigned int calculateCheckSum();
	static unsigned int calculateCheckSum( unsigned char* aData, unsigned int aDataSize);
	const mlSynchData& operator=(const mlSynchData& oSrcData){
		put(oSrcData.data(), oSrcData.size());
		return *this;
	}
	// 
	SDPUT_IMPL(bool)
	SDPUT_IMPL(int)
	SDPUT_IMPL(unsigned int)
	SDPUT_IMPL(char)
	SDPUT_IMPL(unsigned char)
	SDPUT_IMPL(short)
	SDPUT_IMPL(unsigned short)
	SDPUT_IMPL(unsigned long)
	SDPUT_IMPL(__int64)
	SDPUT_IMPL(unsigned __int64)
	SDPUT_IMPL(double)
	SDPUT_IMPL(float)
	#define SAVE_STR_LEN \
		if(lLen > 32000){ \
			unsigned short uLen = 0xFFFF; \
			(*this) << uLen; \
			(*this) << lLen; \
		}else{ \
			unsigned short uLen = (unsigned short)lLen; \
			(*this) << uLen; \
		}

	mlSynchData& operator<<(const char* str){
		unsigned long lLen = strlen(str);
		SAVE_STR_LEN
		put((void*)str, (int)lLen*sizeof(char));
		return *this;
	}
	mlSynchData& operator<<(char* str){
		unsigned long lLen = strlen(str);
		SAVE_STR_LEN
		put((void*)str, (int)lLen*sizeof(char));
		return *this;
	}
	mlSynchData& operator<<(const wchar_t* wstr){
		unsigned long lLen = wcslen(wstr);
		SAVE_STR_LEN
		put((void*)wstr, (int)lLen*sizeof(wchar_t));
		return *this;
	}
	mlSynchData& operator<<(wchar_t* wstr){
		unsigned long lLen = wcslen(wstr);
		SAVE_STR_LEN
		put((void*)wstr, (int)lLen*sizeof(wchar_t));
		return *this;
	}

#ifdef RMML_BUILD
#if _MSC_VER >= 1500 //vs2008

	mlSynchData& operator<<(const jschar* wstr){
		unsigned long lLen = wcslen((const wchar_t*)wstr);
		SAVE_STR_LEN
		put((void*)wstr, (int)lLen*sizeof(wchar_t));
		return *this;
	}
	mlSynchData& operator<<(jschar* wstr){
		unsigned long lLen = wcslen((wchar_t*)wstr);
		SAVE_STR_LEN
			put((void*)wstr, (int)lLen*sizeof(wchar_t));
		return *this;
	}

#endif
#endif

	#ifdef JSVAL_VOID
	bool IsNotRef(JSObject* jso);
	mlSynchData& operator<<(JSContext* cx);
	mlSynchData& operator<<(JSObject* jso);
	mlSynchData& operator<<(long jsv);	// jsval
	void setJSOutMode(JSOutMode aeMode){ meJSOutMode=aeMode; }
	void setJSContext(JSContext* cx){ mcx = cx; }
	#endif
	void put(void* pvoid, int size){
		if(miRealSize < (miSize+size)){
			grow(miSize+size);
		}
		unsigned char* pdest=mpData+miSize;
		if(miSetIdx >= 0){
			pdest=mpData+miSetIdx;
		}else
			miSize+=size;
		switch(size){
		case 1:
			*((char*)pdest)=*((char*)pvoid);
			return;
		case 2:
			*((short*)pdest)=*((short*)pvoid);
			return;
		case 3:
			*((char*)pdest)=*((char*)pvoid);
			*((short*)(((char*)pdest)+1))=*((short*)(((char*)pvoid)+1));
			return;
		case 4:
			*((int*)pdest)=*((int*)pvoid);
			return;
		default:
			memcpy(pdest,pvoid,size);
		}
	}
//	void putf(const char* format, ...);
	// 
	SDGET_IMPL(bool)
	SDGET_IMPL(int)
	SDGET_IMPL(unsigned int)
	SDGET_IMPL(char)
	SDGET_IMPL(unsigned char)
	SDGET_IMPL(short)
	SDGET_IMPL(unsigned short)
	SDGET_IMPL(unsigned long)
	SDGET_IMPL(__int64)
	SDGET_IMPL(unsigned __int64)
	SDGET_IMPL(double)
	SDGET_IMPL(float)

	#define READ_STR_LEN \
		unsigned short uLen; \
		if (!MayBeRead(sizeof(uLen))) \
		{ \
			return false;	 \
		}	 \
		(*this) >> uLen; \
		unsigned long lLen = uLen; \
		if(uLen == 0xFFFF){ \
			(*this) >> lLen; \
		} \
		if ((miGetIdx + (int)lLen) > miRealSize) \
		{ \
			return false; \
		}
		
	bool readSafely(char* &str)
	{
		READ_STR_LEN;
		allocstr(lLen);
		if (lLen > 0)
			memcpy(mpStr, mpData + miGetIdx, lLen);
		mpStr[lLen] = '\0';
		miGetIdx += lLen;
		str = mpStr;
		return true;
	}

	mlSynchData& operator>>(char* &str)
	{
		readSafely(str);	
		return *this;
	}

	bool readSafely(wchar_t* &wstr)
	{
		READ_STR_LEN;
		allocwstr(lLen);
		if (lLen > 0)
			memcpy(mpWStr, mpData + miGetIdx, lLen * sizeof(wchar_t));
		mpWStr[lLen] = L'\0';
		miGetIdx += lLen*sizeof(wchar_t);
		wstr = mpWStr;
		return true;
	}

	mlSynchData& operator>>(wchar_t* &wstr)
	{
		readSafely(wstr);
		return *this;
	}
	#ifdef JSVAL_VOID
	mlSynchData& operator>>(JSContext* &cx);
	JSObject* getJsoById(int aiID);
	void addRef(JSObject* ajso,const wchar_t* apwcPropName,int aiID);
	void setRefs(int aiID, JSObject* jso);
	mlSynchData& operator>>(JSObject* &jso);
	mlSynchData& operator>>(long &jsv); // jsval
	bool readSafely(jsval &jsv);
	bool readSafely(JSObject* &jso);
	#endif
	void get(void* pvoid, int size){
		if((miGetIdx+size)>miRealSize) return; 
		unsigned char* psrc=mpData+miGetIdx;
		switch(size){
		case 1:
			*((char*)pvoid)=*((char*)psrc);
			break;
		case 2:
			*((short*)pvoid)=*((short*)psrc);
			break;
		case 3:
			*((char*)pvoid)=*((char*)psrc);
			*((short*)(((char*)pvoid)+1))=*((short*)(((char*)psrc)+1));
			break;
		case 4:
			*((int*)pvoid)=*((int*)psrc);
			break;
		default:
			memcpy(pvoid,psrc,size);
		}
		miGetIdx+=size;
		return;
	}
//	int getf(const char* format, ...);
};
