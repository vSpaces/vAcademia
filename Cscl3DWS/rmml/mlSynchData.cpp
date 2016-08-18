
//#ifdef RMML_BUILD
#include "mlRMML.h"
#include "jsapi.h"
//#endif
#include "..\oms\oms_context.h"
#include "rmml.h"

#pragma warning( disable : 4800 )

namespace rmml{
/*
class mlSynchData
{
	unsigned char* mpData;
	int miSize;
	int miRealSize;
	char* mpStr;
	wchar_t* mpWStr;
public:
	mlSynchData();
	~mlSynchData();
	int size();
	void* data();
	// 
	mlSynchData& operator<<(int integer);
//	mlSynchData& operator<<(char character);
//	mlSynchData& operator<<(unsigned char uchar);
	mlSynchData& operator<<(unsigned short ushort);
	mlSynchData& operator<<(const char* str);
	mlSynchData& operator<<(const wchar_t* wstr);
	void put(void* pvoid, int size);
//	void putf(const char* format, ...);
	// 
	mlSynchData& operator>>(int integer);
//	mlSynchData& operator>>(char character);
//	mlSynchData& operator>>(unsigned char uchar);
	mlSynchData& operator>>(unsigned short ushort);
	mlSynchData& operator>>(char* str);
	mlSynchData& operator>>(wchar_t* wstr);
	void get(void* pvoid, int size);
//	int getf(const char* format, ...);
};
*/


mlSynchData::~mlSynchData(){
	if(mpData!=NULL && !mbAssigned) MP_DELETE_ARR( mpData);
	if(mpStr!=NULL) MP_DELETE_ARR( mpStr);
	if(mpWStr!=NULL) MP_DELETE_ARR( mpWStr);
	if(mbJS){
//	mvSavedObject.clear();
	}
}

// увеличит размер буфера до size байт
void mlSynchData::grow(unsigned int size){
	if(mbAssigned) return;
	int iInc=(size)/4;
	if(iInc>1024) iInc=1024;
	iInc+=8;
	miRealSize=size+iInc;
	unsigned char *pData = MP_NEW_ARR( unsigned char, miRealSize);
	if(mpData!=NULL){
		if( miSize <= miRealSize)
			memcpy(pData,mpData,miSize);
		MP_DELETE_ARR( mpData);
	}
	mpData=pData;
//	miSize=size;
}

void mlSynchData::clear(){
	if(mpData != NULL){
		MP_DELETE_ARR( mpData);
		mpData = NULL;
	}
	miSize = 0;
	miRealSize = 0;
	miGetIdx = 0;
	miSetIdx = -1;
}

unsigned int mlSynchData::calculateCheckSum(){
	return calculateCheckSum( mpData, miRealSize);
}

unsigned int mlSynchData::calculateCheckSum( unsigned char* aData, unsigned int aDataSize){
	if(aData == NULL)
		return 0;
	unsigned int sum = 0;
	for (unsigned int i = 0;  i < aDataSize;  i++)
		sum += aData[ i];
	return sum;
}

//	char* mpStr;
//	unsigned short muStrAllocSize;
//	wchar_t* mpWStr;
//	unsigned short muWStrAllocSize;

void mlSynchData::allocstr(unsigned int size){
	if(mpStr!=NULL){
		if(muStrAllocSize >= size) return;
		MP_DELETE_ARR( mpStr);
	}
	mpStr = MP_NEW_ARR( char, (muStrAllocSize=size*2)+1);
}

void mlSynchData::allocwstr(unsigned int size){
	if(mpWStr!=NULL){
		if(muWStrAllocSize >= size) return;
		MP_DELETE_ARR( mpWStr);
	}
	mpWStr = MP_NEW_ARR( wchar_t, (muWStrAllocSize=size*2)+1);
}

#ifdef JSVAL_VOID
mlSynchData& mlSynchData::operator<<(JSContext* cx){
	mcx=cx;
//	mvSavedObject.clear();
	// сначала сохраняем все объекты 
	// (если не RMML-элемент и еще не сохранен или 
	// RMML-элемент и _parent=NULL или у родителя есть сойство с =_name)
	// и нессылочные свойства
//	mbSavingRefs=false;
//	mvParentObjects.clear();
miDepth = 0;
	*this << JS_GetGlobalObject(mcx);
//	// потом сохраняем все ссылочные свойства
//	mbSavingRefs=true;
//	*this << JS_GetGlobalObject(mcx);
//	mbSavingRefs=false;
	return *this;
}

__forceinline mlRMMLElement* GetRMMLElement(JSContext* cx, JSObject* jso){
	JSClass* pClass=ML_JS_GETCLASS(cx, jso);
	if(pClass!=NULL && 
	   (pClass->flags & JSCLASS_HAS_PRIVATE) && JSO_IS_MLEL(cx, jso)
	 ){
		return (mlRMMLElement*)JS_GetPrivate(cx,jso);
	}
	return NULL;
}

// в JSOM_STATE2:
// 1) сохраняются только свойства объектов с synch="..." и ссылки на дочерние элементы
// 2) методы не сохраняются совсем

#define PJSVT_NO		(char)0
#define PJSVT_NULL		(char)1
#define PJSVT_OBJECT	(char)2
#define PJSVT_OBJREF	(char)3
#define PJSVT_FUNCTION	(char)4
#define PJSVT_STRING	(char)5
#define PJSVT_INT8		(char)6
#define PJSVT_INT16		(char)7
#define PJSVT_INT31		(char)8

#define PJSVT_DOUBLE	(char)9
#define PJSVT_BOOLEAN	(char)10
#define PJSVT_GO		(char)11
#define PJSVT_OBJSREF	(char)12
// PJSVT_OBJLINK - синхронизируемая ссылка (mlSynchLinkJSO)
#define PJSVT_OBJLINK	(char)13


mlSynchData& mlSynchData::operator<<(JSObject* jso){
	// сохранияем ID объекта
	// заменено на 0 для хеша свойств объектов.
	//*this << (int)jso;
	*this << (int)0;
	bool bMode2SynchedObject = false;
	if((meJSOutMode == JSOM_STATE || meJSOutMode == JSOM_STATE2) && JSO_IS_MLEL(mcx, jso)){
		mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx,jso);
		if(meJSOutMode == JSOM_STATE2 && !pMLEl->MayHaveSynchChildren()){
			*this << (unsigned char)0;
			return *this; 
		}
		if(meJSOutMode == JSOM_STATE2 && pMLEl->NeedToSynchronize())
			bMode2SynchedObject = true;
		if(meJSOutMode == JSOM_STATE || bMode2SynchedObject){
			*this << (unsigned char)1;
			pMLEl->GetState(*this);
		}else{
			*this << (unsigned char)2;
		}
	}
	JSIdArray* pGPropIDArr = JS_Enumerate(mcx, jso);
	int iLen = pGPropIDArr->length;
	while(iLen > 0xFE){
		unsigned char chLen=0xFF;
		*this << chLen;
		iLen-=0xFE;
	}
	*this << (unsigned char)iLen;
	iLen = pGPropIDArr->length;
	for(int ikl=0; ikl<iLen; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(!JS_IdToValue(mcx,id,&v)){
			
		}
		jschar* jscProp = NULL;
		jsval vProp;
		if(JSVAL_IS_STRING(v)){
			jscProp=JS_GetStringChars(JSVAL_TO_STRING(v));
			*this << jscProp;
//if(isEqual(jscProp,L"_depth"))
//int hh=0;
			JS_GetUCProperty(mcx, jso, jscProp, -1, &vProp);
			unsigned int uAttrs=0;
			JSBool bAttrFound;
			JS_GetUCPropertyAttributes(mcx, jso, jscProp, -1, &uAttrs, &bAttrFound);
			if(meJSOutMode!=JSOM_TREE && (uAttrs & JSPROP_PERMANENT)){
				// какой-то глобальный объект (Mouse, Key, Player)
				// (или неудаляемое свойство (??))
				*this << PJSVT_GO;
				// ??
				continue;
			}
//				if(!mbSavingRefs){
//				if(uAttrs & JSPROP_READONLY) continue;
		}else if(JSVAL_IS_INT(v)){
			// элемент массива
			*this << L"\1";
			JS_GetElement(mcx, jso, JSVAL_TO_INT(v), &vProp);
		}
		// если JSOM_STATE2, то пропускаем все свойства несинхронизируемых объектов 
		if(meJSOutMode == JSOM_STATE2 && !bMode2SynchedObject && !JSVAL_IS_OBJECT(vProp)){
			*this << PJSVT_NO;
			continue;
		}
		if(JSVAL_IS_INT(vProp)){
			int iVal=JSVAL_TO_INT(vProp);
			int iAbsVal = iVal; if(iAbsVal < 0) iAbsVal *= -1;
			if(iAbsVal < (1<<7)){
				*this << PJSVT_INT8;
				*this << (char)iVal;
			}else if(iAbsVal < (1<<15)){
				*this << PJSVT_INT16;
				*this << (short)iVal;
			}else{
				*this << PJSVT_INT31;
				*this << iVal;
			}
		}else if(JSVAL_IS_BOOLEAN(vProp)){
			bool bVal=JSVAL_TO_BOOLEAN(vProp);
			*this << PJSVT_BOOLEAN;
			*this << bVal;
		}else if(JSVAL_IS_STRING(vProp)){
			jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(vProp));
			*this << PJSVT_STRING;
			*this << jsc;
		}else if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
			// Проверка нужна для случая, если в synch добавляют undefined свойство.
			// Иначе на компе owner`а оно будет undefined, а на всех остальных null.
			// Проще чтобы на всех null была.
			*this << PJSVT_NULL;
		}else if(JSVAL_IS_DOUBLE(vProp)){
			jsdouble* dVal = JSVAL_TO_DOUBLE(vProp);
			*this << PJSVT_DOUBLE;
			*this << *dVal;
		}else if(JSVAL_IS_OBJECT(vProp)){
			if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
				JSObject* jsoFun=JSVAL_TO_OBJECT(vProp);
				JSClass* pClass=ML_JS_GETCLASS(mcx, jsoFun);
//JSString * JS_DecompileScript(JSContext *cx, JSScript *script,
//    const char *name, uintN indent);
				if(pClass->flags==1 || meJSOutMode == JSOM_STATE || meJSOutMode == JSOM_STATE2){
					// Script
					*this << PJSVT_NO;
				}else{
					JSFunction* jsf=JS_ValueToFunction(mcx, vProp);
					if(jsf){
						JSString* jssFun=JS_DecompileFunction(mcx, jsf, 0);
						jschar* jsc=JS_GetStringChars(jssFun);
						*this << PJSVT_FUNCTION;
						*this << jsc;
					}
				}
			}else{
//if(jscProp != NULL && isEqual(jscProp, L"eventOwner"))
//int hh=0;
//				if(jscProp != NULL && jscProp[0] != L'\0' && jscProp[1] != L'\0' && jscProp[2] == L'\0' && (jscProp[0] > 255 || jscProp[1] > 255) )
//					int hh=0;
				JSObject* jsoP=JSVAL_TO_OBJECT(vProp);
				bool bRef=false;
				bool bParent = false;
				// если объект есть среди родительских, то сохранять его как ссылку
//				std::vector<JSObject*>::const_iterator citer;
//				for(citer = mvParentObjects.begin(); citer != mvParentObjects.end(); citer++){
//					if(*citer == jsoP){ 
//						bRef=true; break; 
//					}
//				}
				if(!bRef){
					mlRMMLElement* pMLElThis = GetRMMLElement(mcx, jso);
					if(pMLElThis){
						if(pMLElThis->mpParent && pMLElThis->mpParent->mjso == jsoP){ 
							// _parent
							bRef=true;
							bParent = true;
						}else{
							mlRMMLElement* pMLElProp = GetRMMLElement(mcx, jsoP);
							if(pMLElProp != NULL){
								if(pMLElProp->mpParent != NULL){
									if(pMLElProp->mpParent->mjso != jso) // not child
										bRef=true;
								}else
									bRef=true;
							}
						}
					}else{ // pMLElThis == NULL
						if(jso != JS_GetGlobalObject(mcx)){
							// если в обычном объекте есть ссылка на RMML-объект, то его выдавать ссылкой
							mlRMMLElement* pMLElProp = GetRMMLElement(mcx, jsoP);
							if(pMLElProp != NULL)
								bRef=true;
						}
					}
				}
				if(bRef){
					if(bParent || bMode2SynchedObject){
						*this << PJSVT_NO;
					}else{
						*this << PJSVT_OBJREF;
						*this << (int)jsoP;
					}
				}else{ 
//					if(jscProp != NULL && (isEqual(jscProp,L"class") || isEqual(jscProp,L"_class"))){
//						*this << PJSVT_NO;
//						continue;
//					}
//for(int ii = 0; ii < miDepth; ii++)
//	mlTrace(mcx, " ");
//mlTrace(mcx, "%s\n", cLPCSTR(jscProp));
					if(mlSynchLinkJSO::IsInstance(mcx, jsoP)){
						*this << PJSVT_OBJLINK;
						mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoP);
						*this << pSynchLink->muObjectID;
						*this << pSynchLink->muBornRealityID;
					}else{
						*this << PJSVT_OBJECT;
//								if(pMLElThis!=NULL && pMLElThis->mRMMLType==MLMT_SCENE &&
//									isEqual(jscProp,L"scene") && ){
//									*this << "Scene";
//								}
						JSClass* pClass=ML_JS_GETCLASS(mcx, jsoP);
						// сохранить класс объекта
						if(pClass!=NULL){
							*this << pClass->name;
						}else{
							*this << " ";
						}
//							}
miDepth++;
//						mvParentObjects.push_back(jsoP);
						*this << jsoP;
//						mvParentObjects.pop_back();
miDepth--;
					}
				}
			}
		}else{ // unknown jsval type
			JSType jst=JS_TypeOfValue(mcx,vProp);
		}
