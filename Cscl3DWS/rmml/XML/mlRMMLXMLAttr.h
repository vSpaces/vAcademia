#pragma once

namespace rmml {

/**
 * JS-класс для XML-документа RMML
 */

class mlXMLAttr :	
					public mlIXMLNode,
					public mlJSClass
{
	bool specified;
public:
	mlXMLAttr(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLAttr(void);
MLJSCLASS_DECL2(mlXMLAttr)
IXMLNODE_IMPL
private:
	enum {
		JSPROP_name,
		JSPROP_specified,
		JSPROP_value,
	};

};

}
