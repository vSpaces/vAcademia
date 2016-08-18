#pragma once


namespace rmml {

/**
 * JS-класс для XML-документа RMML
 */

class mlXMLText :	
					public mlIXMLNode,
					public mlJSClass
{
friend class mlXMLDocument;
friend class mlXMLCDATASection;
friend class mlXMLComment;
public:
	mlXMLText(void);
	void destroy() { MP_DELETE_THIS; }
	~mlXMLText(void);
MLJSCLASS_DECL2(mlXMLText)
IXMLNODE_IMPL
private:
	enum {
		JSPROP_data,
		JSPROP_length
	};
	JSFUNC_DECL(substringData)
	JSFUNC_DECL(appendData)
	JSFUNC_DECL(insertData)
	JSFUNC_DECL(deleteData)
	JSFUNC_DECL(replaceData)
	JSFUNC_DECL(splitText)
};

}
