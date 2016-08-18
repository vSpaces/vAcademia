#pragma once


#include "mlMapJSS2JSO.h"

namespace rmml {

/**
 * JS-класс для XML-документа RMML
 */

class mlXMLDocument :	
					public mlIXMLNode,
					public mlJSClass
{
	std::auto_ptr<mlMapJSS2JSO> mpMapID2XMLEl;
public:
	mlXMLDocument(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLDocument(void);
MLJSCLASS_DECL2(mlXMLDocument)
IXMLNODE_IMPL

	JSFUNC_DECL(createElement)
	JSObject* createElement(JSString* ajssTagName);
//	JSFUNC_DECL(createDocumentFragment)
	JSFUNC_DECL(createTextNode)
	JSObject* createTextNode(JSString* ajssText);
	JSFUNC_DECL(createComment)
	JSObject* createComment(JSString* ajssText);
	JSFUNC_DECL(createCDATASection)
	JSObject* createCDATASection(JSString* ajssText);
//	JSFUNC_DECL(createProcessingInstruction)
	JSFUNC_DECL(createAttribute)
	JSObject* createAttribute(JSString* ajssName);
//	JSFUNC_DECL(createEntityReference)
	JSFUNC_DECL(getElementsByTagName)
	JSFUNC_DECL(getElementById)
	JSObject* getElementById(JSString* ajssID);

	void UpdateIDMap(JSString* ajssID,JSObject* ajsoXMLElement);
	mlXMLElement* GetRootElement();
};

}
