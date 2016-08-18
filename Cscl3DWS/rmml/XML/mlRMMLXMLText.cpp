
#include "mlRMML.h"

namespace rmml {

mlXMLText::mlXMLText(void)
{
	nodeType=TEXT_NODE;
	//nodeName = JS_NewUCStringCopyZ(GPSM(mcx)->GetJSContext(), L"#text");
}

mlXMLText::~mlXMLText(void){
}

/***************************
 * реализация mlXMLText
 */

/*
interface CharacterData : Node {
           attribute  DOMString            data;
                                 // raises(DOMException) on setting
                                 // raises(DOMException) on retrieval
  readonly attribute  unsigned long        length;
  DOMString                 substringData(in unsigned long offset, 
                                          in unsigned long count)
                                          raises(DOMException);
  void                      appendData(in DOMString arg)
                                       raises(DOMException);
  void                      insertData(in unsigned long offset, 
                                       in DOMString arg)
                                       raises(DOMException);
  void                      deleteData(in unsigned long offset, 
                                       in unsigned long count)
                                       raises(DOMException);
  void                      replaceData(in unsigned long offset, 
                                        in unsigned long count, 
                                        in DOMString arg)
                                        raises(DOMException);
};
interface Text : CharacterData {
  Text                      splitText(in unsigned long offset)
                                      raises(DOMException);
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLText::_JSPropertySpec[] = {
	JSPROP_RW(data)
	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLText::_JSFunctionSpec[] = {
	JSFUNC(substringData,2)
	JSFUNC(appendData,1)
	JSFUNC(insertData,2)
	JSFUNC(deleteData,2)
	JSFUNC(replaceData,3)
	JSFUNC(splitText,1)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(XMLText,mlXMLText,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLText)

///// JavaScript Set Property Wrapper
JSBool mlXMLText::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLText);
		SET_PROTO_PROP(mlIXMLNode);
		default:
			switch(iID){
			case JSPROP_data:
				if(JSVAL_IS_STRING(*vp)){
					priv->SetValue(JSVAL_TO_STRING(*vp));
				}
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLText::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	mlXMLText* prv=(mlXMLText*)JS_GetPrivate(cx, obj); \
	if(prv->nodeName == NULL)
		prv->nodeName = wr_JS_NewUCStringCopyZ(cx, L"#text");
	GET_PROPS_BEGIN(mlXMLText);
		GET_PROTO_PROP(mlIXMLNode);
		default:
			switch(iID){
			case JSPROP_data:
				if(priv->nodeValue==NULL) *vp=NULL;
				else *vp=STRING_TO_JSVAL(priv->nodeValue);
				break;
			case JSPROP_length:
				*vp=INT_TO_JSVAL(JS_GetStringLength(priv->nodeValue));
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}


JSBool mlXMLText::JSFUNC_substringData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 2 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1]))){
		JS_ReportError(cx, "substringData method must get two integer arguments");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	int iOffset=JSVAL_TO_INT(argv[0]);
	int iCount=JSVAL_TO_INT(argv[1]);
	JSString* jssData=pXMLText->nodeValue;
	if(jssData==NULL) return JS_TRUE;
	int iLen=JS_GetStringLength(jssData);
	if(iOffset >= iLen) return JS_TRUE;
	if((iOffset+iCount) >= iLen) iCount=iLen-iOffset;
	jschar* jscData=JS_GetStringChars(jssData);
	JSString* jssSubstr=JS_NewUCStringCopyN(cx,jscData+iOffset,iCount);
	*rval=STRING_TO_JSVAL(jssSubstr);
	return JS_TRUE;
}

JSBool mlXMLText::JSFUNC_appendData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_STRING(argv[0]))){
		JS_ReportError(cx, "appendData method must get a string argument");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	JSString* jssAdd=JSVAL_TO_STRING(argv[0]);
	JSString* jssData=pXMLText->nodeValue;
	JSString* jssNewData=JS_ConcatStrings(cx, jssData, jssAdd);
	pXMLText->SetValue(jssNewData);
	return JS_TRUE;
}

JSBool mlXMLText::JSFUNC_insertData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc != 2 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_STRING(argv[1]))){
		JS_ReportError(cx, "deleteData method must get a integer argument followed by a string argument");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	int iOffset=JSVAL_TO_INT(argv[0]);
	JSString* jssInsert=JSVAL_TO_STRING(argv[1]);
	JSString* jssData=pXMLText->nodeValue;
	if(jssData==NULL) return JS_TRUE;
	int iLen=JS_GetStringLength(jssData);
	if(iOffset > iLen) return JS_TRUE;
	JSString* jssLeft=JS_NewDependentString(cx,jssData,0,iOffset);
	JSString* jssRight=JS_NewDependentString(cx,jssData,iOffset,iLen-iOffset);
	JSString* jssLeftInsert=JS_ConcatStrings(cx, jssLeft, jssInsert);
	JSString* jssNewData=JS_ConcatStrings(cx, jssLeftInsert, jssRight);
	pXMLText->SetValue(jssNewData);
	return JS_TRUE;
}

JSBool mlXMLText::JSFUNC_deleteData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc != 2 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1]))){
		JS_ReportError(cx, "deleteData method must get two integer arguments");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	int iOffset=JSVAL_TO_INT(argv[0]);
	int iCount=JSVAL_TO_INT(argv[1]);
	JSString* jssData=pXMLText->nodeValue;
	if(jssData==NULL) return JS_TRUE;
	int iLen=JS_GetStringLength(jssData);
	if(iOffset >= iLen) return JS_TRUE;
	if((iOffset+iCount) >= iLen) iCount=iLen-iOffset;
	JSString* jssLeft=JS_NewDependentString(cx,jssData,0,iOffset);
	JSString* jssRight=JS_NewDependentString(cx,jssData,iOffset+iCount,iLen-(iOffset+iCount));
	JSString* jssNewData=JS_ConcatStrings(cx, jssLeft, jssRight);
	pXMLText->SetValue(jssNewData);
	return JS_TRUE;
}

JSBool mlXMLText::JSFUNC_replaceData(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	if (argc != 3 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1])) || (!JSVAL_IS_STRING(argv[2]))){
		JS_ReportError(cx, "replaceData method must get two integer arguments followed by a string argument");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	int iOffset=JSVAL_TO_INT(argv[0]);
	int iCount=JSVAL_TO_INT(argv[1]);
	JSString* jssNewSubStr=JSVAL_TO_STRING(argv[2]);
	JSString* jssData=pXMLText->nodeValue;
	if(jssData==NULL) return JS_TRUE;
	int iLen=JS_GetStringLength(jssData);
	if(iOffset >= iLen) return JS_TRUE;
	if((iOffset+iCount) >= iLen) iCount=iLen-iOffset;
	JSString* jssLeft=JS_NewDependentString(cx,jssData,0,iOffset);
	JSString* jssRight=JS_NewDependentString(cx,jssData,iOffset+iCount,iLen-(iOffset+iCount));
	JSString* jssLeftReplace=JS_ConcatStrings(cx, jssLeft, jssNewSubStr);
	JSString* jssNewData=JS_ConcatStrings(cx, jssLeftReplace, jssRight);
	pXMLText->SetValue(jssNewData);
	return JS_TRUE;
}

JSBool mlXMLText::JSFUNC_splitText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_INT(argv[0]))){
		JS_ReportError(cx, "splitText method must get an integer argument");
		return JS_TRUE;
	}
	mlXMLText* pXMLText=(mlXMLText*)JS_GetPrivate(cx, obj);
	int iOffset=JSVAL_TO_INT(argv[0]);
	JSString* jssData=pXMLText->nodeValue;
	if(jssData==NULL) return JS_FALSE;
	jschar* jscData=JS_GetStringChars(jssData);
	int iLen=JS_GetStringLength(jssData);
	JSString* jssLeft=JS_NewUCStringCopyN(cx,jscData,iOffset);
	JSString* jssRight=JS_NewUCStringCopyN(cx,jscData+iOffset,iLen-iOffset);
	pXMLText->SetValue(jssLeft);
	JSObject* jsoNewText=mlXMLText::newJSObject(cx);
	mlXMLText* pNewText2=(mlXMLText*)JS_GetPrivate(cx, jsoNewText);
	pNewText2->SetValue(jssRight);
	*rval=OBJECT_TO_JSVAL(jsoNewText);
	return JS_TRUE;
}

JSObject* mlXMLText::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLText::newJSObject(mcx);
	mlIXMLNode::CloneNode(jsoCloned);
	return jsoCloned;
}

}
