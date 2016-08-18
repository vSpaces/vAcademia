#pragma once


namespace rmml {

/**
 * JS-класс для XML-документа RMML
 */

class mlXMLNodeList :	
					public mlJSClass
{
friend class mlIXMLNode;
friend class mlXMLElement;
friend class mlXMLDocument;
	MP_VECTOR<JSObject*> mvNodeList;
//	mlIXMLNode* mpParent;
public:
	mlXMLNodeList(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLNodeList(void);
MLJSCLASS_DECL2(mlXMLNodeList);
private:
	enum {
		JSPROP_length
	};
	JSFUNC_DECL(item);

	void AppendChild(JSObject* ajsoNewChild,bool abTestForExists=true);
	JSObject* InsertBefore(JSObject* ajsoNewChild, JSObject* ajsoRefChild);
	JSObject* ReplaceChild(JSObject* ajsoNewChild, JSObject* ajsoOldChild);
	JSObject* RemoveChild(JSObject* ajsoOldChild);
	JSObject* GetFirst(){
		if(mvNodeList.size()==0) return NULL;
		return mvNodeList[0];
	}
	JSObject* GetLast(){
		if(mvNodeList.size()==0) return NULL;
		return mvNodeList[mvNodeList.size()-1];
	}
	JSObject* GetPrev(JSObject* ajsoChild);
	JSObject* GetNext(JSObject* ajsoChild);
};

}
