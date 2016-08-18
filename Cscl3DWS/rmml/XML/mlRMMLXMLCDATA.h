#pragma once

namespace rmml {

/**
 * JS-класс для CDATA XML-документа RMML
 */

class mlXMLCDATASection :	
					public mlIXMLNode,
					public mlJSClass
{
friend class mlXMLDocument;
public:
	mlXMLCDATASection(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLCDATASection(void);
MLJSCLASS_DECL2(mlXMLCDATASection)
IXMLNODE_IMPL
private:
	enum {
		JSPROP_data,
		JSPROP_length
	};
};

}
