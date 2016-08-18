
#include "mlRMML.h"

namespace rmml {

mlXMLNodeList::mlXMLNodeList(void):
	MP_VECTOR_INIT(mvNodeList)
{
}

mlXMLNodeList::~mlXMLNodeList(void){
	mvNodeList.clear();
}

/***************************
 * реализация mlXMLNodeList
 */

/*
interface NodeList {
  Node                      item(in unsigned long index);
  readonly attribute  unsigned long        length;
};
*/

///// JavaScript Variable Table
JSPropertySpec mlXMLNodeList::_JSPropertySpec[] = {
	JSPROP_RO(length)
	{ 0, 0, 0, 0, 0 }
};

///// JavaScript Function Table
JSFunctionSpec mlXMLNodeList::_JSFunctionSpec[] = {
	JSFUNC(item,1)
	{ 0, 0, 0, 0, 0 }
};

MLJSCLASS_IMPL_BEGIN(__XMLNodeList_,mlXMLNodeList,0)
	MLJSCLASS_ADDPROPFUNC
MLJSCLASS_IMPL_END(mlXMLNodeList)

///// JavaScript Set Property Wrapper
JSBool mlXMLNodeList::JSSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	SET_PROPS_BEGIN(mlXMLNodeList);
		default:;
	SET_PROPS_END;
	return JS_TRUE;
}

///// JavaScript Get Property Wrapper
JSBool mlXMLNodeList::JSGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	GET_PROPS_BEGIN(mlXMLNodeList);
		default:
			switch(iID){
			case JSPROP_length:
				*vp=INT_TO_JSVAL(priv->mvNodeList.size());
				break;
			}
	GET_PROPS_END;
	return JS_TRUE;
}

JSBool mlXMLNodeList::JSFUNC_item(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	*rval = JSVAL_NULL;
	if (argc != 1 || (!JSVAL_IS_INT(argv[0]))){
		JS_ReportError(cx, "item method must get an integer argument");
		return JS_TRUE;
	}
	mlXMLNodeList* pNL=(mlXMLNodeList*)JS_GetPrivate(cx, obj);
	int iIdx=JSVAL_TO_INT(argv[0]);
	if(iIdx < (int)pNL->mvNodeList.size())
		*rval=OBJECT_TO_JSVAL(pNL->mvNodeList[iIdx]);
	return JS_TRUE;
}

//int mlXMLNodeList::

void mlXMLNodeList::AppendChild(JSObject* ajsoNewChild, bool abTestForExists){
	if(abTestForExists){
		// если уже есть, то удалить
		RemoveChild(ajsoNewChild);
	}
	SAVE_FROM_GC(mcx,mjso,ajsoNewChild);
	mvNodeList.push_back(ajsoNewChild);
}

JSObject* mlXMLNodeList::InsertBefore(JSObject* ajsoNewChild, JSObject* ajsoRefChild){
	std::vector<JSObject*>::iterator vi;
	for(vi=mvNodeList.begin(); vi != mvNodeList.end(); vi++ ){
		if(*vi==ajsoRefChild){
			mvNodeList.insert(vi,ajsoNewChild);
			return ajsoNewChild;
		}
	}
	return NULL;
}

JSObject* mlXMLNodeList::ReplaceChild(JSObject* ajsoNewChild, JSObject* ajsoOldChild){
	std::vector<JSObject*>::iterator vi;
	for(vi=mvNodeList.begin(); vi != mvNodeList.end(); vi++ ){
		if(*vi==ajsoOldChild){
			*vi=ajsoNewChild;
			FREE_FOR_GC(mcx,mjso,ajsoOldChild);
			return ajsoOldChild;
		}
	}
	return NULL;
}

JSObject* mlXMLNodeList::RemoveChild(JSObject* ajsoOldChild){
	std::vector<JSObject*>::iterator vi;
	for(vi=mvNodeList.begin(); vi != mvNodeList.end(); vi++ ){
		if(*vi==ajsoOldChild){
			mvNodeList.erase(vi);
			return ajsoOldChild;
		}
	}
	return NULL;
}

JSObject* mlXMLNodeList::GetPrev(JSObject* ajsoChild){
		std::vector<JSObject*>::iterator vi;
		for(vi=mvNodeList.begin(); vi != mvNodeList.end(); vi++ ){
			if(*vi==ajsoChild){
				if(vi==mvNodeList.begin()) return NULL;
				vi--;
				return *vi;
			}
		}
		return NULL;
}

JSObject* mlXMLNodeList::GetNext(JSObject* ajsoChild){
		std::vector<JSObject*>::iterator vi;
		for(vi=mvNodeList.begin(); vi != mvNodeList.end(); vi++ ){
			if(*vi==ajsoChild){
				vi++;
				if(vi==mvNodeList.end()) return NULL;
				return *vi;
			}
		}
		return NULL;
}

}
