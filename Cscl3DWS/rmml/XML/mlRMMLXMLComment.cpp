
#include "mlRMML.h"

namespace rmml {

mlXMLComment::mlXMLComment(void)
{
	nodeType=COMMENT_NODE;
}

mlXMLComment::~mlXMLComment(void){
}

/***************************
 * реализация mlXMLComment
 */

/*
interface CDATASection : Text {
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLComment::_JSPropertySpec[] = {
	JSPROP_RW(data)
	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLComment::_JSFunctionSpec[] = {
	{ "substringData", mlXMLText::JSFUNC_substringData, 2, 0, 0},
	{ "appendData", mlXMLText::JSFUNC_appendData, 1, 0, 0},
	{ "insertData", mlXMLText::JSFUNC_insertData, 2, 0, 0},
	{ "deleteData", mlXMLText::JSFUNC_deleteData, 2, 0, 0},
	{ "replaceData", mlXMLText::JSFUNC_replaceData, 3, 0, 0},
	{ "splitText", mlXMLText::JSFUNC_splitText, 1, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(XMLComment,mlXMLComment,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLComment)

///// JavaScript Set Property Wrapper
JSBool mlXMLComment::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return mlXMLText::JSSetProperty(cx, obj, id, vp);
}

///// JavaScript Get Property Wrapper
JSBool mlXMLComment::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return mlXMLText::JSGetProperty(cx, obj, id, vp);
}

JSObject* mlXMLComment::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLComment::newJSObject(mcx);
	mlIXMLNode::CloneNode(jsoCloned);
	return jsoCloned;
}

}
