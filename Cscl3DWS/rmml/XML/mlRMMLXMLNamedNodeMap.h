#pragma once


namespace rmml {

/**
 * JS-класс ?? RMML
 */

class mlXMLNamedNodeMap :	
					public mlJSClass
{
friend class mlRMMLXML;
friend class mlXMLElement;
friend class mlIXMLNode;
	MP_VECTOR<JSObject*> mvNodeMap;
public:
	mlXMLNamedNodeMap(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLNamedNodeMap(void);
MLJSCLASS_DECL2(mlXMLNamedNodeMap)

private:
	enum {
		JSPROP_length
	};
	JSFUNC_DECL(item)
	JSObject* item(unsigned int auIndex);
	JSFUNC_DECL(getNamedItem)
	JSObject* getNamedItem(JSString* ajssName);
	JSFUNC_DECL(setNamedItem)
	JSObject* setNamedItem(JSObject* ajsoNode);
	JSFUNC_DECL(removeNamedItem)
	JSObject* removeNamedItem(const JSString* ajssName);

	int FindNodeByName(const JSString* ajssName);
	JSObject* removeItem(JSObject* ajsoNode);

	JSString* getID();
};

}
