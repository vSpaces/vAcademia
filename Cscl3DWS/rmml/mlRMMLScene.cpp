#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {
/*
mlRMMLScene::mlRMMLScene(){
	// mlRMMLElement
	mRMMLType=MLMT_SCENE;
}

mlRMMLScene::~mlRMMLScene()
{
}
*/
/*
JSClass mlRMMLScene::sDOMJSClass = {
  "DOM Class", JSCLASS_HAS_PRIVATE,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  nsnull, nsnull, nsnull, DOMJSClass_Construct,
  nsnull, DOMJSClass_HasInstance
};

JSFunctionSpec mlRMMLScene::sDOMJSClass_methods[] = {
  {"toString", DOMJSClass_toString, 0, 0, 0},
  {0, 0, 0, 0, 0}
};

mlresult mlRMMLScene::InitDOMJSClass(JSContext * cx, JSObject* obj)
{

	JSObject *proto = ::JS_InitClass(cx, obj, NULL, &sDOMJSClass, NULL, 0,
									NULL, sDOMJSClass_methods, NULL,
									NULL);
	NS_ASSERTION(proto, "Can't initialize DOM class proto.");
	if (!proto) {
		return RM_ERROR_UNEXPECTED;
	}
	return RM_OK;
}
*/

}