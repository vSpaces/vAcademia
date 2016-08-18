
#include "mlRMML.h"

namespace rmml {

mlXMLDocument::mlXMLDocument(void)
{
	nodeType=DOCUMENT_NODE;
}

mlXMLDocument::~mlXMLDocument(void){
}

/***************************
 * реализация mlXMLDocument
 */

/*
interface Document : Node {
  readonly attribute  DocumentType         doctype;
  readonly attribute  DOMImplementation    implementation;
  readonly attribute  Element              documentElement;
  Element                   createElement(in DOMString tagName)
                                          raises(DOMException);
  DocumentFragment          createDocumentFragment();
  Text                      createTextNode(in DOMString data);
  Comment                   createComment(in DOMString data);
  CDATASection              createCDATASection(in DOMString data)
                                               raises(DOMException);
  ProcessingInstruction     createProcessingInstruction(in DOMString target, 
                                                        in DOMString data)
                                                        raises(DOMException);
  Attr                      createAttribute(in DOMString name)
                                            raises(DOMException);
  EntityReference           createEntityReference(in DOMString name)
                                                  raises(DOMException);
  NodeList                  getElementsByTagName(in DOMString tagname);
Level 2:
  Element					getElementById(in DOMString elementId);
};
selectSingleNode(in DOMString path)
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLDocument::_JSPropertySpec[] = {
//	JSPROP_RO(doctype)
//	JSPROP_RO(implementation)
//	JSPROP_RO(documentElement)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLDocument::_JSFunctionSpec[] = {
	JSFUNC(createElement,1)
	JSFUNC(createTextNode,1)
//	JSFUNC(createComment,1)
//	JSFUNC(createCDATASection,1)
	JSFUNC(createAttribute,1)
	JSFUNC(getElementsByTagName,1)
	JSFUNC(getElementById,1)
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlXMLDocument::_EventSpec[] = {
	{0, 0, 0}
};

//ADDPROPFUNCSPECS_IMPL(mlXMLDocument)

MLJSCLASS_IMPL_BEGIN(XMLDocument,mlXMLDocument,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLDocument)

///// JavaScript Set Property Wrapper
JSBool mlXMLDocument::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLDocument);
		SET_PROTO_PROP(mlIXMLNode);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLDocument::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlXMLDocument);
		GET_PROTO_PROP(mlIXMLNode);
		default:;
	GET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Function Wrappers
JSBool mlXMLDocument::JSFUNC_createElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "createElement method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssTagName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoXMLEl=pXMLDoc->createElement(jssTagName);
	if(jsoXMLEl==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jsoXMLEl);
	return JS_TRUE;
}

JSObject* mlXMLDocument::createElement(JSString* ajssTagName){
	JSObject* jsoXMLElement=mlXMLElement::newJSObject(mcx);
	if(jsoXMLElement==NULL) return NULL;
	mlXMLElement* pXMLElement=(mlXMLElement*)JS_GetPrivate(mcx,jsoXMLElement);
	pXMLElement->SetName(ajssTagName);
	return jsoXMLElement;
}

JSBool mlXMLDocument::JSFUNC_createTextNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "createTextNode method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssText=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoXMLText=pXMLDoc->createTextNode(jssText);
	if(jsoXMLText==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jsoXMLText);
	return JS_TRUE;
}

JSObject* mlXMLDocument::createTextNode(JSString* ajssText){
	JSObject* jsoXMLText=mlXMLText::newJSObject(mcx);
	if(jsoXMLText==NULL) return NULL;
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(mcx,jsoXMLText);
	pXMLText->SetValue(ajssText);
	return jsoXMLText;
}

JSBool mlXMLDocument::JSFUNC_createCDATASection(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "createCDATASection method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssText=JSVAL_TO_STRING(argv[0]);
	JSObject* jso=pXMLDoc->createCDATASection(jssText);
	if(jso==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSObject* mlXMLDocument::createCDATASection(JSString* ajssText){
	JSObject* jso=mlXMLCDATASection::newJSObject(mcx);
	if(jso==NULL) return NULL;
	mlXMLCDATASection* pXMLCDATASection=(mlXMLCDATASection*)JS_GetPrivate(mcx,jso);
	pXMLCDATASection->SetValue(ajssText);
	return jso;
}

JSBool mlXMLDocument::JSFUNC_createComment(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "createComment method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssText=JSVAL_TO_STRING(argv[0]);
	JSObject* jso=pXMLDoc->createComment(jssText);
	if(jso==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jso);
	return JS_TRUE;
}

JSObject* mlXMLDocument::createComment(JSString* ajssText){
	JSObject* jso=mlXMLComment::newJSObject(mcx);
	if(jso==NULL) return NULL;
	mlXMLComment* pXMLComment=(mlXMLComment*)JS_GetPrivate(mcx,jso);
	pXMLComment->SetValue(ajssText);
	return jso;
}



JSBool mlXMLDocument::JSFUNC_createAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "createAttribute method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoXMLEl=pXMLDoc->createAttribute(jssName);
	if(jsoXMLEl==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jsoXMLEl);
	return JS_TRUE;
}

JSObject* mlXMLDocument::createAttribute(JSString* ajssName){
	JSObject* jsoXMLAttr=mlXMLAttr::newJSObject(mcx);
	if(jsoXMLAttr==NULL) return NULL;
	mlXMLAttr* pXMLAttr=(mlXMLAttr*)JS_GetPrivate(mcx,jsoXMLAttr);
	pXMLAttr->SetName(ajssName);
	return jsoXMLAttr;
}

JSBool mlXMLDocument::JSFUNC_getElementsByTagName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "getElementsByTagName method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssTagName=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoNodeList=pXMLDoc->getElementsByTagName(jssTagName);
	*rval=OBJECT_TO_JSVAL(jsoNodeList);
	return JS_TRUE;
}

JSObject* mlXMLDocument::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLDocument::newJSObject(mcx);
	mlXMLDocument* pCloned=(mlXMLDocument*)JS_GetPrivate(mcx,jsoCloned);
	mlIXMLNode::CloneNode(jsoCloned);
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

void mlXMLDocument::UpdateIDMap(JSString* ajssID,JSObject* ajsoXMLElement){
	if(ajssID==NULL){
		// взять его из свойств ajsoXMLElement
		if(ajsoXMLElement!=NULL){
			mlXMLElement* pMLEl=(mlXMLElement*)JS_GetPrivate(mcx,ajsoXMLElement);
			ajssID=pMLEl->getID();
		}
	}
	if(ajssID==NULL) return;
	if(ajsoXMLElement==NULL){
		if(mpMapID2XMLEl.get()!=NULL){
			mpMapID2XMLEl->erase(ajssID);
		}
	}else{
		if(mpMapID2XMLEl.get()==NULL) 
			mpMapID2XMLEl=std::auto_ptr<mlMapJSS2JSO>(new mlMapJSS2JSO);
		mpMapID2XMLEl->insert(ajssID, ajsoXMLElement);
	}
}

JSBool mlXMLDocument::JSFUNC_getElementById(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "getElementById method must get a string argument");
		return JS_TRUE;
	}
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(cx, obj);
	JSString* jssID=JSVAL_TO_STRING(argv[0]);
	JSObject* jsoXMLElement=pXMLDoc->getElementById(jssID);
	if(jsoXMLElement!=NULL)
		*rval=OBJECT_TO_JSVAL(jsoXMLElement);
	return JS_TRUE;
}

JSObject* mlXMLDocument::getElementById(JSString* ajssID) {
	if(mpMapID2XMLEl.get()==NULL) return NULL;
	return mpMapID2XMLEl->find(ajssID);
}

mlXMLElement* mlXMLDocument::GetRootElement(){
	if(childNodes == NULL) return NULL;
	JSObject* jsoChild = childNodes->GetFirst();
	for(;;){
		if(jsoChild == NULL) return NULL;
		mlIXMLNode* pChild = (mlIXMLNode*)JS_GetPrivate(mcx, jsoChild);
		if(pChild == NULL) return NULL;
		if(pChild->nodeType == ELEMENT_NODE) 
			return (mlXMLElement*)pChild;
		jsoChild = childNodes->GetNext(jsoChild);
	}
}

}