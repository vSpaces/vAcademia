#pragma once


namespace rmml {

/**
 * Базовый класс для XML-записей (XMLDocument, XMLNode, ...)
 * (DOM-интерфейс Node)
 */ 

#define TIDB_mlIXMLNode 0x10

#define DATN_nodeName L"nodeName"
#define DATN_nodeValue L"nodeValue"
#define DATN_nodeType L"nodeType"
#define DATN_parentNode L"parentNode"
#define DATN_childNodes L"childNodes"
#define DATN_firstChild L"firstChild"
#define DATN_lastChild L"lastChild"
#define DATN_previousSibling L"previousSibling"
#define DATN_nextSibling L"nextSibling"
#define DATN_attributes L"attributes"
#define DATN_ownerDocument L"ownerDocument"

class mlXMLNodeList;
class mlXMLNamedNodeMap;

class mlIXMLNode;
typedef std::vector<mlIXMLNode*> mlNodeVect;

struct mlNs;

class mlIXMLNode
{
friend class mlRMMLXML;
friend class mlXMLDocument;
friend class mlXMLText;
friend class mlXMLNamedNodeMap;
friend class mlXMLElement;
friend class mlXMLAttr;
public:
	mlIXMLNode();
	virtual ~mlIXMLNode();
protected:
MLJSPROTO_DECL
private:
	enum { JSPROP0_mlIXMLNode=TIDB_mlIXMLNode-1,
		JSPROP_nodeName,
		JSPROP_nodeValue,
		JSPROP_nodeType,
		JSPROP_parentNode,
		JSPROP_childNodes,
		JSPROP_firstChild,
		JSPROP_lastChild,
		JSPROP_previousSibling,
		JSPROP_nextSibling,
		JSPROP_attributes,
		JSPROP_ownerDocument,
		JSPROP_textContent,
		TIDE_mlIXMLNode
	};

	JSFUNC_DECL(insertBefore)
	JSObject* insertBefore(JSObject* ajsoNewChild, JSObject* ajsoRefChild);
	JSFUNC_DECL(replaceChild)
	JSObject* replaceChild(JSObject* ajsoNewChild, JSObject* ajsoOldChild);
	JSFUNC_DECL(removeChild)
	JSObject* removeChild(JSObject* ajsoOldChild);
	JSFUNC_DECL(appendChild)
	JSObject* appendChild(JSObject* ajsoNewChild);
	JSFUNC_DECL(hasChildNodes)
	JSFUNC_DECL(cloneNode)
	virtual JSObject* cloneNode(bool abDeep){ return NULL; }
	JSFUNC_DECL(select)
	bool select(const wchar_t* apwcXPathExpr, mlNodeVect& avNodes);

	void CreateChildNodes();
	void CreateAttributes();

public:
	enum NodeType{
		ELEMENT_NODE       = 1,
		ATTRIBUTE_NODE     = 2,
		TEXT_NODE          = 3,
		CDATA_SECTION_NODE = 4,
		ENTITY_REFERENCE_NODE = 5,
		ENTITY_NODE        = 6,
		PROCESSING_INSTRUCTION_NODE = 7,
		COMMENT_NODE       = 8,
		DOCUMENT_NODE      = 9,
		DOCUMENT_TYPE_NODE = 10,
		DOCUMENT_FRAGMENT_NODE = 11,
		NOTATION_NODE      = 12,
		DTD_NODE           = 14,
		ELEMENT_DECL	   = 15,
		ATTRIBUTE_DECL     = 16,
		ENTITY_DECL		   = 17,
		NAMESPACE_DECL	   = 18
	}nodeType;
	mlNs* ns;
	mlIXMLNode* GetParentNode(){ return parentNode; }
	JSObject* GetFirstChild();
	JSObject* GetLastChild();
	mlIXMLNode* GetFirstChildNode();
	mlIXMLNode* GetLastChildNode();
	const wchar_t* GetContent();
	mlIXMLNode* GetNextSiblingNode();
	mlIXMLNode* GetPrevSiblingNode();
	mlIXMLNode* GetDocumentNode();
	const wchar_t* GetNodeName();
	mlIXMLNode* GetFirstAttrNode();
protected:
	JSString* nodeName;
	JSString* nodeValue;
	mlIXMLNode* parentNode;
	mlXMLNodeList* childNodes;
	mlXMLNamedNodeMap* attributes;

	void SetName(JSString* ajssName);
	void SetValue(JSString* ajssValue);
	mlXMLNamedNodeMap* GetAttributes();
	JSObject* GetPrevChild(JSObject* ajsoChild);
	JSObject* GetNextChild(JSObject* ajsoChild);
	JSObject* GetDocument();
	JSObject* CloneNode(JSObject* ajsoCloned);
	JSObject* getElementsByTagName(JSString* ajssTagName, mlXMLNodeList* apNodeList=NULL);
	bool GetTextContent(mlString &asTextContent);
/*
	mlIXMLNode parentNode;
  readonly attribute  NodeList             childNodes;
	mlIXMLNode firstChild;
	mlIXMLNode lastChild;
	mlIXMLNode previousSibling;
	mlIXMLNode nextSibling;
  readonly attribute  NamedNodeMap         attributes;
  readonly attribute  Document             ownerDocument;
*/
/*
interface Node {
  // NodeType
  const unsigned short      ELEMENT_NODE       = 1;
  const unsigned short      ATTRIBUTE_NODE     = 2;
  const unsigned short      TEXT_NODE          = 3;
  const unsigned short      CDATA_SECTION_NODE = 4;
  const unsigned short      ENTITY_REFERENCE_NODE = 5;
  const unsigned short      ENTITY_NODE        = 6;
  const unsigned short      PROCESSING_INSTRUCTION_NODE = 7;
  const unsigned short      COMMENT_NODE       = 8;
  const unsigned short      DOCUMENT_NODE      = 9;
  const unsigned short      DOCUMENT_TYPE_NODE = 10;
  const unsigned short      DOCUMENT_FRAGMENT_NODE = 11;
  const unsigned short      NOTATION_NODE      = 12;

  readonly attribute  DOMString            nodeName;
           attribute  DOMString            nodeValue;
                                                 // raises(DOMException) on setting
                                                 // raises(DOMException) on retrieval
  readonly attribute  unsigned short       nodeType;
  readonly attribute  Node                 parentNode;
  readonly attribute  NodeList             childNodes;
  readonly attribute  Node                 firstChild;
  readonly attribute  Node                 lastChild;
  readonly attribute  Node                 previousSibling;
  readonly attribute  Node                 nextSibling;
  readonly attribute  NamedNodeMap         attributes;
  readonly attribute  Document             ownerDocument;
  Node                      insertBefore(in Node newChild, 
                                         in Node refChild)
                                         raises(DOMException);
  Node                      replaceChild(in Node newChild, 
                                         in Node oldChild)
                                         raises(DOMException);
  Node                      removeChild(in Node oldChild)
                                        raises(DOMException);
  Node                      appendChild(in Node newChild)
                                        raises(DOMException);
  boolean                   hasChildNodes();
  Node                      cloneNode(in boolean deep);
*/
	virtual JSContext* GetJSContext()=0;
	virtual JSObject* GetJSObject()=0;
};

#define IXMLNODE_IMPL \
	JSContext* GetJSContext(){ return mcx; } \
	JSObject* GetJSObject(){ return mjso; } \
	JSObject* cloneNode(bool abDeep);

}