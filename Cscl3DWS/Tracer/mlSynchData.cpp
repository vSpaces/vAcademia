
#include "stdafx.h"
#include "mlSynchData.h"
//#include "..\oms\oms_context.h"
//#include "jsapi.h"
//#include "rmml.h"
#ifdef JSVAL_OBJECT
#include "mlRMML.h"
#endif

#pragma warning( disable : 4800 )

//namespace rmml{
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
	if(mpData!=NULL && !mbAssigned) delete mpData;
	if(mpStr!=NULL) delete mpStr;
	if(mpWStr!=NULL) delete mpWStr;
	if(mbJS){
//	mvSavedObject.clear();
	}
}

// увеличит размер буфера до size байт
void mlSynchData::grow(int size){
	if(mbAssigned) return;
	int iInc=(size)/4;
	if(iInc>1024) iInc=1024;
	iInc+=8;
	miRealSize=size+iInc;
	unsigned char* pData=new unsigned char[miRealSize];
	if(mpData!=NULL){
		memcpy(pData,mpData,miSize);
		delete mpData;
	}
	mpData=pData;
//	miSize=size;
}

//	char* mpStr;
//	unsigned short muStrAllocSize;
//	wchar_t* mpWStr;
//	unsigned short muWStrAllocSize;

void mlSynchData::allocstr(int size){
	if(mpStr!=NULL){
		if(muStrAllocSize >= size) return;
		delete mpStr;
	}
	mpStr=new char[(muStrAllocSize=size*2)+1];
}

void mlSynchData::allocwstr(int size){
	if(mpWStr!=NULL){
		if(muWStrAllocSize >= size) return;
		delete mpWStr;
	}
	mpWStr=new wchar_t[(muWStrAllocSize=size*2)+1];
}