//			}else{ // if(mbSavingRefs)
//				if(JSVAL_IS_OBJECT(vProp)){
//					// если это ссылка на сохраненый JS-object
//					// то сохранить свойство
//					// ??
//				}
//			}
	}
	JS_DestroyIdArray(mcx, pGPropIDArr);
	return *this;
}

mlSynchData& mlSynchData::operator<<(long vProp){ // jsval
	if(JSVAL_IS_INT(vProp)){
		int iVal=JSVAL_TO_INT(vProp);
		int iAbsVal = iVal; if(iAbsVal < 0) iAbsVal *= -1;
		if(iAbsVal < (1<<7)){
			*this << PJSVT_INT8;
			*this << (char)iVal;
		}else if(iAbsVal < (1<<15)){
			*this << PJSVT_INT16;
			*this << (short)iVal;
		}else{
			*this << PJSVT_INT31;
			*this << iVal;
		}
	}else if(JSVAL_IS_BOOLEAN(vProp)){
		bool bVal=JSVAL_TO_BOOLEAN(vProp);
		*this << PJSVT_BOOLEAN;
		*this << bVal;
	}else if(JSVAL_IS_STRING(vProp)){
		jschar* jsc=JS_GetStringChars(JSVAL_TO_STRING(vProp));
		*this << PJSVT_STRING;
		*this << jsc;
	}else if(JSVAL_IS_NULL(vProp) || JSVAL_IS_VOID(vProp)){
		// Проверка нужна для случая, если в synch добавляют undefined свойство.
			// Иначе на компе owner`а оно будет undefined, а на всех остальных null.
			// Проще чтобы на всех null была.
		*this << PJSVT_NULL;
	}else if(JSVAL_IS_DOUBLE(vProp)){
		jsdouble* dVal=JSVAL_TO_DOUBLE(vProp);
		*this << PJSVT_DOUBLE;
		*this << *dVal;
	}else if(JSVAL_IS_OBJECT(vProp)){
		if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
			JSObject* jsoFun=JSVAL_TO_OBJECT(vProp);
			JSClass* pClass=ML_JS_GETCLASS(mcx, jsoFun);
//JSString * JS_DecompileScript(JSContext *cx, JSScript *script,
//    const char *name, uintN indent);
			if(pClass->flags==1){
				// Script
				*this << PJSVT_NO;
			}else{
				JSFunction* jsf=JS_ValueToFunction(mcx, vProp);
				if(jsf){
					JSString* jssFun=JS_DecompileFunction(mcx, jsf, 0);
					jschar* jsc=JS_GetStringChars(jssFun);
					*this << PJSVT_FUNCTION;
					*this << jsc;
				}
			}
		}else{
			JSObject* jsoP=JSVAL_TO_OBJECT(vProp);
			bool bRef=false;
			mlRMMLElement* pMLEl = GetRMMLElement(mcx, jsoP);
			bRef=(pMLEl != NULL);
			if(bRef){
				*this << PJSVT_OBJSREF;
				mlString sMLElPath = pMLEl->GetStringID();
				*this << sMLElPath.c_str();
			}else{ 
				if(mlSynchLinkJSO::IsInstance(mcx, jsoP)){
					*this << PJSVT_OBJLINK;
					mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoP);
					*this << pSynchLink->muObjectID;
					*this << pSynchLink->muBornRealityID;
				}else{
					*this << PJSVT_OBJECT;
					JSClass* pClass=ML_JS_GETCLASS(mcx, jsoP);
					// сохранить класс объекта
					if(pClass!=NULL){
						*this << pClass->name;
					}else{
						*this << " ";
					}
					*this << jsoP;
				}
			}
		}
	}else{ // unknown jsval type
		JSType jst=JS_TypeOfValue(mcx,vProp);
	}
	return *this;
}

