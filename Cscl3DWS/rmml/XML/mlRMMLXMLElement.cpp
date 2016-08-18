
#include "mlRMML.h"

namespace rmml {

mlXMLElement::mlXMLElement(void)
{
	nodeType=ELEMENT_NODE;
}

mlXMLElement::~mlXMLElement(void){
}

/***************************
 * реализация mlXMLElement
 */

/*
interface Element : Node {
  readonly attribute  DOMString            tagName;
  DOMString         getAttribute(in DOMString name);
  void              setAttribute(in DOMString name, 
                                 in DOMString value)
                                 raises(DOMException);
  void              removeAttribute(in DOMString name)
                                    raises(DOMException);
  Attr              getAttributeNode(in DOMString name);
  Attr              setAttributeNode(in Attr newAttr)
                                     raises(DOMException);
  Attr              removeAttributeNode(in Attr oldAttr)
                                        raises(DOMException);
  NodeList          getElementsByTagName(in DOMString name);
  void              normalize();
  Element			findElement(String tagName, String attrName, String attrValue)
};
normalize() - если есть несколько идущих подряд текстовых child'ов, то объединяет их в один
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLElement::_JSPropertySpec[] = {
	JSPROP_RO(tagName)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLElement::_JSFunctionSpec[] = {
	JSFUNC(getAttribute,1)
	JSFUNC(setAttribute,2)
	JSFUNC(removeAttribute,1)
	JSFUNC(getAttributeNode,1)
	JSFUNC(setAttributeNode,1)
	JSFUNC(removeAttributeNode,1)
	JSFUNC(getElementsByTagName,1)
//	JSFUNC(normalize,0)
	JSFUNC(findElement,3)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(XMLElement,mlXMLElement,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLElement)

///// JavaScript Set Property Wrapper
JSBool mlXMLElement::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLElement);
		SET_PROTO_PROP(mlIXMLNode);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLElement::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlXMLElement);
		GET_PROTO_PROP(mlIXMLNode);
		default:			
			switch(iID){
			case JSPROP_tagName:
				*vp=STRING_TO_JSVAL(priv->nodeName);
				break;
			}

	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlXMLElement::JSFUNC_getAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "getAttribute method must get a string argument");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	JSString* jssAttrValue=pXMLElem->getAttribute(jssName);
	if(jssAttrValue==NULL)
		*rval=JS_GetEmptyStringValue(cx);
	else
		*rval=STRING_TO_JSVAL(jssAttrValue);
	return JS_TRUE;
}

JSString* mlXMLElement::getAttribute(JSString* ajssName){
	JSObject* jsoXMLAttr=getAttributeNode(ajssName);
	if(jsoXMLAttr==NULL) return NULL;
	mlXMLAttr* pXMLAttr=(mlXMLAttr*)JS_GetPrivate(mcx,jsoXMLAttr);
	ML_ASSERTION(mcx, pXMLAttr!=NULL,"mlXMLElement::getAttribute");
	return pXMLAttr->nodeValue;
}

JSString* mlXMLElement::getID(){
	mlXMLNamedNodeMap* pAttributes=GetAttributes();
	if(pAttributes==NULL) return NULL;
	return pAttributes->getID();
}

JSBool mlXMLElement::JSFUNC_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	if (argc != 2 || (!JSVAL_IS_STRING(argv[0])) || (!JSVAL_IS_STRING(argv[1]))){
		JS_ReportError(cx, "setAttribute method must get two string arguments");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	JSString* jssValue=JSVAL_TO_STRING(argv[1]);
	pXMLElem->setAttribute(jssName,jssValue);
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

void mlXMLElement::setAttribute(JSString* ajssName, JSString* ajssValue){
	JSObject* jsoXMLAttr=getAttributeNode(ajssName);
	if(jsoXMLAttr==NULL) return;
	mlXMLAttr* pXMLAttr=(mlXMLAttr*)JS_GetPrivate(mcx,jsoXMLAttr);
	ML_ASSERTION(mcx, pXMLAttr!=NULL,"mlXMLElement::getAttribute");
	// если это ID, 
	// то удалить его из mapID2XMLEl
	// UpdateIDMap(,NULL);
	// ??
	// и вставить с новым ID-ом
	// ??
	pXMLAttr->SetValue(ajssValue);
}

JSBool mlXMLElement::JSFUNC_removeAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "removeAttribute method must get a string argument");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	pXMLElem->removeAttribute(jssName);
	return JS_TRUE;
}

void mlXMLElement::removeAttribute(JSString* ajssName){
	mlXMLNamedNodeMap* pAttributes=GetAttributes();
	if(pAttributes==NULL) return;
	pAttributes->removeNamedItem(ajssName);
}

JSBool mlXMLElement::JSFUNC_getAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "getAttributeNode method must get a string argument");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoXMLAttr=pXMLElem->getAttributeNode(jssName);
	if(jsoXMLAttr==NULL) return JS_TRUE;
	*rval=OBJECT_TO_JSVAL(jsoXMLAttr);
	return JS_TRUE;
}

JSObject* mlXMLElement::getAttributeNode(JSString* ajssName){
	if(ajssName==NULL) return NULL;
	mlXMLNamedNodeMap* pAttributes=GetAttributes();
	if(pAttributes==NULL) return NULL;
	return pAttributes->getNamedItem(ajssName);
}

JSBool mlXMLElement::JSFUNC_setAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	if (argc != 1 || (!JSVAL_IS_REAL_OBJECT(argv[0]))){
		JS_ReportError(cx, "setAttributeNode method must get a XML attribute object");
		return JS_TRUE;
	}
	JSObject* jsoXMLAttr=JSVAL_TO_OBJECT(argv[0]);
	if(!mlXMLAttr::IsInstance(cx, jsoXMLAttr)){
		JS_ReportError(cx, "setAttributeNode method must get a XML attribute object");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	if(pXMLElem->setAttributeNode(jsoXMLAttr)==NULL){
		JS_ReportError(cx, "unknown internal error in getAttributeNode method implementation");
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSObject* mlXMLElement::setAttributeNode(JSObject* ajsoAttr){
	if(!mlXMLAttr::IsInstance(mcx, ajsoAttr)) return NULL;
	mlXMLNamedNodeMap* pAttributes=GetAttributes();
	if(pAttributes==NULL) return NULL;
	return pAttributes->setNamedItem(ajsoAttr);
}

JSBool mlXMLElement::JSFUNC_removeAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_FALSE;
	if (argc != 1 || (!JSVAL_IS_REAL_OBJECT(argv[0]))){
		JS_ReportError(cx, "removeAttributeNode method must get a XML attribute object");
		return JS_TRUE;
	}
	JSObject* jsoXMLAttr=JSVAL_TO_OBJECT(argv[0]);
	if(!mlXMLAttr::IsInstance(cx, jsoXMLAttr)){
		JS_ReportError(cx, "removeAttributeNode method must get a XML attribute object");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	if(pXMLElem->removeAttributeNode(jsoXMLAttr)==NULL){
		JS_ReportError(cx, "unknown internal error in removeAttributeNode method implementation");
		return JS_TRUE;
	}
	*rval = JSVAL_TRUE;
	return JS_TRUE;
}

JSObject* mlXMLElement::removeAttributeNode(JSObject* ajsoAttr){
	if(!mlXMLAttr::IsInstance(mcx, ajsoAttr)) return NULL;
	mlXMLNamedNodeMap* pAttributes=GetAttributes();
	if(pAttributes==NULL) return NULL;
	return pAttributes->removeItem(ajsoAttr);
}

JSBool mlXMLElement::JSFUNC_getElementsByTagName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "getElementsByTagName method must get a string argument");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssTagName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoNodeList=pXMLElem->getElementsByTagName(jssTagName);
	*rval=OBJECT_TO_JSVAL(jsoNodeList);
	return JS_TRUE;
}

JSObject* mlXMLElement::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLElement::newJSObject(mcx);
	mlIXMLNode::CloneNode(jsoCloned);
	mlXMLElement* pCloned=(mlXMLElement*)JS_GetPrivate(mcx,jsoCloned);
	// clone attributes
	if(attributes!=NULL){
		int iCount=attributes->mvNodeMap.size();
		for(int iIdx=0; iIdx<iCount; iIdx++){
			JSObject* jsoAttr=attributes->mvNodeMap[iIdx];
			mlXMLAttr* pAttr=(mlXMLAttr*)JS_GetPrivate(mcx,jsoAttr);
			pCloned->attributes->setNamedItem(pAttr->cloneNode(0));
		}
	}
	if(abDeep && childNodes!=NULL){
		// clone children
		int iCount=childNodes->mvNodeList.size();
		for(int iIdx=0; iIdx<iCount; iIdx++){
			JSObject* jsoChild=childNodes->mvNodeList[iIdx];
			mlIXMLNode* pChildNode=(mlIXMLNode*)JS_GetPrivate(mcx,jsoChild);
			pCloned->childNodes->AppendChild(pChildNode->cloneNode(true));
		}
	}
	return jsoCloned;
}

JSBool mlXMLElement::JSFUNC_findElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval=JSVAL_NULL;
	if (argc < 1 || argc > 3){
		JS_ReportError(cx, "findElement method must get one, two or three string arguments");
		return JS_TRUE;
	}
	if(!JSVAL_IS_STRING(argv[0])){
		JS_ReportError(cx, "findElement method must get string arguments");
		return JS_TRUE;
	}
	mlXMLElement* pXMLElem=(mlXMLElement*)JS_GetPrivate(cx, obj);
	JSString* jssTagName=JSVAL_TO_STRING(argv[0]);
	JSString* jssAttrName=NULL;
	JSString* jssAttrValue=NULL;
	if(argc >= 2){
		if(!JSVAL_IS_STRING(argv[1])){
			JS_ReportError(cx, "findElement method must get string arguments");
			return JS_TRUE;
		}
		jssAttrName=JSVAL_TO_STRING(argv[1]);
	}
	if(argc == 3){
		if(!JSVAL_IS_STRING(argv[2])){
			JS_ReportError(cx, "findElement method must get string arguments");
			return JS_TRUE;
		}
		jssAttrValue=JSVAL_TO_STRING(argv[2]);
	}
	JSObject* jsoElement=pXMLElem->findElement(jssTagName,jssAttrName,jssAttrValue);
	if(jsoElement!=NULL)
		*rval=OBJECT_TO_JSVAL(jsoElement);
	return JS_TRUE;
}

JSObject* mlXMLElement::findElement(JSString* ajssTagName, JSString* ajssAttrName, JSString* ajssAttrValue){
	if(isEqual(nodeName,ajssTagName)){
		if(ajssAttrName==NULL) return mjso;
		JSString* jssVal=getAttribute(ajssAttrName);
		if(jssVal!=NULL){
			if(ajssAttrValue==NULL) return mjso;
			if(isEqual(jssVal,ajssAttrValue)) return mjso;
		}
	}
	if(childNodes==NULL) return NULL;
	// ищем среди дочерних элементов 1-го уровня
	int iCount=childNodes->mvNodeList.size();
	for(int iIdx=0; iIdx<iCount; iIdx++){
		JSObject* jsoChild=childNodes->mvNodeList[iIdx];
		mlIXMLNode* pChildNode=(mlIXMLNode*)JS_GetPrivate(mcx,jsoChild);
		if(pChildNode->nodeType==ELEMENT_NODE){
			mlXMLElement* pElem=(mlXMLElement*)pChildNode;
			JSObject* jsoFoundElem=pElem->findElement(ajssTagName, ajssAttrName, ajssAttrValue);
			if(jsoFoundElem!=NULL) return jsoFoundElem;
		}
	}
	return NULL;
}

}
