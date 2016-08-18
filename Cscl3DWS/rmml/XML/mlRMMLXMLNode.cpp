
#include "mlRMML.h"

namespace rmml {

mlIXMLNode::mlIXMLNode(){
	nodeName=NULL;
	nodeValue=NULL;
	parentNode=NULL;
	childNodes=NULL;
	attributes=NULL;
	ns = NULL;
}

mlIXMLNode::~mlIXMLNode(){
}

///// JavaScript Variable Table
JSPropertySpec mlIXMLNode::_JSPropertySpec[] = {
	JSPROP_RO(nodeName)
	JSPROP_RW(nodeValue)
	JSPROP_RO(nodeType)
	JSPROP_RO(parentNode)
	JSPROP_RO(childNodes)
	JSPROP_RO(firstChild)
	JSPROP_RO(lastChild)
	JSPROP_RO(previousSibling)
	JSPROP_RO(nextSibling)
	JSPROP_RO(attributes)
	JSPROP_RO(ownerDocument)
	JSPROP_RO(textContent)
	{ 0, 0, 0, 0, 0 }
};

/*
interface Node {

  readonly attribute  DOMString            nodeName;
           attribute  DOMString            nodeValue;
                                                 // raises(DOMException) on setting
                                                 // raises(DOMException) on retrieval
  readonly attribute  unsigned short       nodeType;
  readonly attribute  Node                 parentNode;
  readonly attribute  NodeList             childNodes;
  readonly attribute  Node                 firstChild;
  readonly attribute  Node                 lastChild;
  readonly attribute  Node                 previousSibling;
  readonly attribute  Node                 nextSibling;
  readonly attribute  NamedNodeMap         attributes;
  readonly attribute  Document             ownerDocument;
  Node                      insertBefore(in Node newChild, 
                                         in Node refChild)
                                         raises(DOMException);
  Node                      replaceChild(in Node newChild, 
                                         in Node oldChild)
                                         raises(DOMException);
  Node                      removeChild(in Node oldChild)
                                        raises(DOMException);
  Node                      appendChild(in Node newChild)
                                        raises(DOMException);
  boolean                   hasChildNodes();
  Node                      cloneNode(in boolean deep);
*/

///// JavaScript Function Table
JSFunctionSpec mlIXMLNode::_JSFunctionSpec[] = {
	JSFUNC(insertBefore,2)
	JSFUNC(replaceChild,2)
	JSFUNC(removeChild,1)
	JSFUNC(appendChild,1)
	JSFUNC(hasChildNodes,0)
	JSFUNC(cloneNode,1)
	JSFUNC(select,1)	// выбирает узлы в соответствии с XPath-запросом
	{ 0, 0, 0, 0, 0 }
};

EventSpec mlIXMLNode::_EventSpec[] = {
	{0, 0, 0}
};

ADDPROPFUNCSPECS_IMPL(mlIXMLNode)

///// JavaScript Set Property Wrapper
JSBool mlIXMLNode::SetJSProperty(JSContext* cx, int id, jsval *vp) {
	switch(id) {
		case JSPROP_nodeValue:
			if(JSVAL_IS_STRING(*vp)){
				SetValue(JSVAL_TO_STRING(*vp));
			}
			int hh=0;
			// ??
			break;
	}
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlIXMLNode::GetJSProperty(int id, jsval *vp) {
	switch(id) {
		case JSPROP_nodeName:
			if(nodeName == NULL){ 
				if(nodeType == TEXT_NODE){
					nodeName = wr_JS_NewUCStringCopyZ(GetJSContext(), L"#text");
					SAVE_STR_FROM_GC(GetJSContext(), GetJSObject(), nodeName);
					*vp = STRING_TO_JSVAL(nodeName);
				}else
					*vp=JSVAL_NULL;
			}else *vp = STRING_TO_JSVAL(nodeName);
			break;
		case JSPROP_nodeValue:
			if(nodeValue==NULL) *vp=JSVAL_NULL;
			else *vp=STRING_TO_JSVAL(nodeValue);
			break;
		case JSPROP_nodeType:
			*vp=INT_TO_JSVAL(nodeType);
			break;
		case JSPROP_parentNode:
			if(parentNode==NULL) *vp=JSVAL_NULL;
			else *vp=OBJECT_TO_JSVAL(parentNode->GetJSObject());
			break;
		case JSPROP_childNodes:{
			*vp = JSVAL_NULL;
			if(childNodes!=NULL){
				*vp=OBJECT_TO_JSVAL(childNodes->mjso);
			}
			}break;
		case JSPROP_firstChild:{
			*vp = JSVAL_NULL;
			if(childNodes!=NULL){
				JSObject* jsoFirstChild=childNodes->GetFirst();
				if(jsoFirstChild!=NULL)
					*vp=OBJECT_TO_JSVAL(jsoFirstChild);
			}
			}break;
		case JSPROP_lastChild:
			*vp = JSVAL_NULL;
			if(childNodes!=NULL){
				JSObject* jsoLastChild=childNodes->GetLast();
				if(jsoLastChild!=NULL)
					*vp=OBJECT_TO_JSVAL(jsoLastChild);
			}
			break;
		case JSPROP_previousSibling:{
			*vp=JSVAL_NULL;
			if(parentNode!=NULL){
				JSObject* jsoPrevSibling=parentNode->GetPrevChild(GetJSObject());
				if(jsoPrevSibling!=NULL)
					*vp=OBJECT_TO_JSVAL(jsoPrevSibling);
			}
			}break;
		case JSPROP_nextSibling:{
			*vp=JSVAL_NULL;
			if(parentNode!=NULL){
				JSObject* jsoNextSibling=parentNode->GetNextChild(GetJSObject());
				if(jsoNextSibling!=NULL)
					*vp=OBJECT_TO_JSVAL(jsoNextSibling);
			}
			}break;
		case JSPROP_attributes:
			if(nodeType != ELEMENT_NODE && nodeType != DOCUMENT_NODE){
				*vp = JSVAL_NULL;
			}else{
				if(attributes == NULL)
					CreateAttributes();
				*vp = OBJECT_TO_JSVAL(attributes->mjso);
			}break;
		case JSPROP_ownerDocument:{
			JSObject* jsoDoc=GetDocument();
			if(jsoDoc==NULL) *vp=JSVAL_NULL;
			else *vp=OBJECT_TO_JSVAL(jsoDoc);
			}break;
		case JSPROP_textContent:{
			mlString sTextContent;
			if(!GetTextContent(sTextContent)){
				*vp=JSVAL_NULL;
			}else{
				JSString* jss = wr_JS_NewUCStringCopyZ(GetJSContext(), sTextContent.c_str());
				*vp=STRING_TO_JSVAL(jss);
			}
			}break;
	}
	return JS_TRUE;
}

///// JavaScript Function Wrappers
JSBool mlIXMLNode::JSFUNC_insertBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pIXMLNode=(mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 2) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[1])) return JS_FALSE;
	JSObject* jsoNewChild=JSVAL_TO_OBJECT(argv[0]);
	JSObject* jsoRefChild=JSVAL_TO_OBJECT(argv[1]);
	JSObject* jsoInserted=pIXMLNode->insertBefore(jsoNewChild,jsoRefChild);
	if(jsoInserted==NULL) *rval=JSVAL_NULL;
	else *rval=OBJECT_TO_JSVAL(jsoInserted);
	return JS_TRUE;
}

