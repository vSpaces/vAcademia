#pragma once

namespace rmml {

/**
 * JS-класс для комментариев XML-документа RMML
 */

class mlXMLComment :	
					public mlIXMLNode,
					public mlJSClass
{
friend class mlXMLDocument;
public:
	mlXMLComment(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLComment(void);
MLJSCLASS_DECL2(mlXMLComment)
IXMLNODE_IMPL
private:
	enum {
		JSPROP_data,
		JSPROP_length
	};
};

}
