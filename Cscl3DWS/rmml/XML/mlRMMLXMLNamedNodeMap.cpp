
#include "mlRMML.h"

namespace rmml {

mlXMLNamedNodeMap::mlXMLNamedNodeMap(void):
	MP_VECTOR_INIT(mvNodeMap)
{
}

mlXMLNamedNodeMap::~mlXMLNamedNodeMap(void){
	mvNodeMap.clear();
}

/***************************
 * реализация mlXMLNamedNodeMap
 */

/*
interface NamedNodeMap {
  Node                      getNamedItem(in DOMString name);
  Node                      setNamedItem(in Node arg)
                                         raises(DOMException);
  Node                      removeNamedItem(in DOMString name)
                                            raises(DOMException);
  Node                      item(in unsigned long index);
  readonly attribute  unsigned long        length;
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLNamedNodeMap::_JSPropertySpec[] = {
	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLNamedNodeMap::_JSFunctionSpec[] = {
	JSFUNC(getNamedItem,1)
	JSFUNC(setNamedItem,1)
	JSFUNC(removeNamedItem,1)
	JSFUNC(item,1)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(__NamedNodeMap_,mlXMLNamedNodeMap,0)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLNamedNodeMap)

///// JavaScript Set Property Wrapper
JSBool mlXMLNamedNodeMap::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLNamedNodeMap);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLNamedNodeMap::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlXMLNamedNodeMap);
		default:
			switch(iID){
			case JSPROP_length:
				*vp=INT_TO_JSVAL(priv->mvNodeMap.size());
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlXMLNamedNodeMap::JSFUNC_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlXMLNamedNodeMap* pNNM=(mlXMLNamedNodeMap*)JS_GetPrivate(cx, obj);
	if (argc != 1){
		JS_ReportError(cx, "item method must get one integer argument");
		return JS_TRUE;
	}
	if(JSVAL_IS_INT(argv[0])){
		int iIdx=JSVAL_TO_INT(argv[0]);
		if((unsigned int)iIdx < pNNM->mvNodeMap.size()){
			*rval=OBJECT_TO_JSVAL(pNNM->mvNodeMap[iIdx]);
			return JS_TRUE;
		}
	}else{
		JS_ReportError(cx, "getNamedItem method must get one integer argument");
	}
	*rval = JSVAL_NULL;
	return JS_TRUE;
}

JSObject* mlXMLNamedNodeMap::item(unsigned int auIndex){
	if(auIndex >= mvNodeMap.size()) return NULL;
	return mvNodeMap[auIndex];
}

JSBool mlXMLNamedNodeMap::JSFUNC_getNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc != 1){
		JS_ReportError(cx, "getNamedItem method must get one integer argument");
		return JS_TRUE;
	}
	*rval = JSVAL_NULL;
	if(JSVAL_IS_STRING(argv[0])){
		JSString* jssName=JSVAL_TO_STRING(argv[0]);
		mlXMLNamedNodeMap* pNNM=(mlXMLNamedNodeMap*)JS_GetPrivate(cx, obj);
		JSObject* jsoNode=pNNM->getNamedItem(jssName);
		if(jsoNode != NULL)
			*rval=OBJECT_TO_JSVAL(jsoNode);
	}else{
		JS_ReportError(cx, "getNamedItem method must get a string argument");
	}
	return JS_TRUE;
}

JSObject* mlXMLNamedNodeMap::getNamedItem(JSString* ajssName){
	int iIdx=FindNodeByName(ajssName);
	if(iIdx<0) return NULL;
	return mvNodeMap[iIdx];
}

//  Node setNamedItem(in Node arg)
JSBool mlXMLNamedNodeMap::JSFUNC_setNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlXMLNamedNodeMap* pNNM=(mlXMLNamedNodeMap*)JS_GetPrivate(cx, obj);
	*rval = JSVAL_FALSE;
	if (argc != 1){
		JS_ReportError(cx, "setNamedItem method must get XML attribute object");
		return JS_TRUE;
	}
	if(JSVAL_IS_REAL_OBJECT(argv[0])){
		JSObject* jsoNode = JSVAL_TO_OBJECT(argv[0]);
		if(jsoNode == NULL || (!mlXMLAttr::IsInstance(cx, jsoNode))){
			JS_ReportError(cx, "setNamedItem method must get XML attribute object");
			return JS_TRUE;
		}
		if(pNNM->setNamedItem(jsoNode))
			*rval = JSVAL_TRUE;
	}else{
		JS_ReportError(cx, "setNamedItem method must get not null XML attribute object");
	}
	return JS_TRUE;
}

int mlXMLNamedNodeMap::FindNodeByName(const JSString* ajssName){
	std::vector<JSObject*>::iterator vi;
	int iIdx=0;
	for(vi=mvNodeMap.begin(); vi != mvNodeMap.end(); vi++ ){
		JSObject* obj=*vi;
		mlIXMLNode* pNode=(mlIXMLNode*)JS_GetPrivate(mcx,obj);
		if(isEqual(pNode->nodeName,ajssName)) return iIdx;
		iIdx++;
	}
	return -1;
}

JSString* mlXMLNamedNodeMap::getID(){
	std::vector<JSObject*>::iterator vi;
	for(vi=mvNodeMap.begin(); vi != mvNodeMap.end(); vi++ ){
		JSObject* obj=*vi;
		mlIXMLNode* pNode=(mlIXMLNode*)JS_GetPrivate(mcx,obj);
		jschar* jscName=JS_GetStringChars(pNode->nodeName);
		if(jscName[0]==L'i' || jscName[0]==L'I'){
			if(jscName[1]==L'd' || jscName[1]==L'D'){
				if(jscName[2]==L'\0'){
					return pNode->nodeValue;
				}
			}
		}
	}
	return NULL;
}

JSObject* mlXMLNamedNodeMap::setNamedItem(JSObject* ajsoNode){
	if(ajsoNode==NULL) return NULL;
	mlIXMLNode* pNewNode=(mlIXMLNode*)JS_GetPrivate(mcx,ajsoNode);
	JSString* jssNewNodeName=pNewNode->nodeName;
	if(jssNewNodeName==NULL) return NULL;
	int iIdx=FindNodeByName(jssNewNodeName);
	if(iIdx>=0){
		JSObject* jsoOldNode=mvNodeMap[iIdx];
		FREE_FOR_GC(mcx,mjso,jsoOldNode);
		mvNodeMap[iIdx]=ajsoNode;
	}else{
		mvNodeMap.push_back(ajsoNode);
	}
	SAVE_FROM_GC(mcx,mjso,ajsoNode);
	return ajsoNode;
}

JSBool mlXMLNamedNodeMap::JSFUNC_removeNamedItem(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlXMLNamedNodeMap* pNNM=(mlXMLNamedNodeMap*)JS_GetPrivate(cx, obj);
	*rval=JSVAL_NULL;
	if (argc != 1){
		JS_ReportError(cx, "removeNamedItem method must get a string argument");
		return JS_TRUE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "removeNamedItem method must get a string argument");
		return JS_TRUE;
	}
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoOldNode=pNNM->removeNamedItem(jssName);
	if(jsoOldNode!=NULL)
		*rval=OBJECT_TO_JSVAL(jsoOldNode);
	return JS_TRUE;
}

JSObject* mlXMLNamedNodeMap::removeNamedItem(const JSString* ajssName){
	if(ajssName==NULL) return NULL;
	int iIdx=FindNodeByName(ajssName);
	if(iIdx>=0){
		JSObject* jsoOldNode=mvNodeMap[iIdx];
		FREE_FOR_GC(mcx,mjso,jsoOldNode);
		mvNodeMap.erase(mvNodeMap.begin()+iIdx);
		return jsoOldNode;
	}
	return NULL;
}

JSObject* mlXMLNamedNodeMap::removeItem(JSObject* ajsoNode){
	if(ajsoNode==NULL) return NULL;
	std::vector<JSObject*>::iterator vi;
	for(vi=mvNodeMap.begin(); vi != mvNodeMap.end(); vi++ ){
		if(*vi==ajsoNode){
			FREE_FOR_GC(mcx,mjso,ajsoNode);
			mvNodeMap.erase(vi);
			return ajsoNode;
		}
	}
	return NULL;
}

}
