
#include "mlRMML.h"

namespace rmml {

mlXMLCDATASection::mlXMLCDATASection(void)
{
	nodeType=CDATA_SECTION_NODE;
}

mlXMLCDATASection::~mlXMLCDATASection(void){
}

/***************************
 * реализация mlXMLCDATASection
 */

/*
interface CDATASection : Text {
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLCDATASection::_JSPropertySpec[] = {
	JSPROP_RW(data)
	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLCDATASection::_JSFunctionSpec[] = {
	{ "substringData", mlXMLText::JSFUNC_substringData, 2, 0, 0},
	{ "appendData", mlXMLText::JSFUNC_appendData, 1, 0, 0},
	{ "insertData", mlXMLText::JSFUNC_insertData, 2, 0, 0},
	{ "deleteData", mlXMLText::JSFUNC_deleteData, 2, 0, 0},
	{ "replaceData", mlXMLText::JSFUNC_replaceData, 3, 0, 0},
	{ "splitText", mlXMLText::JSFUNC_splitText, 1, 0, 0},
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(XMLCDATASection,mlXMLCDATASection,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLCDATASection)

///// JavaScript Set Property Wrapper
JSBool mlXMLCDATASection::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return mlXMLText::JSSetProperty(cx, obj, id, vp);
}

///// JavaScript Get Property Wrapper
JSBool mlXMLCDATASection::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return mlXMLText::JSGetProperty(cx, obj, id, vp);
}

JSObject* mlXMLCDATASection::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLCDATASection::newJSObject(mcx);
	mlIXMLNode::CloneNode(jsoCloned);
	return jsoCloned;
}

}
