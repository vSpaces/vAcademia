
#include "mlRMML.h"

namespace rmml {

mlXMLAttr::mlXMLAttr(void)
{
	nodeType=ATTRIBUTE_NODE;
	specified=true;
}

mlXMLAttr::~mlXMLAttr(void){
}

/***************************
 * реализация mlXMLAttr
 */

/*
interface Attr : Node {
  readonly attribute  DOMString            name;
  readonly attribute  boolean              specified;
           attribute  DOMString            value;
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLAttr::_JSPropertySpec[] = {
	JSPROP_RO(name)
	JSPROP_RO(specified)
	JSPROP_RW(value)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLAttr::_JSFunctionSpec[] = {
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(__XMLAttr_,mlXMLAttr,2)
	MLJSCLASS_ADDPROTO(mlIXMLNode)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLAttr)

///// JavaScript Set Property Wrapper
JSBool mlXMLAttr::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLAttr);
		SET_PROTO_PROP(mlIXMLNode);
		default:
			switch(iID){
			case JSPROP_value:
				if(JSVAL_IS_STRING(*vp)){
					priv->SetValue(JSVAL_TO_STRING(*vp));
				}
				break;
			}
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLAttr::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlXMLAttr);
		GET_PROTO_PROP(mlIXMLNode);
		default:
			switch(iID){
			case JSPROP_name:
				if(priv->nodeValue==NULL) *vp=NULL;
				else *vp=STRING_TO_JSVAL(priv->nodeName);
				break;
			case JSPROP_specified:
				*vp=BOOLEAN_TO_JSVAL(JS_TRUE);
				break;
			case JSPROP_value:
				if(priv->nodeValue==NULL) *vp=NULL;
				else *vp=STRING_TO_JSVAL(priv->nodeValue);
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSObject* mlXMLAttr::cloneNode(bool abDeep){
	JSObject* jsoCloned=mlXMLAttr::newJSObject(mcx);
	mlIXMLNode::CloneNode(jsoCloned);
	return jsoCloned;
}

}