JSObject* mlIXMLNode::insertBefore(JSObject* ajsoNewChild, JSObject* ajsoRefChild){
	if(childNodes==NULL) return NULL;
	return childNodes->InsertBefore(ajsoNewChild,ajsoRefChild);
}

JSBool mlIXMLNode::JSFUNC_replaceChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pIXMLNode=(mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 2) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[1])) return JS_FALSE;
	JSObject* jsoNewChild=JSVAL_TO_OBJECT(argv[0]);
	JSObject* jsoOldChild=JSVAL_TO_OBJECT(argv[1]);
	JSObject* jsoReplaced=pIXMLNode->replaceChild(jsoNewChild,jsoOldChild);
	if(jsoReplaced==NULL) *rval=JSVAL_NULL;
	else *rval=OBJECT_TO_JSVAL(jsoReplaced);
	return JS_TRUE;
}

JSObject* mlIXMLNode::replaceChild(JSObject* ajsoNewChild, JSObject* ajsoOldChild){
	if(childNodes==NULL) return NULL;
	JSObject* jsoReplaced=childNodes->ReplaceChild(ajsoNewChild,ajsoOldChild);
	if(jsoReplaced==NULL) return NULL;
	return jsoReplaced;
}

JSBool mlIXMLNode::JSFUNC_removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pIXMLNode=(mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	JSObject* jsoNodeToDelete=JSVAL_TO_OBJECT(argv[0]);
	jsoNodeToDelete=pIXMLNode->removeChild(jsoNodeToDelete);
	if(jsoNodeToDelete==NULL) *rval=JSVAL_NULL;
	*rval=OBJECT_TO_JSVAL(jsoNodeToDelete);
	return JS_TRUE;
}

