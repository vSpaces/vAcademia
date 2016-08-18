#include "mlRMML.h"
#include "config/oms_config.h"
#include "config/prolog.h"

namespace rmml {

JSFUNC_DECL(getVisibleAt)

///// JavaScript Function Table
JSFunctionSpec _JSAuthctionSpec[] = {
	JSFUNC(getVisibleAt,2) // получить видимый объект сцены в определенной точке 
	{ 0, 0, 0, 0, 0 }
};

void mlSceneManager::SetAuthScene(mlRMMLComposition* apAuthScene){
	mpAuthScene = apAuthScene;
	if(mpAuthScene == NULL)
		return;
	jsval v = OBJECT_TO_JSVAL(mpAuthScene->mjso);
	JS_DefineProperty(cx, mjsoPlayer, "_auth_", v, NULL, NULL, JSPROP_READONLY | JSPROP_PERMANENT);
	JS_DefineFunctions(cx, mpAuthScene->mjso, _JSAuthctionSpec);
	// ??
}

JSBool JSFUNC_getVisibleAt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval){
	if(argc < 2 || (!JSVAL_IS_INT(argv[0])) || (!JSVAL_IS_INT(argv[1]))){
		JS_ReportError(cx, "getVisibleAt function must get two integer arguments");
		return JS_TRUE;
	}
	*rval = JSVAL_NULL;
	int iX = JSVAL_TO_INT(argv[0]);
	int iY = JSVAL_TO_INT(argv[1]);
	mlPoint pnt; pnt.x = iX; pnt.y = iY;
	mlRMMLElement* pMLElVisible = GPSM(cx)->GetVisibleAt(pnt);
	if(pMLElVisible != NULL)
		*rval = OBJECT_TO_JSVAL(pMLElVisible->mjso);
	return JS_TRUE;
}

mlRMMLElement* mlSceneManager::GetSceneVisibleAt(const mlPoint aPnt, mlIVisible** appVisible){
	return mpScene ? mpScene->GetVisibleAt(aPnt, appVisible) : NULL;
}

mlRMMLElement* mlSceneManager::GetUIVisibleAt(const mlPoint aPnt, mlIVisible** appVisible){
	return mpUI ? mpUI->GetVisibleAt(aPnt, appVisible) : NULL;
}

mlRMMLElement* mlSceneManager::GetVisibleAt(const mlPoint aPnt)
{
	//mlRMMLElement*ElementPtr = GetSceneVisibleAt(aPnt);
	//return ElementPtr ? ElementPtr : GetUIVisibleAt(aPnt);
	mlIVisible* pUIVisible = NULL;
	mlRMMLElement* pElement = GetUIVisibleAt(aPnt, &pUIVisible);
	if (!pElement)
		return GetSceneVisibleAt(aPnt);

	if(pUIVisible == NULL && pElement != NULL)
		pUIVisible = pElement->GetIVisible();
	mlIVisible* pVisible = NULL;
	mlRMMLElement* pSceneElement = GetSceneVisibleAt(aPnt, &pVisible);
	if(pVisible == NULL && pSceneElement != NULL)
		pVisible = pSceneElement->GetIVisible();
	if (pSceneElement && pUIVisible != NULL && pVisible != NULL
		&& pVisible->GetAbsDepth() < pUIVisible->GetAbsDepth())
		pElement = pSceneElement;

	return pElement;
}

}