#pragma once


namespace rmml {

/**
 * JS-класс для XML-документа RMML
 */

class mlXMLElement :	
					public mlIXMLNode,
					public mlJSClass
{
public:
	mlXMLElement(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLElement(void);
MLJSCLASS_DECL2(mlXMLElement)
IXMLNODE_IMPL
private:
	enum {
		JSPROP_tagName
	};
	JSFUNC_DECL(getAttribute)
	JSString* getAttribute(JSString* ajssName);
	JSFUNC_DECL(setAttribute)
	void setAttribute(JSString* ajssName, JSString* ajssValue);
	JSFUNC_DECL(removeAttribute)
	void removeAttribute(JSString* ajssName);
	JSFUNC_DECL(getAttributeNode)
	JSObject* getAttributeNode(JSString* ajssName);
	JSFUNC_DECL(setAttributeNode)
	JSObject* setAttributeNode(JSObject* ajsoAttr);
	JSFUNC_DECL(removeAttributeNode)
	JSObject* removeAttributeNode(JSObject* ajsoAttr);
	JSFUNC_DECL(getElementsByTagName)
//	JSFUNC_DECL(normalize)
	JSFUNC_DECL(findElement)
	JSObject* findElement(JSString* ajssTagName, JSString* ajssAttrName, JSString* ajssAttrValue);

public:
	JSString* getID();
};

}