JSObject* mlIXMLNode::removeChild(JSObject* ajsoOldChild) {
	if(childNodes==NULL) return NULL;
	return childNodes->RemoveChild(ajsoOldChild);
}

JSBool mlIXMLNode::JSFUNC_appendChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pIXMLNode = (mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 1) return JS_FALSE;
	if(!JSVAL_IS_OBJECT(argv[0])) return JS_FALSE;
	JSObject* jsoNew=pIXMLNode->appendChild(JSVAL_TO_OBJECT(argv[0]));
	if(jsoNew==NULL) return JS_FALSE;
	*rval=OBJECT_TO_JSVAL(jsoNew);
	return JS_TRUE;
}

JSBool mlIXMLNode::JSFUNC_select(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pIXMLNode = (mlIXMLNode*)JS_GetPrivate(cx, obj);
	if(argc <= 0 || !JSVAL_IS_STRING(argv[0])){
		JS_ReportWarning(cx, "First argument of select-method must be a string");
		*rval = JSVAL_NULL;
		return JS_TRUE;
	}
	JSString* jssXPaxExpr = JSVAL_TO_STRING(argv[0]);
	mlNodeVect vNodes;
	bool bRet = pIXMLNode->select(wr_JS_GetStringChars(jssXPaxExpr), vNodes);
	int iLen = vNodes.size();
	jsval* pjsvNodes = MP_NEW_ARR( jsval, iLen + 1);
	mlNodeVect::const_iterator cvi;
	int iNum = 0;
	for(cvi = vNodes.begin(); cvi != vNodes.end(); cvi++, iNum++){
		mlIXMLNode* pNode = *cvi;
		pjsvNodes[iNum] = OBJECT_TO_JSVAL(pNode->GetJSObject());
	}
	JSObject* jsoResult = JS_NewArrayObject(cx, iLen, pjsvNodes);
	*rval = OBJECT_TO_JSVAL(jsoResult);
	return JS_TRUE;
}

void mlIXMLNode::CreateChildNodes(){
	if(childNodes!=NULL) return;
	JSContext* cx=GetJSContext();
	JSObject* jsoNodeList=mlXMLNodeList::newJSObject(cx);
	childNodes=(mlXMLNodeList*)JS_GetPrivate(cx,jsoNodeList);
	// защищаем от сборщика мусора
	JSObject* jso=GetJSObject();
	SAVE_FROM_GC(cx,jso,jsoNodeList);
}

#define GET_CX_JSO \
	JSContext* cx=GetJSContext(); \
	JSObject* jso=GetJSObject();

JSObject* mlIXMLNode::appendChild(JSObject* ajsoNewChild) {
	GET_CX_JSO;
	SAVE_FROM_GC(cx, jso, ajsoNewChild)
	CreateChildNodes();
	FREE_FOR_GC(cx, jso, ajsoNewChild)
	childNodes->AppendChild(ajsoNewChild);
	//
	mlIXMLNode* pNewChild=(mlIXMLNode*)JS_GetPrivate(cx,ajsoNewChild);
	pNewChild->parentNode=this;
	//
	return ajsoNewChild;
}