JSObject* mlSynchData::getJsoById(int aiID){	
	for(Id2JsoIter Iter=pId2Jso->begin(); Iter != pId2Jso->end(); Iter++ ){
		Id_Jso* pIJ=*Iter;
		if(pIJ->id==aiID) return pIJ->jso;
	}
	return NULL;
}
void mlSynchData::addRef(JSObject* ajso,const wchar_t* apwcPropName,int aiID){
	JsoRef* pJsoRef = MP_NEW( JsoRef);
	pJsoRef->jso=ajso;
	pJsoRef->prop=apwcPropName;
	pJsoRef->id=aiID;
	pJsoRefs->push_back(pJsoRef);
}
void mlSynchData::setRefs(int aiID, JSObject* jso){
	if(pJsoRefs == NULL) return;
	for(JsoRefsIter Iter=pJsoRefs->begin(); Iter!=pJsoRefs->end(); Iter++){
		JsoRef* pJsoRef=*Iter;
//		JSObject* jso=getJsoById(pJsoRef->id);
		if(pJsoRef->id==aiID){
			jsval vProp=OBJECT_TO_JSVAL(jso);
			wr_JS_SetUCProperty(mcx,pJsoRef->jso,pJsoRef->prop.c_str(),-1,&vProp);
			pJsoRefs->erase(Iter);
			MP_DELETE( pJsoRef);
			Iter=pJsoRefs->begin();
		}
	}
}
mlSynchData& mlSynchData::operator>>(JSContext* &cx){
	mcx=cx;
	MP_NEW_VECTOR(pId2Jso, Id2Jso);
	MP_NEW_LIST(pJsoRefs, JsoRefs);
	JSObject* jso=JS_GetGlobalObject(mcx);
	*this >> jso;
//	*this >> jso; // reading refs ??
	// заполняем ссылки
	// ??
	//
	for(Id2JsoIter Iter=pId2Jso->begin(); Iter != pId2Jso->end(); Iter++ ){
		MP_DELETE( *Iter);
	}
	pId2Jso->clear();
	MP_DELETE( pId2Jso);
	//
	for(JsoRefsIter Iter2=pJsoRefs->begin(); Iter2 != pJsoRefs->end(); Iter2++ ){
		MP_DELETE( *Iter2);
	}
	pJsoRefs->clear();
	MP_DELETE( pJsoRefs);
	return *this;
}