#ifdef JSVAL_OBJECT
mlSynchData& mlSynchData::operator<<(JSContext* cx){
	mcx=cx;
//	mvSavedObject.clear();
	// сначала сохраняем все объекты 
	// (если не RMML-элемент и еще не сохранен или 
	// RMML-элемент и _parent=NULL или у родителя есть сойство с =_name)
	// и нессылочные свойства
//	mbSavingRefs=false;
	*this << JS_GetGlobalObject(mcx);
//	// потом сохраняем все ссылочные свойства
//	mbSavingRefs=true;
//	*this << JS_GetGlobalObject(mcx);
//	mbSavingRefs=false;
	return *this;
}
mlRMMLElement* GetRMMLElement(JSContext* cx, JSObject* jso){
	JSClass* pClass=JS_GetClass(jso);
	if(pClass!=NULL && 
	   (pClass->flags & JSCLASS_HAS_PRIVATE) && 
	   pClass->spare==1){	
		return (mlRMMLElement*)JS_GetPrivate(cx,jso);
	}
	return NULL;
}
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
mlSynchData& mlSynchData::operator<<(JSObject* jso){
	// сохранияем ID объекта
	*this << (int)jso;
	if(JS_GetClass(jso)->spare==1){
		mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(mcx,jso);
		pMLEl->GetState(*this);
	}
	JSIdArray* pGPropIDArr= JS_Enumerate(mcx, jso);
	int iLen=pGPropIDArr->length;
	while(iLen > 0xFE){
		unsigned char chLen=0xFF;
		*this << chLen;
		iLen-=0xFE;
	}
	*this << (unsigned char)iLen;
	iLen=pGPropIDArr->length;
	for(int ikl=0; ikl<iLen; ikl++){
		jsid id=pGPropIDArr->vector[ikl];
		jsval v;
		if(JS_IdToValue(mcx,id,&v)){
			if(JSVAL_IS_STRING(v)){
				jschar* jscProp=JS_GetStringChars(JSVAL_TO_STRING(v));
				*this << jscProp;
				jsval vProp;
				JS_GetUCProperty(mcx, jso, jscProp, -1, &vProp);
				unsigned int uAttrs=0;
				JSBool bAttrFound;
				JS_GetUCPropertyAttributes(mcx, jso, jscProp, -1, &uAttrs, &bAttrFound);
//				if(!mbSavingRefs){
//				if(uAttrs & JSPROP_READONLY) continue;
				if(uAttrs & JSPROP_PERMANENT){
					// какой-то глобальный объект (Mouse, Key, Player)
					*this << PJSVT_GO;
					// ??
					continue;
				}
				if(JSVAL_IS_INT(vProp)){
					int iVal=JSVAL_TO_INT(vProp);
					if(iVal < (1<<8)){
						*this << PJSVT_INT8;
						*this << (unsigned char)iVal;
					}else if(iVal < (1<<16)){
						*this << PJSVT_INT16;
						*this << (unsigned short)iVal;
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
				}else if(JSVAL_IS_NULL(vProp)){
					*this << PJSVT_NULL;
				}else if(JSVAL_IS_DOUBLE(vProp)){
					jsdouble* dVal=JSVAL_TO_DOUBLE(vProp);
					*this << PJSVT_DOUBLE;
					*this << *dVal;
				}else if(JSVAL_IS_OBJECT(vProp)){
					if(JS_TypeOfValue(mcx,vProp)==JSTYPE_FUNCTION){
						JSObject* jsoFun=JSVAL_TO_OBJECT(vProp);
						JSClass* pClass=JS_GetClass(jsoFun);
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
						mlRMMLElement* pMLElThis=GetRMMLElement(mcx, jso);
						if(pMLElThis){
							if(pMLElThis->mpParent && pMLElThis->mpParent->mjso == jsoP){ 
								// _parent
								bRef=true;
							}else{
								mlRMMLElement* pMLElProp=GetRMMLElement(mcx, jsoP);
								if(pMLElProp && pMLElProp->mpParent && pMLElProp->mpParent->mjso != jso) // not child
									bRef=true;
							}
						}
						if(bRef){
							*this << PJSVT_OBJREF;
							*this << (int)jsoP;
						}else{ 
							*this << PJSVT_OBJECT;
//							if(pMLElThis!=NULL && pMLElThis->mRMMLType==MLMT_SCENE &&
//								isEqual(jscProp,L"scene") && ){
//								*this << "Scene";
//							}
							JSClass* pClass=JS_GetClass(jsoP);
							// сохранить класс объекта
							if(pClass!=NULL){
								*this << pClass->name;
							}else{
								*this << " ";
							}
//							}
							*this << jsoP;
						}
					}
				}else // unknown jsval type
				{
				}
//			}else{ // if(mbSavingRefs)
//				if(JSVAL_IS_OBJECT(vProp)){
//					// если это ссылка на сохраненый JS-object
//					// то сохранить свойство
//					// ??
//				}
//			}
			}else{ // prop id is not a string
			}
		}
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
	JsoRef* pJsoRef=new JsoRef;
	pJsoRef->jso=ajso;
	pJsoRef->prop=apwcPropName;
	pJsoRef->id=aiID;
	pJsoRefs->push_back(pJsoRef);
}
void mlSynchData::setRefs(int aiID, JSObject* jso){
	for(JsoRefsIter Iter=pJsoRefs->begin(); Iter!=pJsoRefs->end(); Iter++){
		JsoRef* pJsoRef=*Iter;
//		JSObject* jso=getJsoById(pJsoRef->id);
		if(pJsoRef->id==aiID){
			jsval vProp=OBJECT_TO_JSVAL(jso);
			JS_SetUCProperty(mcx,pJsoRef->jso,pJsoRef->prop.c_str(),-1,&vProp);
			pJsoRefs->erase(Iter);
			delete pJsoRef;
			Iter=pJsoRefs->begin();
		}
	}
}
mlSynchData& mlSynchData::operator>>(JSContext* &cx){
	mcx=cx;
	pId2Jso=new Id2Jso;
	pJsoRefs=new JsoRefs;
	JSObject* jso=JS_GetGlobalObject(mcx);
	*this >> jso;
//	*this >> jso; // reading refs ??
	// заполняем ссылки
	// ??
	//
	for(Id2JsoIter Iter=pId2Jso->begin(); Iter != pId2Jso->end(); Iter++ ){
		delete *Iter;
	}
	pId2Jso->clear();
	delete pId2Jso;
	//
	for(JsoRefsIter Iter2=pJsoRefs->begin(); Iter2 != pJsoRefs->end(); Iter2++ ){
		delete *Iter2;
	}
	pJsoRefs->clear();
	delete pJsoRefs;
	return *this;
}
mlSynchData& mlSynchData::operator>>(JSObject* &jso){
	int id;
	*this >> id;
	pId2Jso->push_back(new Id_Jso(id,jso));
	if(JS_GetClass(jso)->spare==1){
		mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(mcx,jso);
		pMLEl->SetState(*this);
	}
	//
	unsigned char iPropCnt;
	*this >> iPropCnt;
	for(int iProp=0; iProp<iPropCnt; iProp++){
		jschar* jscProp;
		*this >> jscProp;
		wstring wsPropName=jscProp;
		bool bSet=false;
		jsval vIs;
		if(JS_GetUCProperty(mcx,jso,jscProp,-1,&vIs)){
			// ??
		}
		jsval vProp=JSVAL_NULL;
		char chPropType;
		*this >> chPropType;
		switch(chPropType){
		case PJSVT_NO:
			continue;
			break;
		case PJSVT_GO:
			// ??
			break;
		case PJSVT_INT8:{
			unsigned char iVal;
			*this >> iVal;
			vProp=INT_TO_JSVAL(iVal);
			if(vProp != vIs) bSet=true;
			}break;
		case PJSVT_INT16:{
			unsigned short iVal;
			*this >> iVal;
			vProp=INT_TO_JSVAL(iVal);
			if(vProp != vIs) bSet=true;
			}break;
		case PJSVT_INT31:{
			int iVal;
			*this >> iVal;
			vProp=INT_TO_JSVAL(iVal);
			if(vProp != vIs) bSet=true;
			}break;
		case PJSVT_BOOLEAN:{
			bool bVal;
			*this >> bVal;
			vProp=BOOLEAN_TO_JSVAL(bVal);
			if(vProp != vIs) bSet=true;
			}break;
		case PJSVT_STRING:{
			jschar* jscVal;
			*this >> jscVal;
			if(!JSVAL_IS_STRING(vIs)){
				vProp=STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx,jscVal));
				bSet=true;
			}else if(!isEqual(JSVAL_TO_STRING(vIs),jscVal)){
				vProp=STRING_TO_JSVAL(JS_NewUCStringCopyZ(mcx,jscVal));
				bSet=true;
			}
			}break;
		case PJSVT_NULL:
			vProp=JSVAL_NULL;
			if(vProp != vIs) bSet=true;
			break;
		case PJSVT_DOUBLE:{
			jsdouble jsdProp;
			*this >> jsdProp;
			if(!JSVAL_IS_DOUBLE(vIs)){
				JS_NewDoubleValue(mcx,jsdProp,&vProp);
				bSet=true;
			}else{
				jsdouble jsdIs=*JSVAL_TO_DOUBLE(vIs);
				if(jsdIs!=jsdProp){
					JS_NewDoubleValue(mcx,jsdProp,&vProp);
					bSet=true;
				}
			}
			}break;
		case PJSVT_FUNCTION:{
			jschar* jscFun;
			*this >> jscFun;
			JSFunction* jsf=JS_CompileFunction(mcx,jso,cLPCSTR((wchar_t*)wsPropName.c_str()),0,NULL,cLPCSTR(jscFun),wcslen(jscFun),NULL,0);
			JSObject* jsoFun=JS_GetFunctionObject(jsf);
			jsval vFun;
			JSBool bRet=JS_ConvertValue(mcx, OBJECT_TO_JSVAL(jsoFun), JSTYPE_FUNCTION, &vFun);
//JS_CompileFunction(JSContext *cx, JSObject *obj, const char *name,
//                   uintN nargs, const char **argnames,
//                   const char *bytes, size_t length,
//                   const char *filename, uintN lineno);
//			bRet=JS_DefineUCProperty(mcx,jso,jscProp,-1,vFun,0,0,0);
			vProp=vFun;
			bSet=true;
			}break;
		case PJSVT_OBJECT:{
			char* pszClassName=NULL;
			*this >> pszClassName;
			JSObject* jsoProp=NULL;
			if(!JSVAL_IS_NULL(vIs) && JSVAL_IS_OBJECT(vIs)){
				jsoProp=JSVAL_TO_OBJECT(vIs);
				JSClass* pIsClass=JS_GetClass(jsoProp);
				if(!isEqual(pIsClass->name,pszClassName))
				{
				}
			}
			if(jsoProp==NULL){
				if(isEqual(pszClassName,"global")){
					jsoProp = JS_NewObject(mcx,NULL,NULL,NULL);
				}else{
					jsoProp=gpSM->CreateElementByClassName(pszClassName);
				}
				vProp=OBJECT_TO_JSVAL(jsoProp);
				bSet=true;
			}
mlRMMLElement* pMLEl=NULL;
if(JS_GetClass(jsoProp)->spare==1)
pMLEl=(mlRMMLElement*)JS_GetPrivate(mcx,jsoProp);
			*this >> jsoProp;
			}break;
		case PJSVT_OBJREF:{
			int iID=0; 
			*this >> iID;
			JSObject* jsoProp=getJsoById(iID);
			if(jsoProp!=NULL){
				vProp=OBJECT_TO_JSVAL(jsoProp);
				bSet=true;
			}else{
				addRef(jso,wsPropName.c_str(),iID);
			}
			}break;
		default:
			break;
		}
		if(bSet){
			JS_SetUCProperty(mcx,jso,wsPropName.c_str(),-1,&vProp);
		}
	}
	setRefs(id,jso);
	// если это mlRMMLElement
	if(JS_GetClass(jso)->spare==1){
		mlRMMLElement* pMLEl=(mlRMMLElement*)JS_GetPrivate(mcx,jso);
		// и он в сцене
		if(pMLEl->GetScene()!=NULL){
			// то создаем все медиа-объеты
			pMLEl->CreateAndLoadMOs();
		}
	}
	return *this;
}
#endif

//} // namespace rmml