JSBool mlIXMLNode::JSFUNC_hasChildNodes(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pXMLNode=(mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 0) return JS_FALSE;
	if(pXMLNode->childNodes==NULL || pXMLNode->childNodes->mvNodeList.size()==0)
		*rval=BOOLEAN_TO_JSVAL(false);
	else
		*rval=BOOLEAN_TO_JSVAL(true);
	return JS_TRUE;
}

JSBool mlIXMLNode::JSFUNC_cloneNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	mlIXMLNode* pXMLNode=(mlIXMLNode*)JS_GetPrivate(cx, obj);
	if (argc != 1) return JS_FALSE;
	if(!JSVAL_IS_BOOLEAN(argv[0])) return JS_FALSE;
	bool bDeep=JSVAL_TO_BOOLEAN(argv[0]);
	JSObject* jsoCloned=pXMLNode->cloneNode(bDeep);
	if(jsoCloned==NULL) *rval=JSVAL_NULL;
	else *rval=OBJECT_TO_JSVAL(jsoCloned);
	return JS_TRUE;
}

__forceinline JSString* CopyJSString(JSContext* cx, JSString* ajss){
	if(ajss==NULL) return NULL;
	return JS_NewUCStringCopyN(cx,JS_GetStringChars(ajss),JS_GetStringLength(ajss));
}

JSObject* mlIXMLNode::CloneNode(JSObject* ajsoCloned){
	JSContext* cx=GetJSContext();
	mlIXMLNode* pCloned=(mlIXMLNode*)JS_GetPrivate(cx,ajsoCloned);
	if(nodeName!=NULL)
		pCloned->SetName(CopyJSString(cx,nodeName));
	if(nodeValue!=NULL)
		pCloned->SetValue(CopyJSString(cx,nodeValue));
	if(attributes!=NULL)
		pCloned->CreateAttributes();
	if(childNodes!=NULL)
		pCloned->CreateChildNodes();
	return ajsoCloned;
}

void mlIXMLNode::SetName(JSString* ajssName){
	nodeName=ajssName;
	GET_CX_JSO;
	SAVE_STR_FROM_GC(cx, jso, ajssName);
}

void mlIXMLNode::SetValue(JSString* ajssValue){
	GET_CX_JSO;
	if(nodeValue!=NULL)
		FREE_STR_FOR_GC(cx,jso,nodeValue);
	nodeValue=ajssValue;
	SAVE_STR_FROM_GC(cx,jso,nodeValue);
}

void mlIXMLNode::CreateAttributes(){
	if(attributes!=NULL) return;
	GET_CX_JSO;
	JSObject* jsoAttributes=mlXMLNamedNodeMap::newJSObject(cx);
	attributes=(mlXMLNamedNodeMap*)JS_GetPrivate(cx,jsoAttributes);
	SAVE_FROM_GC(cx,jso,jsoAttributes);
}

mlXMLNamedNodeMap* mlIXMLNode::GetAttributes(){
	CreateAttributes();
	return attributes;
}

JSObject* mlIXMLNode::GetDocument(){
	if(parentNode==NULL) return NULL;
	if(parentNode->nodeType==DOCUMENT_NODE) return parentNode->GetJSObject();
	return parentNode->GetDocument();
}

JSObject* mlIXMLNode::GetPrevChild(JSObject* ajsoChild){
	if(ajsoChild==NULL || childNodes==NULL) return NULL;
	return childNodes->GetPrev(ajsoChild);
}

JSObject* mlIXMLNode::GetNextChild(JSObject* ajsoChild){
	if(ajsoChild==NULL || childNodes==NULL) return NULL;
	return childNodes->GetNext(ajsoChild);
}