bool mlSynchData::readSafely(JSObject* &jso)
{
	int id;
	if (!MayBeRead(sizeof(id)))
	{
		return false;
	}
	*this >> id;

	if (pId2Jso != NULL) 
	{
		pId2Jso->push_back(new Id_Jso(id,jso));
	}

	JSClass* clas = ML_JS_GETCLASS(mcx, jso);
	if (JSO_IS_MLEL(mcx, jso))
	{
		unsigned char st = 0;
		*this >> st;
		if(st == 0)
		{
			return false;
		}
		if (st == 1)
		{
			mlRMMLElement* pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx,jso);
			pMLEl->SetState(*this);
		}
	}

	// запомнить все свойства, которые были у объекта
	JSIdArray* pGPropIDArr = JS_Enumerate(mcx, jso);
	int iLen = pGPropIDArr->length;
	typedef jschar* pjschar;
	pjschar* aWasProps = MP_NEW_ARR( pjschar, iLen + 1);
	for (int ii = 0; ii < iLen; ii++)
	{
		jsval v;
		JS_IdToValue(mcx, pGPropIDArr->vector[ii], &v);
		if (JSVAL_IS_STRING(v))
		{
			aWasProps[ii] = JS_GetStringChars(JSVAL_TO_STRING(v));
		}
		else
		{
			aWasProps[ii] = NULL;
		}
	}
	JS_DestroyIdArray(mcx, pGPropIDArr);
	pjschar* pWasPropsBegin = aWasProps;
	pjschar* pWasPropsEnd = &(aWasProps[iLen]);
	// 
	unsigned char iPropCnt;
	if (!MayBeRead(sizeof(iPropCnt)))
	{
		return false;
	}
	*this >> iPropCnt;

	for(int iProp = 0; iProp < iPropCnt; iProp++)
	{
		wchar_t* jscProp;
		(*this) >> jscProp;
		if (jscProp == NULL)
		{
			return false;
		}

		wstring wsPropName = jscProp;
		bool bSet = false;
		jsval vIs;
		if (wr_JS_GetUCProperty(mcx,jso,jscProp,-1,&vIs))
		{
			// если такое свойство есть, 
			for (pjschar* pWasProp = pWasPropsBegin; pWasProp != pWasPropsEnd; pWasProp++)
			{
				if (*pWasProp == NULL) 
				{
					continue;
				}
				if (isEqual( (const wchar_t *) *pWasProp, jscProp))
				{
					// то вычеркиваем его из списка свойств на удаление
					*pWasProp = NULL;
					break;
				}
			}
			while((*pWasPropsBegin == NULL) && (pWasPropsBegin != pWasPropsEnd))
			{
				pWasPropsBegin++;
			}
		}

		jsval vProp = JSVAL_NULL;
		char chPropType;
		if (!MayBeRead(sizeof(chPropType)))
		{
			return false;
		}
		*this >> chPropType;

		JSObject* jsoSavedFromGC = NULL;
		switch(chPropType)
		{
		case PJSVT_NO:
			continue;
			break;
		case PJSVT_GO:
			// ??
			break;
		case PJSVT_INT8:
			{
				char iVal;
				if (!MayBeRead(sizeof(iVal)))
				{
					return false;
				}
				*this >> iVal;
				vProp = INT_TO_JSVAL(iVal);
				if (vProp != vIs) bSet=true;
			}
			break;
		case PJSVT_INT16:
			{
				short iVal;
				if (!MayBeRead(sizeof(iVal)))
				{
					return false;
				}
				*this >> iVal;
				vProp = INT_TO_JSVAL(iVal);
				if (vProp != vIs) bSet=true;
			}
			break;
		case PJSVT_INT31:
			{
				int iVal;
				if (!MayBeRead(sizeof(iVal)))
				{
					return false;
				}
				*this >> iVal;
				vProp = INT_TO_JSVAL(iVal);
				if (vProp != vIs) bSet=true;
			}
			break;
		case PJSVT_BOOLEAN:
			{
				bool bVal;
				if (!MayBeRead(sizeof(bVal)))
				{
					return false;
				}
				*this >> bVal;
				vProp = BOOLEAN_TO_JSVAL(bVal);
				if (vProp != vIs) bSet = true;
			}
			break;
		case PJSVT_STRING:
			{
				wchar_t* jscVal;
				(*this) >> jscVal;
				if (jscVal == NULL)
				{
					return false;
				}
				if(!JSVAL_IS_STRING(vIs))
				{
					vProp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,jscVal));
					bSet = true;
				}
				else if(!isEqual(JSVAL_TO_STRING(vIs),jscVal))
				{
					vProp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,jscVal));
					bSet = true;
				}
			}
			break;
		case PJSVT_NULL:
			vProp = JSVAL_NULL;
			if (vProp != vIs) bSet=true;
			break;
		case PJSVT_DOUBLE:
			{
				jsdouble jsdProp = 0.0;
				if (!MayBeRead(sizeof(jsdProp)))
				{
					return false;
				}
				(*this) >> jsdProp;
				if(!JSVAL_IS_DOUBLE(vIs))
				{
					mlJS_NewDoubleValue(mcx,jsdProp,&vProp);
					bSet=true;
				}
				else
				{
					jsdouble jsdIs=*JSVAL_TO_DOUBLE(vIs);
					if(jsdIs!=jsdProp)
					{
						mlJS_NewDoubleValue(mcx,jsdProp,&vProp);
						bSet=true;
					}
				}
			}
			break;
		case PJSVT_FUNCTION:
			{
				wchar_t* jscFun;
				(*this) >> jscFun;
				if (jscFun == NULL)
				{
					return false;
				}
				JSFunction* jsf=JS_CompileFunction(mcx,jso,cLPCSTR((wchar_t*)wsPropName.c_str()),0,NULL,cLPCSTR(jscFun),wcslen(jscFun),NULL,0);
				JSObject* jsoFun=JS_GetFunctionObject(jsf);
				jsval vFun;
				JSBool bRet=JS_ConvertValue(mcx, OBJECT_TO_JSVAL(jsoFun), JSTYPE_FUNCTION, &vFun);
				vProp = vFun;
				bSet = true;
			}
			break;
		case PJSVT_OBJECT:
			{
				char* pszClassName = NULL;
				*this >> pszClassName;
				JSObject* jsoProp=NULL;
				if(!JSVAL_IS_NULL(vIs) && JSVAL_IS_OBJECT(vIs))
				{
					jsoProp=JSVAL_TO_OBJECT(vIs);
					JSClass* pIsClass=ML_JS_GETCLASS(mcx, jsoProp);
					if(!isEqual(pIsClass->name,pszClassName))
					{
						return false;
					}
				}
				if(jsoProp==NULL)
				{
					if(isEqual(pszClassName,"global"))
					{
						jsoProp = JS_NewObject(mcx,NULL,NULL,NULL);
					}
					else if (isEqual(pszClassName,"Array"))
					{
						jsval va[1];
						jsoProp = JS_NewArrayObject(mcx, 0, va);
					}
					else
					{
						jsoProp = GPSM(mcx)->CreateElementByClassName(pszClassName);
						if (jsoProp==NULL)
						{
							return false;
						}
					}
					vProp=OBJECT_TO_JSVAL(jsoProp);
					bSet=true;
				}
				SAVE_FROM_GC(mcx, JS_GetGlobalObject(mcx), jsoProp);
				jsoSavedFromGC = jsoProp;

				//mlRMMLElement* pMLEl = NULL;
				//if(JSO_IS_MLEL(mcx, jsoProp))
				//	pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx,jsoProp);
				if (!readSafely(jsoProp))
				{
					FREE_FOR_GC(mcx, JS_GetGlobalObject(mcx), jsoSavedFromGC);
					return false;
				}
			}
			break;
		case PJSVT_OBJREF:
			{
				int iID=0; 
				if (!MayBeRead(sizeof(iID)))
				{
					return false;
				}
				*this >> iID;
				JSObject* jsoProp=getJsoById(iID);
				if(jsoProp!=NULL)
				{
					vProp=OBJECT_TO_JSVAL(jsoProp);
					bSet=true;
				}
				else
				{
					addRef(jso,wsPropName.c_str(),iID);
				}
			}
			break;
		case PJSVT_OBJLINK:
			{
				unsigned int uID=0; 
				if (!MayBeRead(sizeof(uID)))
				{
					return false;
				}
				*this >> uID;
				unsigned int uLinkBornRealityID = 0; 
				if (!MayBeRead(sizeof(uLinkBornRealityID)))
				{
					return false;
				}
				*this >> uLinkBornRealityID;

				JSObject* jsoProp = mlSynchLinkJSO::newJSObject(mcx);

				mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoProp);
				pSynchLink->muObjectID = uID;
				pSynchLink->muBornRealityID = uLinkBornRealityID;

				vProp=OBJECT_TO_JSVAL(jsoProp);
				bSet=true;
			}
			break;
		default:
			return false;
		}

		if(bSet)
		{
			wr_JS_SetUCProperty(mcx, jso, wsPropName.c_str(), -1, &vProp);
		}
		if(jsoSavedFromGC != NULL){
			FREE_FOR_GC(mcx, JS_GetGlobalObject(mcx), jsoSavedFromGC);
		}
	}
	
	for(pjschar* pWasProp = pWasPropsBegin; pWasProp != pWasPropsEnd; pWasProp++)
	{
		if(*pWasProp == NULL) 
		{
			continue;
		}
		jschar* jscProp = *pWasProp;
		jsval v;
		JS_DeleteUCProperty2(mcx, jso, jscProp, -1, &v);
	}

	MP_DELETE_ARR( aWasProps);

	setRefs(id, jso);
	// если это mlRMMLElement
	if(JSO_IS_MLEL(mcx, jso))
	{
		mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(mcx,jso);
		// и он в сцене
		if(pMLEl->GetScene()!=NULL)
		{
			// то создаем все медиа-объеты
			pMLEl->CreateAndLoadMOs();
		}
	}

	return true;
}

