
#pragma once

#define SDPUT_IMPL(T) \
	mlSynchData& operator<<(T data){ \
		if(miRealSize < (miSize+(int)sizeof(T))){ \
			grow(miSize+sizeof(T)); \
		} \
		*((T*)(mpData+miSize))=data; \
		miSize+=sizeof(T); \
		return *this; \
	}

#define SDGET_IMPL(T) \
	mlSynchData& operator>>(T &data){ \
		if((miGetIdx+(int)sizeof(T))>miRealSize) return *this; \
		data=*((T*)(mpData+miGetIdx)); \
		miGetIdx+=(int)sizeof(T); \
		return *this; \
	}

#ifdef JSVAL_OBJECT
struct Id_Jso{
	int id;
	JSObject* jso;
	Id_Jso(int aid, JSObject* ajso){ id=aid, jso=ajso; }
};
typedef std::vector<Id_Jso*> Id2Jso;
typedef std::vector<Id_Jso*>::iterator Id2JsoIter;
struct JsoRef{
	JSObject* jso;
	std::wstring prop;
	int id;
};
typedef std::list<JsoRef*> JsoRefs;
typedef std::list<JsoRef*>::iterator JsoRefsIter;
#endif

#ifndef RMML_DECL 
#define RMML_DECL 
#endif

class RMML_DECL mlSynchData
{
	unsigned char* mpData;
	bool mbAssigned;
	int miGetIdx;
	int miSize;
	int miRealSize;
	int miSetIdx;
	char* mpStr;
	unsigned short muStrAllocSize;
	wchar_t* mpWStr;
	unsigned short muWStrAllocSize;
	void grow(int size);
	void allocstr(int size);
	void allocwstr(int size);
	bool mbJS;
	#ifdef JSVAL_OBJECT
	JSContext* mcx;
//	bool mbSavingRefs;
//	#pragma warning( disable : 4251 )
//	std::vector<JSObject*> mvSavedObject;
	Id2Jso* pId2Jso;
	JsoRefs* pJsoRefs;
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
		#ifdef JSVAL_OBJECT
		if( mbJS)
		{
			mcx=NULL;
		//	mbSavingRefs=false;
			pId2Jso=NULL;
			pJsoRefs=NULL;
		}
		#endif
	}

	mlSynchData::mlSynchData(void* pvoid, int size){
		mpData=(unsigned char*)pvoid;
		mbAssigned=true;
		miGetIdx=0;
		miSize=0;
		miRealSize=size;
		mpStr=NULL;
		mpWStr=NULL;
	}

	mlSynchData::~mlSynchData();
	
	int size(){ if(mbAssigned) return miRealSize; return miSize; }
	void* data(){ return mpData; }
//	void assign(void* pvoid, int size){  }
	void reset(){ if(!mbAssigned) miSize=0; miGetIdx=0; miSetIdx=-1; }
	int getPos(){ return miSize; }
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
	// 
	SDPUT_IMPL(bool)
	SDPUT_IMPL(int)
	SDPUT_IMPL(unsigned int)
	SDPUT_IMPL(char)
	SDPUT_IMPL(unsigned char)
	SDPUT_IMPL(short)
	SDPUT_IMPL(unsigned short)
	SDPUT_IMPL(double)
	SDPUT_IMPL(float)
	mlSynchData& operator<<(const char* str){
		unsigned short uLen=(unsigned short)strlen(str);
		(*this)<<uLen;
		put((void*)str, (int)uLen*sizeof(char));
		return *this;
	}
	mlSynchData& operator<<(char* str){
		unsigned short uLen=(unsigned short)strlen(str);
		(*this)<<uLen;
		put((void*)str, (int)uLen*sizeof(char));
		return *this;
	}
	mlSynchData& operator<<(const wchar_t* wstr){
		unsigned short uLen=(unsigned short)wcslen(wstr);
		(*this)<<uLen;
		put((void*)wstr, (int)uLen*sizeof(wchar_t));
		return *this;
	}
	mlSynchData& operator<<(wchar_t* wstr){
		unsigned short uLen=(unsigned short)wcslen(wstr);
		(*this)<<uLen;
		put((void*)wstr, (int)uLen*sizeof(wchar_t));
		return *this;
	}
	#ifdef JSVAL_OBJECT
	bool IsNotRef(JSObject* jso);
	mlSynchData& operator<<(JSContext* cx);
	mlSynchData& operator<<(JSObject* jso);
	mlSynchData& operator<<(jsval jsv);
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
	SDGET_IMPL(double)
	SDGET_IMPL(float)
	mlSynchData& operator>>(char* &str){
		unsigned short uLen;
		(*this)>>uLen;
		if((miGetIdx+uLen)>miRealSize) return *this;
		allocstr(uLen);
		memcpy(mpStr,mpData+miGetIdx,uLen);
		mpStr[uLen]='\0';
		miGetIdx+=uLen;
		str=mpStr;
		return *this;
	}
	mlSynchData& operator>>(wchar_t* &wstr){
		unsigned short uLen;
		(*this)>>uLen;
		if((miGetIdx+uLen)>miRealSize) return *this; 
		allocwstr(uLen);
		memcpy(mpWStr,mpData+miGetIdx,uLen*sizeof(wchar_t));
		mpWStr[uLen]=L'\0';
		miGetIdx+=uLen*sizeof(wchar_t);
		wstr=mpWStr;
		return *this;
	}
	#ifdef JSVAL_OBJECT
	mlSynchData& operator>>(JSContext* &cx);
	JSObject* getJsoById(int aiID);
	void addRef(JSObject* ajso,const wchar_t* apwcPropName,int aiID);
	void setRefs(int aiID, JSObject* jso);
	mlSynchData& operator>>(JSObject* &jso);
	#endif
	void get(void* pvoid, int size){
		if((miGetIdx+size)>miRealSize) return; 
		unsigned char* psrc=mpData+miGetIdx;
		switch(size){
		case 1:
			*((char*)pvoid)=*((char*)psrc);
			return;
		case 2:
			*((short*)pvoid)=*((short*)psrc);
			return;
		case 3:
			*((char*)pvoid)=*((char*)psrc);
			*((short*)(((char*)pvoid)+1))=*((short*)(((char*)psrc)+1));
			return;
		case 4:
			*((int*)pvoid)=*((int*)psrc);
			return;
		default:
			memcpy(pvoid,psrc,size);
		}
		miGetIdx+=size;
		return;
	}
//	int getf(const char* format, ...);
};