JSObject* mlIXMLNode::getElementsByTagName(JSString* ajssTagName, mlXMLNodeList* apNodeList){
	JSContext* cx=GetJSContext();
	if(apNodeList==NULL){
		JSObject* jsoNodeList=mlXMLNodeList::newJSObject(cx);
		apNodeList=(mlXMLNodeList*)JS_GetPrivate(cx,jsoNodeList);
	}
	if(childNodes==NULL || ajssTagName==NULL) return apNodeList->mjso;
	std::vector<JSObject*>::iterator vi;
	for(vi=childNodes->mvNodeList.begin(); vi != childNodes->mvNodeList.end(); vi++ ){
		JSObject* jsoChild=*vi;
		mlIXMLNode* pChildNode=(mlIXMLNode*)JS_GetPrivate(cx,jsoChild);
		if(pChildNode->nodeType==ELEMENT_NODE){
			if(isEqual(pChildNode->nodeName,ajssTagName)){
				apNodeList->AppendChild(jsoChild,false);
			}
			mlXMLElement* pChildElem=(mlXMLElement*)pChildNode;
			pChildElem->getElementsByTagName(ajssTagName,apNodeList);
		}
	}
	return apNodeList->mjso;
}

bool mlIXMLNode::GetTextContent(mlString &asTextContent){
	JSContext* cx=GetJSContext();
	if(childNodes==NULL){
		if(nodeValue != NULL){
			jschar* jscNodeValue = JS_GetStringChars(nodeValue);
			asTextContent += (const wchar_t*)jscNodeValue;
		}
		return !asTextContent.empty();
	}
	std::vector<JSObject*>::iterator vi;
	for(vi=childNodes->mvNodeList.begin(); vi != childNodes->mvNodeList.end(); vi++ ){
		JSObject* jsoChild=*vi;
		mlIXMLNode* pChildNode=(mlIXMLNode*)JS_GetPrivate(cx,jsoChild);
		pChildNode->GetTextContent(asTextContent);
	}
	return !asTextContent.empty();
}

JSObject* mlIXMLNode::GetFirstChild(){
	if(childNodes == NULL) return NULL;
	return childNodes->GetFirst();
}

JSObject* mlIXMLNode::GetLastChild(){
	if(childNodes == NULL) return NULL;
	return childNodes->GetLast();
}

mlIXMLNode* mlIXMLNode::GetFirstChildNode(){
	JSObject* jsoChild = GetFirstChild();
	if(jsoChild == NULL) return NULL;
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoChild);
}

mlIXMLNode* mlIXMLNode::GetLastChildNode(){
	JSObject* jsoChild = GetLastChild();
	if(jsoChild == NULL) return NULL;
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoChild);
}

const wchar_t* mlIXMLNode::GetContent(){
	if(nodeValue == NULL)
		return NULL;
	return (const wchar_t*)JS_GetStringChars(nodeValue);
}

mlIXMLNode* mlIXMLNode::GetNextSiblingNode(){
	if(parentNode == NULL || parentNode->childNodes == NULL)
		return NULL;
	JSObject* jsoNextSibling = parentNode->childNodes->GetNext(GetJSObject());
	if(jsoNextSibling == NULL) return NULL;
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoNextSibling);
}

mlIXMLNode* mlIXMLNode::GetPrevSiblingNode(){
	if(parentNode == NULL || parentNode->childNodes == NULL)
		return NULL;
	JSObject* jsoPrevSibling = parentNode->childNodes->GetPrev(GetJSObject());
	if(jsoPrevSibling == NULL) return NULL;
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoPrevSibling);
}

mlIXMLNode* mlIXMLNode::GetDocumentNode(){
	JSObject* jsoDocument = GetDocument();
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoDocument);
}

const wchar_t* mlIXMLNode::GetNodeName(){
	if(nodeName == NULL) return L"";
	return wr_JS_GetStringChars(nodeName);
}

mlIXMLNode* mlIXMLNode::GetFirstAttrNode(){
	if(attributes == NULL) return NULL;
	JSObject* jsoAttr = attributes->item(0);
	if(jsoAttr == NULL) return NULL;
	return (mlIXMLNode*)JS_GetPrivate(GetJSContext(), jsoAttr);
}

}