mlSynchData& mlSynchData::operator>>(JSObject* &jso)
{
	readSafely(jso);

	return *this;
}

bool mlSynchData::readSafely(jsval &vProp)
{
	vProp = JSVAL_NULL;
	char chPropType;
	if (!MayBeRead(sizeof(chPropType)))
	{
		return false;
	}

	*this >> chPropType;
    
	switch(chPropType)
	{
	case PJSVT_NO:
		break;
	case PJSVT_GO:
		break;
	case PJSVT_INT8:
		{
			char iVal;
			if (!MayBeRead(sizeof(iVal)))
			{
				return false;
			}
			*this >> iVal;
			vProp = INT_TO_JSVAL(iVal);
		}
		break;
	case PJSVT_INT16:
		{
			short iVal;
			if (!MayBeRead(sizeof(iVal)))
			{
				return false;
			}
			*this >> iVal;
			vProp = INT_TO_JSVAL(iVal);
		}
		break;
	case PJSVT_INT31:
		{
			int iVal;
			if (!MayBeRead(sizeof(iVal)))
			{
				return false;
			}
			*this >> iVal;
			vProp = INT_TO_JSVAL(iVal);
		}
		break;
	case PJSVT_BOOLEAN:
		{
			bool bVal;
			if (!MayBeRead(sizeof(bVal)))
			{
				return false;
			}
			*this >> bVal;
			vProp = BOOLEAN_TO_JSVAL(bVal);
		}
		break;
	case PJSVT_STRING:
		{
			wchar_t* jscVal;
			if (!readSafely(jscVal))
			{
				return false;
			}
			if(*jscVal == L'\0')
				vProp = JS_GetEmptyStringValue(mcx);
			else
				vProp = STRING_TO_JSVAL(wr_JS_NewUCStringCopyZ(mcx,jscVal));
		}
		break;
	case PJSVT_NULL:
		vProp=JSVAL_NULL;
		break;
	case PJSVT_DOUBLE:
		{
			jsdouble jsdProp;
			if (!MayBeRead(sizeof(jsdProp)))
			{
				return false;
			}
			*this >> jsdProp;
			mlJS_NewDoubleValue(mcx, jsdProp, &vProp);
		}
		break;
	case PJSVT_FUNCTION:
		{
			wchar_t* jscFun;
			if (!readSafely(jscFun))
			{
				return false;
			}
			JSFunction* jsf = JS_CompileFunction(mcx, JS_GetGlobalObject(mcx), "argFun", 0, NULL, cLPCSTR(jscFun), wcslen(jscFun), NULL, 0);
			JSObject* jsoFun = JS_GetFunctionObject(jsf);
			jsval vFun;
			JSBool bRet = JS_ConvertValue(mcx, OBJECT_TO_JSVAL(jsoFun), JSTYPE_FUNCTION, &vFun);
			vProp = vFun;
		}
		break;
	case PJSVT_OBJECT:
		{
			char* pszClassName = NULL;
			if (!readSafely(pszClassName))
			{
				return false;
			}
			JSObject* jsoProp = NULL;
			if (jsoProp == NULL)
			{
				if (isEqual(pszClassName,"global"))
				{
					jsoProp = JS_NewObject(mcx,NULL,NULL,NULL);
				}
				else if (isEqual(pszClassName,"Array"))
				{
					jsval va[1];
					jsoProp = JS_NewArrayObject(mcx, 0, va);
				}
				else
				{
					jsoProp = GPSM(mcx)->CreateElementByClassName(pszClassName);
					if (jsoProp == NULL)
					{
						return false;
					}
				}
				vProp=OBJECT_TO_JSVAL(jsoProp);
			}
			// временно защитить вновь созданный объект от сборщика мусора
			SAVE_FROM_GC(mcx, JS_GetGlobalObject(mcx), jsoProp);

			mlRMMLElement* pMLEl = NULL;
			if (JSO_IS_MLEL(mcx, jsoProp))
			{
				pMLEl = (mlRMMLElement*)JS_GetPrivate(mcx, jsoProp);
			}
			*this >> jsoProp;
			FREE_FOR_GC(mcx, JS_GetGlobalObject(mcx), jsoProp);
		}
		break;
	case PJSVT_OBJREF:
		{
			int iID = 0; 
			if (!MayBeRead(sizeof(iID)))
			{
				return false;
			}
			*this >> iID;
			JSObject* jsoProp=getJsoById(iID);
			if(jsoProp!=NULL)
			{
				vProp=OBJECT_TO_JSVAL(jsoProp);
			}
			else
			{
	//			addRef(jso,wsPropName.c_str(),iID);
				// ??
			}
		}
		break;
	case PJSVT_OBJSREF:
		{
			wchar_t* pwcMLElStringID = NULL;
			if (!readSafely(pwcMLElStringID))
			{
				return false;
			}
			mlRMMLElement* pMLEl = GPSM(mcx)->FindElemByStringID(pwcMLElStringID);
			if (pMLEl != NULL)
			{
				vProp = OBJECT_TO_JSVAL(pMLEl->mjso);
			}
		}
		break;
	case PJSVT_OBJLINK:
		{
			unsigned int uID = 0; 
			if (!MayBeRead(sizeof(uID)))
			{
				return false;
			}
			*this >> uID;
			unsigned int uBornRealityID = 0;
			if (!MayBeRead(sizeof(uBornRealityID)))
			{
				return false;
			}
			*this >> uBornRealityID;
			JSObject* jsoProp = mlSynchLinkJSO::newJSObject(mcx);
			mlSynchLinkJSO* pSynchLink = (mlSynchLinkJSO*)JS_GetPrivate(mcx, jsoProp);
			pSynchLink->muObjectID = uID;
			pSynchLink->muBornRealityID = uBornRealityID;
			vProp = OBJECT_TO_JSVAL(jsoProp);
		}
		break;
	default:
		return false;
	}

	return true;
}

mlSynchData& mlSynchData::operator>>(long &vProp){ // jsval
	jsval jsvProp;
	readSafely(jsvProp);
	vProp = jsvProp;
	return *this;
}

#endif

} // namespace rmml
