
#include "mlRMML.h"

#include <float.h>
#include "mlDict.h"
#include "mlHashTable.h"

namespace rmml {

//#define DEBUG_STEP
#undef LIBXML_XPTR_ENABLED


#define XML_MAX_NAMELEN 100

double xmlXPathNAN = 0;
double xmlXPathPINF = 1;
double xmlXPathNINF = -1;
double xmlXPathNZERO = 0;

typedef enum {
    XPATH_EXPRESSION_OK = 0,
    XPATH_NUMBER_ERROR,
    XPATH_UNFINISHED_LITERAL_ERROR,
    XPATH_START_LITERAL_ERROR,
    XPATH_VARIABLE_REF_ERROR,
    XPATH_UNDEF_VARIABLE_ERROR,
    XPATH_INVALID_PREDICATE_ERROR,
    XPATH_EXPR_ERROR,
    XPATH_UNCLOSED_ERROR,
    XPATH_UNKNOWN_FUNC_ERROR,
    XPATH_INVALID_OPERAND,
    XPATH_INVALID_TYPE,
    XPATH_INVALID_ARITY,
    XPATH_INVALID_CTXT_SIZE,
    XPATH_INVALID_CTXT_POSITION,
    XPATH_MEMORY_ERROR,
    XPTR_SYNTAX_ERROR,
    XPTR_RESOURCE_ERROR,
    XPTR_SUB_RESOURCE_ERROR,
    XPATH_UNDEF_PREFIX_ERROR,
    XPATH_ENCODING_ERROR,
    XPATH_INVALID_CHAR_ERROR,
    XPATH_INVALID_CTXT
} mlXPathError;

void *mlGenericErrorContext = NULL;

#undef xmlGenericErrorContext
#define xmlGenericErrorContext mlGenericErrorContext

void mlGenericError (void *ctx, const char *msg,...){
	// ??
}

#undef xmlGenericError
#define xmlGenericError mlGenericError

#undef xmlXPathObjectPtr
#define xmlXPathObjectPtr mlXPathObjectPtr

#undef xmlFree
//#define xmlFree delete
#define xmlFree(x) MP_DELETE_ARR(x)

#undef xmlStrlen
#define xmlStrlen wcslen

#undef XML_ATTRIBUTE_NODE
#define XML_ATTRIBUTE_NODE mlIXMLNode::ATTRIBUTE_NODE
#undef XML_DOCUMENT_NODE
#define XML_DOCUMENT_NODE mlIXMLNode::DOCUMENT_NODE
#undef XML_ELEMENT_NODE
#define XML_ELEMENT_NODE mlIXMLNode::ELEMENT_NODE
#undef XML_TEXT_NODE
#define XML_TEXT_NODE mlIXMLNode::TEXT_NODE
#undef XML_CDATA_SECTION_NODE
#define XML_CDATA_SECTION_NODE mlIXMLNode::CDATA_SECTION_NODE
#undef XML_PI_NODE
#define XML_PI_NODE mlIXMLNode::PROCESSING_INSTRUCTION_NODE
#undef XML_COMMENT_NODE
#define XML_COMMENT_NODE mlIXMLNode::COMMENT_NODE
#undef XML_NAMESPACE_DECL
#define XML_NAMESPACE_DECL mlIXMLNode::NAMESPACE_DECL
#undef XML_DTD_NODE
#define XML_DTD_NODE mlIXMLNode::DTD_NODE
#undef XML_ENTITY_DECL
#define XML_ENTITY_DECL mlIXMLNode::ENTITY_DECL

typedef mlIXMLNode::NodeType mlNsType;

struct mlNs {
    struct mlNs  *next;	// next Ns link for this node
    mlNsType      type;	// global or local
    const xmlChar *href;	// URL for the namespace
    const xmlChar *prefix;	// prefix for the namespace
    void           *_private;   // application data
	mlNs(mlNsType atype = XML_NAMESPACE_DECL){
		next = NULL;
		type = atype;
		href = NULL;
		prefix = NULL;
		_private = NULL;
	}
};
typedef mlNs *mlNsPtr;

typedef enum {
    AXIS_ANCESTOR = 1,
    AXIS_ANCESTOR_OR_SELF,
    AXIS_ATTRIBUTE,
    AXIS_CHILD,
    AXIS_DESCENDANT,
    AXIS_DESCENDANT_OR_SELF,
    AXIS_FOLLOWING,
    AXIS_FOLLOWING_SIBLING,
    AXIS_NAMESPACE,
    AXIS_PARENT,
    AXIS_PRECEDING,
    AXIS_PRECEDING_SIBLING,
    AXIS_SELF
} mlXPathAxisVal;

typedef enum {
    NODE_TEST_NONE = 0,
    NODE_TEST_TYPE = 1,
    NODE_TEST_PI = 2,
    NODE_TEST_ALL = 3,
    NODE_TEST_NS = 4,
    NODE_TEST_NAME = 5
} mlXPathTestVal;

typedef enum {
    NODE_TYPE_NODE = 0,
    NODE_TYPE_COMMENT = XML_COMMENT_NODE,
    NODE_TYPE_TEXT = XML_TEXT_NODE,
    NODE_TYPE_PI = XML_PI_NODE
} mlXPathTypeVal;

struct mlXPathObject;
typedef mlXPathObject* mlXPathObjectPtr;

typedef int (*mlXPathConvertFunc) (mlXPathObjectPtr obj, int type);

struct mlXPathType {
    const wchar_t* name;		// the type name
    mlXPathConvertFunc func;		// the conversion function
};
typedef mlXPathType *mlXPathTypePtr;

struct mlXPathParserContext;

typedef mlXPathObjectPtr (*mlXPathAxisFunc) (mlXPathParserContext* ctxt,
				 mlXPathObjectPtr cur);

struct mlXPathAxis {
    const wchar_t* name;		// the axis name
    mlXPathAxisFunc func;		// the search function
};

typedef mlXPathAxis* mlXPathAxisPtr;

typedef void (*mlXPathFunction) (mlXPathParserContext* ctxt, int nargs);

struct mlError {
    int		domain;	// What part of the library raised this error 
    int		code;	// The error code, e.g. an xmlParserError 
    wchar_t *message;// human-readable informative error message 
//    mlErrorLevel level;// how consequent is the error
    char       *file;	// the filename
    int		line;	// the line number if available
    char       *str1;	// extra string information
    char       *str2;	// extra string information
    char       *str3;	// extra string information
    int		int1;	// extra number information
    int		int2;	// column number of the error or 0 if N/A (todo: rename this field when we would break ABI)
    void       *ctxt;   // the parser context if available
    void       *node;   // the node in the tree
};

typedef void (*mlStructuredErrorFunc) (void *userData, mlError* error);
typedef mlXPathObjectPtr (*mlXPathVariableLookupFunc) (void *ctxt,
                                         const xmlChar *name,
                                         const xmlChar *ns_uri);

struct mlXPathContext {
    mlXMLDocument* doc;			// The current document 
    mlIXMLNode* node;			// The current node 
//
//    int nb_variables_unused;		// unused (hash table) 
//    int max_variables_unused;		// unused (hash table) 
    mlHashTablePtr varHash;		// Hash table of defined variables 

    int nb_types;			// number of defined types 
    int max_types;			// max number of types 
    mlXPathTypePtr types;		// Array of defined types 

//    int nb_funcs_unused;		// unused (hash table) 
//    int max_funcs_unused;		// unused (hash table) 
    mlHashTablePtr funcHash;		// Hash table of defined funcs 
//
    int nb_axis;			// number of defined axis 
    int max_axis;			// max number of axis 
    mlXPathAxis* axis;		// Array of defined axis 
//
//    // the namespace nodes of the context node 
    mlNsPtr *namespaces;		// Array of namespaces 
    int nsNr;				// number of namespace in scope 
    void *user;				// function to free 

    // extra variables 
    int contextSize;			// the context size 
    int proximityPosition;		// the proximity position 
//
//    // extra stuff for XPointer 
//    int xptr;				// it this an XPointer context 
//    xmlNodePtr here;			// for here() 
//    xmlNodePtr origin;			// for origin() 
//
    // the set of namespace declarations in scope for the expression 
    mlHashTable* nsHash;		// The namespaces hash table 
    mlXPathVariableLookupFunc varLookupFunc;// variable lookup func 
    void *varLookupData;		// variable lookup data 
//
//    // Possibility to link in an extra item 
//    void *extra;                        // needed for XSLT 
//
//    // The function name and URI when calling a function 
//    const wchar_t *function;
//    const wchar_t *functionURI;
//
//    // function lookup function and data 
//    xmlXPathFuncLookupFunc funcLookupFunc;// function lookup func 
//    void *funcLookupData;		// function lookup data 
//
//    // temporary namespace lists kept for walking the namespace axis 
    mlNsPtr *tmpNsList;		// Array of namespaces 
//    int tmpNsNr;			// number of namespace in scope 
//
//    // error reporting mechanism 
    void *userData;                     // user specific data block 
    mlStructuredErrorFunc error;       // the callback in case of errors 
    mlError lastError;			// the last error 
//    xmlNodePtr debugNode;		// the source node XSLT 
//
//    // dictionnary 
    mlDict* dict;			// dictionnary if any 
//
//    int flags;				// flags to control compilation 
//    // Cache for reusal of XPath objects 
//    void *cache;
	mlXPathContext(mlXMLDocument* pXMLDoc){
		doc = pXMLDoc;
		node = NULL;
		dict = NULL;

		varHash = NULL;

		nb_types = 0;
		max_types = 0;
		types = NULL;

		MP_NEW_V( funcHash, mlHashTable, 0); // xmlHashCreate(0);

		nb_axis = 0;
		max_axis = 0;
		axis = NULL;

		namespaces = NULL;
		nsNr = 0;

		nsHash = NULL;
		varLookupFunc = NULL;
		varLookupData = NULL;

		user = NULL;

		tmpNsList = NULL;

		contextSize = -1;
		proximityPosition = -1;

#ifdef XP_DEFAULT_CACHE_ON
//    if (xmlXPathContextSetCache(ret, 1, -1, 0) == -1) {
//	xmlXPathFreeContext(ret);
//	return(NULL);
//    }
#endif
		RegisterAllFunctions();   
	}
	~mlXPathContext(){
		 MP_DELETE( funcHash);
	}
	void RegisterAllFunctions();
	int RegisterFuncNS(const wchar_t *name, const wchar_t *ns_uri, mlXPathFunction f);
	const xmlChar* NsLookup(const xmlChar *prefix);
	mlXPathObjectPtr VariableLookupNS(const xmlChar *name, const xmlChar *ns_uri);
};

#define STRANGE assert(0);

void mlXPathErrMemory(mlXPathContext* ctxt, const char *extra);

#undef xmlXPathErrMemory
#define xmlXPathErrMemory mlXPathErrMemory

void mlXPathErrMemory(mlXPathContext* ctxt, const char *extra)
{
    if (ctxt != NULL) {
        if (extra) {
            wchar_t buf[1024];

            swprintf_s(buf, 1024, L"Memory allocation failed : %s\n", extra);
            ctxt->lastError.message = (wchar_t *) Strdup(buf);
        } else {
            ctxt->lastError.message = (wchar_t *)Strdup(L"Memory allocation failed\n");
        }
        ctxt->lastError.domain = XML_FROM_XPATH;
        ctxt->lastError.code = XML_ERR_NO_MEMORY;
	if (ctxt->error != NULL)
	    ctxt->error(ctxt->userData, &ctxt->lastError);
    } else {
//        if (extra)
//            __xmlRaiseError(NULL, NULL, NULL,
//                            NULL, NULL, XML_FROM_XPATH,
//                            XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0,
//                            extra, NULL, NULL, 0, 0,
//                            "Memory allocation failed : %s\n", extra);
//        else
//            __xmlRaiseError(NULL, NULL, NULL,
//                            NULL, NULL, XML_FROM_XPATH,
//                            XML_ERR_NO_MEMORY, XML_ERR_FATAL, NULL, 0,
//                            NULL, NULL, NULL, 0, 0,
//                            "Memory allocation failed\n");
    }
}

typedef enum {
    XPATH_OP_END=0,
    XPATH_OP_AND,
    XPATH_OP_OR,
    XPATH_OP_EQUAL,
    XPATH_OP_CMP,
    XPATH_OP_PLUS,
    XPATH_OP_MULT,
    XPATH_OP_UNION,
    XPATH_OP_ROOT,
    XPATH_OP_NODE,
    XPATH_OP_RESET,
    XPATH_OP_COLLECT,
    XPATH_OP_VALUE,
    XPATH_OP_VARIABLE,
    XPATH_OP_FUNCTION,
    XPATH_OP_ARG,
    XPATH_OP_PREDICATE,
    XPATH_OP_FILTER,
    XPATH_OP_SORT
#ifdef LIBXML_XPTR_ENABLED
    ,XPATH_OP_RANGETO
#endif
} mlXPathOp;

struct mlXPathStepOp {
    mlXPathOp op;		// The identifier of the operation
    int ch1;			// First child
    int ch2;			// Second child
    int value;
    int value2;
    int value3;
    void *value4;
    void *value5;
    void *cache;
    void *cacheURI;
	mlXPathStepOp(){
		op = XPATH_OP_END;
		ch1 = 0;
		ch2 = 0;
		value = 0;
		value2 = 0;
		value3 = 0;
		value4 = NULL;
		value5 = NULL;
		cache = NULL;
		cacheURI = NULL;
	}
};

struct mlXPathCompExpr {
    int nbStep;			// Number of steps in this expression 
    int maxStep;		// Maximum number of steps allocated 
    mlXPathStepOp *steps;	// ops for computation of this expression 
    int last;			// index of last step in expression 
    wchar_t *expr;		// the expression being computed 
    mlDict* dict;		// the dictionnary to use if any 
#ifdef DEBUG_EVAL_COUNTS
    int nb;
    wchar_t *string;
#endif
//#ifdef XPATH_STREAMING
//    xmlPatternPtr stream;
//#endif
	mlXPathCompExpr(){
		maxStep = 10;
		nbStep = 0;
		steps = MP_NEW_ARR( mlXPathStepOp, maxStep);
		last = -1;
		expr = NULL;
		dict = NULL;
#ifdef DEBUG_EVAL_COUNTS
		nb = 0;
		string = NULL;
#endif
	}
	~mlXPathCompExpr(){
		MP_DELETE_ARR( steps);
	}
};


typedef enum {
    XPATH_UNDEFINED = 0,
    XPATH_NODESET = 1,
    XPATH_BOOLEAN = 2,
    XPATH_NUMBER = 3,
    XPATH_STRING = 4,
    XPATH_POINT = 5,
    XPATH_RANGE = 6,
    XPATH_LOCATIONSET = 7,
    XPATH_USERS = 8,
    XPATH_XSLT_TREE = 9  // An XSLT value tree, non modifiable 
} mlXPathObjectType;

typedef mlIXMLNode* mlIXMLNodePtr;
typedef mlIXMLNodePtr mlNodePtr;

struct mlNodeSet {
	mlNodeVect nodeTab;
//    int nodeNr;			// number of nodes in the set 
//    int nodeMax;		// size of the array as allocated 
//    xmlNodePtr *nodeTab;	// array of nodes in no particular order 
    // @@ with_ns to check wether namespace nodes should be looked at @@
	int GetSize(){
		return nodeTab.size();
	}
	void Add(mlNodePtr val);
};
typedef mlNodeSet *mlNodeSetPtr;

struct mlXPathObject {
    mlXPathObjectType type;
    mlNodeSetPtr nodesetval;
	int boolval;
	double floatval;
	wchar_t* stringval;
    void *user;
    int index;
    void *user2;
    int index2;
	mlXPathObject(){
		type = XPATH_UNDEFINED;
	    nodesetval = NULL;
		boolval = 0;
	    floatval = 0.0;
	    stringval = NULL;
		void *user = NULL;
	    int index = 0;
		user2 = NULL;
		index2 = 0;
	}
	mlXPathObject::~mlXPathObject();
};

struct mlLocationSet {
    int locNr;		      // number of locations in the set
    int locMax;		      // size of the array as allocated
    mlXPathObjectPtr *locTab; // array of locations
};

typedef mlLocationSet *mlLocationSetPtr;


struct mlXPathParserContext {
    const wchar_t *cur;			// the current char being parsed
    const wchar_t *base;			// the full expression

    int error;				// error code

    mlXPathContext*  context;	// the evaluation context
    mlXPathObject*     value;	// the current value
    int                 valueNr;	// number of values stacked
    int                valueMax;	// max number of values stacked
    mlXPathObjectPtr *valueTab;	// stack of values

    mlXPathCompExpr* comp;		// the precompiled expression
//    int xptr;				// it this an XPointer expression
    mlNodePtr         ancestor;	// used for walking preceding axis
	mlXPathParserContext(const wchar_t* apwcExpr, mlXPathContext* apCtxt){
		cur = base = apwcExpr;
		error = 0;
		context = apCtxt;
		comp = MP_NEW( mlXPathCompExpr);
		valueNr = 0;
		valueMax = 0;
		valueTab = NULL;
		ancestor = NULL;
	}
	~mlXPathParserContext(){
		if(comp != NULL)
			 MP_DELETE( comp);
	}
	void Err(int err){
		error = err;
	}

	mlXPathObjectPtr valuePop();
	int valuePush(mlXPathObject* value);

	void CompPredicate(int filter);
	wchar_t * CompNodeTest(mlXPathTestVal *test, mlXPathTypeVal *type, const wchar_t **prefix, wchar_t *name);
	mlXPathAxisVal IsAxisName(const wchar_t *name);
	void CompNumber();
	double StringEvalNumber(const wchar_t *str);
	wchar_t* ParseLiteral();
	wchar_t* ParseNCName();
	wchar_t* ParseQName(wchar_t **prefix);
	wchar_t* ParseNameComplex(int qualified);
	wchar_t* ParseName();
	int CurrentChar(int *len);
	void CompFunctionCall();
	void CompPrimaryExpr();
	void CompUnionExpr();
	wchar_t* ScanName();
	void CompAdditiveExpr();
	void CompMultiplicativeExpr();
	void CompUnaryExpr();
	void CompFilterExpr();
	void CompStep();
	void CompVariableReference();
	void CompLiteral();
	void CompRelativeLocationPath();
	void CompLocationPath();
	int CompExprAdd(mlXPathCompExpr* comp, int ch1, int ch2,
		mlXPathOp op, int value, int value2, int value3, void *value4, void *value5);
	void CompPathExpr();
	void CompRelationalExpr();
	void CompEqualityExpr();
	void CompAndExpr();
	void CompileExpr(int sort = 0){
		CompAndExpr();
	}
//	bool EvalExpr(){
//		CompileExpr(1);
//	}
	// Evaluation
	void RunEval();
	bool GetResult(mlNodeVect& avNodes);
	int CompOpEval(mlXPathStepOp* op);
	void BooleanFunction(int nargs);
	void NotFunction(int nargs);
	void TrueFunction(int nargs);
	void FalseFunction(int nargs);
	void NumberFunction(int nargs);
	int EqualValues();
	int NotEqualValues();
	int EqualNodeSetFloat(mlXPathObjectPtr arg, double f, int neq);
	int EqualValuesCommon(mlXPathObjectPtr arg1, mlXPathObjectPtr arg2);
	int CompareValues(int inf, int strict);
	int CompareNodeSetValue(int inf, int strict, mlXPathObjectPtr arg, mlXPathObjectPtr val);
	int CompareNodeSetFloat(int inf, int strict, mlXPathObjectPtr arg, mlXPathObjectPtr f);
	int CompareNodeSetString(int inf, int strict, mlXPathObjectPtr arg, mlXPathObjectPtr s);
	int NodeCollectAndTest(mlXPathStepOp* op,
			   mlNodePtr * first, mlNodePtr * last);
	int NodeCollectAndTestNth(mlXPathStepOp* op, int indx,
                              mlNodePtr* first, mlNodePtr* last);
	void Root();
};

mlXPathObject::~mlXPathObject(){
    if ((type == XPATH_NODESET) || (type == XPATH_XSLT_TREE)) {
	if (boolval) {
//	    if (obj->nodesetval != NULL)
//			mlXPathFreeValueTree(obj->nodesetval);
	} else {
	    if (nodesetval != NULL)
			 MP_DELETE( nodesetval);
			// mlXPathFreeNodeSet(obj->nodesetval);
	}
//#ifdef LIBXML_XPTR_ENABLED
//    } else if (obj->type == XPATH_LOCATIONSET) {
//	if (obj->user != NULL)
//	    mlXPtrFreeLocationSet(obj->user);
//#endif
    } else if (type == XPATH_STRING) {
		if (stringval != NULL)
			 MP_DELETE( stringval);
    }
}

mlXPathObjectPtr mlXPathNewFloat(double val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
        xmlXPathErrMemory(NULL, "creating float object\n");
		return(NULL);
    }
    ret->type = XPATH_NUMBER;
    ret->floatval = val;
    return(ret);
}


mlXPathObjectPtr mlXPathNewString(const wchar_t *val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
//        xmlXPathErrMemory(NULL, "creating string object\n");
		return(NULL);
    }
    ret->type = XPATH_STRING;
    if (val != NULL)
	ret->stringval = Strdup(val);
    else
	ret->stringval = Strdup((const wchar_t *)"");
    return(ret);
}

mlXPathObjectPtr mlXPathWrapString (wchar_t *val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
//        xmlXPathErrMemory(NULL, "creating string object\n");
		return(NULL);
    }
    ret->type = XPATH_STRING;
    ret->stringval = val;
    return(ret);
}

//mlXPathObjectPtr
//mlXPathNewCString(const char *val) {
//    mlXPathObjectPtr ret;
//
//    ret = new mlXPathObject;
//    if (ret == NULL) {
////        xmlXPathErrMemory(NULL, "creating string object\n");
//		return(NULL);
//    }
//    ret->type = XPATH_STRING;
//    ret->stringval = Strdup(Lval);
//    return(ret);
//}

mlXPathObjectPtr
mlXPathWrapCString (char * val) {
    return(mlXPathWrapString((wchar_t*)(val)));
}

mlXPathObjectPtr
mlXPathWrapExternal (void *val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
//        xmlXPathErrMemory(NULL, "creating user object\n");
		return(NULL);
    }
    ret->type = XPATH_USERS;
    ret->user = val;
    return(ret);
}

mlNodeSetPtr mlXPathNodeSetMerge(mlNodeSetPtr val1, mlNodeSetPtr val2) {

    if (val2 == NULL) return(val1);
    if (val1 == NULL) {
		val1 = MP_NEW( mlNodeSet);
    }
//	val1->nodes.push_back(val2->nodes);
	mlNodeVect::const_iterator cvi;
	for(cvi = val2->nodeTab.begin(); cvi != val2->nodeTab.end(); cvi++){
		val1->nodeTab.push_back(*cvi);
	}
	return val1;
}

mlNodeSetPtr mlXPathNodeSetMergeUnique(mlNodeSetPtr val1, mlNodeSetPtr val2) {
    int i;

    if (val2 == NULL) return(val1);
    if (val1 == NULL) {
		val1 = MP_NEW( mlNodeSet);
    }

    // @@ with_ns to check whether namespace nodes should be looked at @@

    for (i = 0;i < val2->GetSize();i++) {
//		if (val2->nodeTab[i]->nodeType == XML_NAMESPACE_DECL) {
//			mlNsPtr ns = (mlNsPtr) val2->nodeTab[i];
//
//			val1->nodeTab.push_back(mlXPathNodeSetDupNs((mlNodePtr) ns->next, ns));
//		} else
			val1->nodeTab.push_back(val2->nodeTab[i]);
    }

    return(val1);
}

mlXPathObjectPtr
mlXPathObjectCopy(mlXPathObjectPtr val) {
    mlXPathObjectPtr ret;

    if (val == NULL)
	return(NULL);

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
//        xmlXPathErrMemory(NULL, "copying object\n");
		return(NULL);
    }
	*ret = *val;
    switch (val->type) {
	case XPATH_BOOLEAN:
	case XPATH_NUMBER:
	case XPATH_POINT:
	case XPATH_RANGE:
	    break;
	case XPATH_STRING:
	    ret->stringval = Strdup(val->stringval);
	    break;
	case XPATH_XSLT_TREE:
#if 0
//
//  Removed 11 July 2004 - the current handling of xslt tmpRVT nodes means that
//  this previous handling is no longer correct, and can cause some serious
//  problems (ref. bug 145547)

	    if ((val->nodesetval != NULL) &&
		(val->nodesetval->nodeTab != NULL)) {
		xmlNodePtr cur, tmp;
		xmlDocPtr top;

		ret->boolval = 1;
		top =  xmlNewDoc(NULL);
		top->name = (char *)
		    Strdup(val->nodesetval->nodeTab[0]->name);
		ret->user = top;
		if (top != NULL) {
		    top->doc = top;
		    cur = val->nodesetval->nodeTab[0]->children;
		    while (cur != NULL) {
			tmp = xmlDocCopyNode(cur, top, 1);
			xmlAddChild((xmlNodePtr) top, tmp);
			cur = cur->next;
		    }
		}

		ret->nodesetval = xmlXPathNodeSetCreate((xmlNodePtr) top);
	    } else
		ret->nodesetval = xmlXPathNodeSetCreate(NULL);
	    // Deallocate the copied tree value 
	    break;
#endif
	case XPATH_NODESET:
	    ret->nodesetval = mlXPathNodeSetMerge(NULL, val->nodesetval);
	    // Do not deallocate the copied tree value 
	    ret->boolval = 0;
	    break;
	case XPATH_LOCATIONSET:
#ifdef LIBXML_XPTR_ENABLED
	{
	    mlLocationSetPtr loc = (mlLocationSetPtr)val->user;
	    ret->user = (void *) mlXPtrLocationSetMerge(NULL, loc);
	    break;
	}
#endif
        case XPATH_USERS:
	    ret->user = val->user;
	    break; 
        case XPATH_UNDEFINED:
	    xmlGenericError(xmlGenericErrorContext,
		    "xmlXPathObjectCopy: unsupported type %d\n",
		    val->type);
	    break;
    }
    return(ret);
}

const xmlChar* mlXPathContext::NsLookup(const xmlChar *prefix){
	if(prefix == NULL) return NULL; // Tandy
#ifdef XML_XML_NAMESPACE
    if (isEqual(prefix, (const xmlChar *) "xml"))
		return(XML_XML_NAMESPACE);
#endif

    if (namespaces != NULL) {
		int i;

		for (i = 0;i < nsNr;i++) {
			if ((namespaces[i] != NULL) &&   // Pathed on 29.08.2005 by BDima to fix namespace bug
			    ((namespaces[i]->prefix == NULL || *namespaces[i]->prefix == 0) && 
				 (prefix == NULL || *prefix == 0)) ||
			    (isEqual(namespaces[i]->prefix, prefix)))
			return(namespaces[i]->href);
		}
    }

    return((const xmlChar *) nsHash->Lookup(prefix));
}

mlXPathObjectPtr mlXPathContext::VariableLookupNS(const xmlChar *name, const xmlChar *ns_uri) {
    if (varLookupFunc != NULL) {
	mlXPathObjectPtr ret;

	ret = ((mlXPathVariableLookupFunc)varLookupFunc)
	        (varLookupData, name, ns_uri);
	if (ret != NULL) return(ret);
    }

    if (varHash == NULL)
		return(NULL);
    if (name == NULL)
		return(NULL);

    return(mlXPathObjectCopy((mlXPathObjectPtr)	varHash->Lookup2(name, ns_uri)));
}

mlXPathObjectPtr mlXPathParserContext::valuePop()
{
    mlXPathObjectPtr ret;

    if ((this->valueNr <= 0))
        return (0);
    this->valueNr--;
    if (this->valueNr > 0)
        this->value = this->valueTab[this->valueNr - 1];
    else
        this->value = NULL;
    ret = this->valueTab[this->valueNr];
    this->valueTab[this->valueNr] = 0;
    return (ret);
}

int mlXPathParserContext::valuePush(mlXPathObject* value)
{
    if ((value == NULL)) return(-1);
    if (this->valueNr >= this->valueMax) {
        mlXPathObjectPtr *tmp;

//        tmp = (mlXPathObject*) realloc(this->valueTab,
//                                             2 * this->valueMax *
//                                             sizeof(this->valueTab[0]));
		tmp = MP_NEW_ARR( mlXPathObjectPtr, 2 * this->valueMax);
		for(int ii = 0; ii < this->valueMax; ii++)
			tmp[ii] = this->valueTab[ii];
        if (tmp == NULL) {
//            xmlGenericError(xmlGenericErrorContext, "realloc failed !\n");
            return (0);
        }
        this->valueMax *= 2;
		this->valueTab = tmp;
    }
    this->valueTab[this->valueNr] = value;
    this->value = value;
    return (this->valueNr++);
}

mlXPathObjectPtr mlXPathNewBoolean(int val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    if (ret == NULL) {
        xmlXPathErrMemory(NULL, "creating boolean object\n");
		return(NULL);
    }
    // memset(ret, 0 , (size_t) sizeof(xmlXPathObject));
    ret->type = XPATH_BOOLEAN;
    ret->boolval = (val != 0);
    return(ret);
}

void mlXPathTrueFunction(mlXPathParserContext* ctxt, int nargs) {
//    CHECK_ARITY(0);
    ctxt->valuePush(mlXPathNewBoolean(1));
}

void mlXPathFalseFunction(mlXPathParserContext* ctxt, int nargs) {
//    CHECK_ARITY(0);
    ctxt->valuePush(mlXPathNewBoolean(0));
}

typedef void (*mlXPathFunction) (mlXPathParserContext* ctxt, int nargs);

struct mlXPathFunc{
	const wchar_t* name;
	mlXPathFunction f;
};

mlXPathFunc XPathFuncs[]=
{
//	{ L"boolean",                         mlXPathBooleanFunction },
// { L"ceiling",                         mlXPathCeilingFunction }, 
// { L"count",                         mlXPathCountFunction }, 
// { L"concat",                         mlXPathConcatFunction }, 
// { L"contains",                         mlXPathContainsFunction }, 
// { L"id",                         mlXPathIdFunction }, 
 { L"false",                         mlXPathFalseFunction }, 
// { L"floor",                         mlXPathFloorFunction }, 
// { L"last",                         mlXPathLastFunction }, 
// { L"lang",                         mlXPathLangFunction }, 
// { L"local-name",                         mlXPathLocalNameFunction }, 
// { L"not",                         mlXPathNotFunction }, 
// { L"name",                         mlXPathNameFunction }, 
// { L"namespace-uri",                         mlXPathNamespaceURIFunction }, 
// { L"normalize-space",                         mlXPathNormalizeFunction }, 
// { L"number",                         mlXPathNumberFunction }, 
// { L"position",                         mlXPathPositionFunction }, 
// { L"round",                         mlXPathRoundFunction }, 
// { L"string",                         mlXPathStringFunction }, 
// { L"string-length",                         mlXPathStringLengthFunction }, 
// { L"starts-with",                         mlXPathStartsWithFunction }, 
// { L"substring",                         mlXPathSubstringFunction }, 
// { L"substring-before",                         mlXPathSubstringBeforeFunction }, 
// { L"substring-after",                         mlXPathSubstringAfterFunction }, 
// { L"sum",                         mlXPathSumFunction }, 
 { L"true",                         mlXPathTrueFunction }, 
// { L"translate",                         mlXPathTranslateFunction }, 
	{ L"", NULL}

//   xmlXPathRegisterFuncNS(ctxt, (const wchar_t *)"escape-uri",
//	 (const wchar_t *)"http://www.w3.org/2002/08/xquery-functions",
//                         xmlXPathEscapeUriFunction);
};

int mlXPathContext::RegisterFuncNS(const wchar_t *name,
		       const wchar_t *ns_uri, mlXPathFunction f) {
    if (name == NULL)
		return(-1);

    if (this->funcHash == NULL)
		MP_NEW_V( this->funcHash, mlHashTable, 0); // xmlHashCreate(0);
    if (this->funcHash == NULL)
		return(-1);
    if (f == NULL)
        return funcHash->RemoveEntry2(name, ns_uri, NULL);
    return funcHash->AddEntry2(name, ns_uri, XML_CAST_FPTR(f));
}

void mlXPathContext::RegisterAllFunctions(){
	for(int ii=0; ; ii++){
		mlXPathFunc& func = XPathFuncs[ii];
		if(func.name[0] == 0) break;
		RegisterFuncNS(func.name, NULL, func.f);
	}
}

#define XP_ERROR0(X)							\
    { Err(X); return(0); }

#define XP_ERROR(X)							\
    { Err(X); return; }

#define IsBaseChar_ch(c)	(((0x41 <= (c)) && ((c) <= 0x5a)) || \
				 ((0x61 <= (c)) && ((c) <= 0x7a)) || \
				 ((0xc0 <= (c)) && ((c) <= 0xd6)) || \
				 ((0xd8 <= (c)) && ((c) <= 0xf6)) || \
				  (0xf8 <= (c)))

struct xmlChSRange {
    unsigned short	low;
    unsigned short	high;
};

struct xmlChLRange {
    unsigned int	low;
    unsigned int	high;
};

struct xmlChRangeGroup {
    int			nbShortRange;
    int			nbLongRange;
    const xmlChSRange	*shortRange;	// points to an array of ranges
    const xmlChLRange	*longRange;
};

int mlCharInRange (unsigned int val, const xmlChRangeGroup* rptr) {
    int low, high, mid;
    xmlChSRange* sptr;
    xmlChLRange* lptr;

    if (rptr == NULL) return(0);
    if (val < 0x10000) {	// is val in 'short' or 'long'  array?
	if (rptr->nbShortRange == 0)
	    return 0;
	low = 0;
	high = rptr->nbShortRange - 1;
	sptr = (xmlChSRange*)rptr->shortRange;
	while (low <= high) {
	    mid = (low + high) / 2;
	    if ((unsigned short) val < sptr[mid].low) {
		high = mid - 1;
	    } else {
	        if ((unsigned short) val > sptr[mid].high) {
		    low = mid + 1;
		} else {
		    return 1;
		}
	    }
	}
    } else {
	if (rptr->nbLongRange == 0) {
	    return 0;
	}
	low = 0;
	high = rptr->nbLongRange - 1;
	lptr = (xmlChLRange*)(rptr->longRange);
	while (low <= high) {
	    mid = (low + high) / 2;
	    if (val < lptr[mid].low) {
		high = mid - 1;
	    } else {
	        if (val > lptr[mid].high) {
		    low = mid + 1;
		} else {
		    return 1;
		}
	    }
	}
    }
    return 0;
}


#define xmlIsBaseCharQ(c)	(((c) < 0x100) ? \
				 xmlIsBaseChar_ch((c)) : \
				 mlCharInRange((c), &xmlIsBaseCharGroup))

const xmlChRangeGroup xmlIsBaseCharGroup;

#define xmlIsBlank_ch(c)	(((c) == 0x20) || \
				 ((0x9 <= (c)) && ((c) <= 0xa)) || \
				 ((c) == 0xd))

#define xmlIsBlankQ(c)		(((c) < 0x100) ? \
				 xmlIsBlank_ch((c)) : 0)


#define xmlIsChar_ch(c)		(((0x9 <= (c)) && ((c) <= 0xa)) || \
				 ((c) == 0xd) || \
				  (0x20 <= (c)))

#define xmlIsCharQ(c)		(((c) < 0x100) ? \
				 xmlIsChar_ch((c)) :\
				(((0x100 <= (c)) && ((c) <= 0xd7ff)) || \
				 ((0xe000 <= (c)) && ((c) <= 0xfffd)) || \
				 ((0x10000 <= (c)) && ((c) <= 0x10ffff))))

const xmlChRangeGroup xmlIsCharGroup;

#define xmlIsCombiningQ(c)	(((c) < 0x100) ? \
				 0 : \
				 mlCharInRange((c), &xmlIsCombiningGroup))

const xmlChRangeGroup xmlIsCombiningGroup;

#define xmlIsDigit_ch(c)	(((0x30 <= (c)) && ((c) <= 0x39)))

#define xmlIsDigitQ(c)		(((c) < 0x100) ? \
				 xmlIsDigit_ch((c)) : \
				 mlCharInRange((c), &xmlIsDigitGroup))

const xmlChRangeGroup xmlIsDigitGroup;

#define xmlIsExtender_ch(c)	(((c) == 0xb7))

#define xmlIsExtenderQ(c)	(((c) < 0x100) ? \
				 xmlIsExtender_ch((c)) : \
				 mlCharInRange((c), &xmlIsExtenderGroup))

const xmlChRangeGroup xmlIsExtenderGroup;

#define xmlIsIdeographicQ(c)	(((c) < 0x100) ? \
				 0 :\
				(((0x4e00 <= (c)) && ((c) <= 0x9fa5)) || \
				 ((c) == 0x3007) || \
				 ((0x3021 <= (c)) && ((c) <= 0x3029))))

#define xmlIsBaseChar_ch(c)	(((0x41 <= (c)) && ((c) <= 0x5a)) || \
				 ((0x61 <= (c)) && ((c) <= 0x7a)) || \
				 ((0xc0 <= (c)) && ((c) <= 0xd6)) || \
				 ((0xd8 <= (c)) && ((c) <= 0xf6)) || \
				  (0xf8 <= (c)))

#define IS_BYTE_CHAR(c)	 xmlIsChar_ch(c)
#define IS_CHAR(c)   xmlIsCharQ(c)
#define IS_CHAR_CH(c)  xmlIsChar_ch(c)
#define IS_BLANK(c)  xmlIsBlankQ(c)
#define IS_BLANK_CH(c)  xmlIsBlank_ch(c)
#define IS_BASECHAR(c) xmlIsBaseCharQ(c)
#define IS_DIGIT(c) xmlIsDigitQ(c)
#define IS_DIGIT_CH(c)  xmlIsDigit_ch(c)
#define IS_COMBINING(c) xmlIsCombiningQ(c)
#define IS_COMBINING_CH(c) 0 
#define IS_EXTENDER(c) xmlIsExtenderQ(c)
#define IS_EXTENDER_CH(c)  xmlIsExtender_ch(c)
#define IS_IDEOGRAPHIC(c) xmlIsIdeographicQ(c)
#define IS_LETTER(c) (IS_BASECHAR(c) || IS_IDEOGRAPHIC(c))
#define IS_LETTER_CH(c) xmlIsBaseChar_ch(c)
#define IS_ASCII_LETTER(c)	(((0x41 <= (c)) && ((c) <= 0x5a)) || \
				 ((0x61 <= (c)) && ((c) <= 0x7a)))
#define IS_ASCII_DIGIT(c)	((0x30 <= (c)) && ((c) <= 0x39))
#define IS_PUBIDCHAR(c)	xmlIsPubidCharQ(c)


int mlXPathParserContext::CurrentChar(int *len) {
    unsigned char c;
    unsigned int val;
    const wchar_t *lcur;

    lcur = cur;

    //
     // We are supposed to handle UTF8, check it's valid
     // From rfc2044: encoding of the Unicode values on UTF-8:
     //
     // UCS-4 range (hex.)           UTF-8 octet sequence (binary)
     // 0000 0000-0000 007F   0xxxxxxx
     // 0000 0080-0000 07FF   110xxxxx 10xxxxxx
     // 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
     //
     // Check for the 0x110000 limit too
     
    c = *lcur;
    if (c & 0x80) {
	if ((lcur[1] & 0xc0) != 0x80)
	    goto encoding_error;
	if ((c & 0xe0) == 0xe0) {

	    if ((cur[2] & 0xc0) != 0x80)
		goto encoding_error;
	    if ((c & 0xf0) == 0xf0) {
		if (((c & 0xf8) != 0xf0) ||
		    ((cur[3] & 0xc0) != 0x80))
		    goto encoding_error;
		// 4-byte code 
		*len = 4;
		val = (lcur[0] & 0x7) << 18;
		val |= (lcur[1] & 0x3f) << 12;
		val |= (lcur[2] & 0x3f) << 6;
		val |= lcur[3] & 0x3f;
	    } else {
	      // 3-byte code 
		*len = 3;
		val = (lcur[0] & 0xf) << 12;
		val |= (lcur[1] & 0x3f) << 6;
		val |= lcur[2] & 0x3f;
	    }
	} else {
	  // 2-byte code 
	    *len = 2;
	    val = (lcur[0] & 0x1f) << 6;
	    val |= lcur[1] & 0x3f;
	}
	if (!IS_CHAR(val)) {
	    XP_ERROR0(XPATH_INVALID_CHAR_ERROR);
	}    
	return(val);
    } else {
	// 1-byte code 
	*len = 1;
	return((int) *lcur);
    }
encoding_error:
    //
     // If we detect an UTF8 error that probably means that the
     // input encoding didn't get properly advertised in the
     // declaration header. Report the error and switch the encoding
     // to ISO-Latin-1 (if you don't like this policy, just declare the
     // encoding !)
     
    *len = 0;
    XP_ERROR0(XPATH_ENCODING_ERROR);
}

int CopyChar(int len, wchar_t *out, int val) {
    if (out == NULL) return(0);
    // the len parameter is ignored
//    if  (val >= 0x80) {
//	return(CopyCharMultiByte (out, val));
//    }
    *out = (wchar_t) val;
    return 1;
}

#define TODO 								\
    mlGenericError(mlGenericErrorContext,				\
	    "Unimplemented block at %s:%d\n",				\
            __FILE__, __LINE__);

#define CHECK_ERROR							\
    if (error != XPATH_EXPRESSION_OK) return

#define CHECK_ERROR0							\
    if (error != XPATH_EXPRESSION_OK) return(0)

#define CUR (*cur)
#define SKIP(val) cur += (val)
#define NXT(val) cur[(val)]
#define CUR_PTR cur
#define CUR_CHAR(l) CurrentChar(&l)

#define COPY_BUF(l,b,i,v)                                              \
    if (l == 1) b[i++] = (wchar_t) v;                                  \
    else i += CopyChar(l,&b[i],v)

#define NEXTL(l)  cur += l

#define SKIP_BLANKS 							\
    while (IS_BLANK_CH(*(cur))) NEXT

#define CURRENT (*cur)
#define NEXT ((*cur) ?  cur++: cur)

#define PUSH_FULL_EXPR(op, op1, op2, val, val2, val3, val4, val5)	\
    CompExprAdd(comp, (op1), (op2),			\
	                (op), (val), (val2), (val3), (val4), (val5))
#define PUSH_LONG_EXPR(op, val, val2, val3, val4, val5)			\
    CompExprAdd(comp, comp->last, -1,		\
	                (op), (val), (val2), (val3), (val4), (val5))

#define PUSH_LEAVE_EXPR(op, val, val2) 					\
	CompExprAdd(comp, -1, -1, (op), (val), (val2), 0 ,NULL ,NULL)

#define PUSH_UNARY_EXPR(op, ch, val, val2) 				\
	CompExprAdd(comp, (ch), -1, (op), (val), (val2), 0 ,NULL ,NULL)

#define PUSH_BINARY_EXPR(op, ch1, ch2, val, val2) 			\
	CompExprAdd(comp, (ch1), (ch2), (op),			\
			(val), (val2), 0 ,NULL ,NULL)

void mlXPathParserContext::CompFunctionCall() {
    wchar_t *name;
    wchar_t *prefix;
    int nbargs = 0;

    name = ParseQName(&prefix);
    if (name == NULL) {
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    SKIP_BLANKS;
#ifdef DEBUG_EXPR
    if (prefix == NULL)
	xmlGenericError(xmlGenericErrorContext, "Calling function %s\n",
			name);
    else
	xmlGenericError(xmlGenericErrorContext, "Calling function %s:%s\n",
			prefix, name);
#endif

    if (CUR != '(') {
	XP_ERROR(XPATH_EXPR_ERROR);
    }
    NEXT;
    SKIP_BLANKS;

    comp->last = -1;
    if (CUR != ')') {
	while (CUR != 0) {
	    int op1 = comp->last;
	    comp->last = -1;
	    CompileExpr();
	    CHECK_ERROR;
	    PUSH_BINARY_EXPR(XPATH_OP_ARG, op1, comp->last, 0, 0);
	    nbargs++;
	    if (CUR == ')') break;
	    if (CUR != ',') {
		XP_ERROR(XPATH_EXPR_ERROR);
	    }
	    NEXT;
	    SKIP_BLANKS;
	}
    }
    PUSH_LONG_EXPR(XPATH_OP_FUNCTION, nbargs, 0, 0,
	           name, prefix);
    NEXT;
    SKIP_BLANKS;
}

void mlXPathParserContext::CompPrimaryExpr() {
    SKIP_BLANKS;
    if (CUR == '$') CompVariableReference();
    else if (CUR == '(') {
	NEXT;
	SKIP_BLANKS;
	CompileExpr();
	CHECK_ERROR;
	if (CUR != ')') {
	    XP_ERROR(XPATH_EXPR_ERROR);
	}
	NEXT;
	SKIP_BLANKS;
    } else if (IS_ASCII_DIGIT(CUR) || (CUR == '.' && IS_ASCII_DIGIT(NXT(1)))) {
		CompNumber();
    } else if ((CUR == '\'') || (CUR == '"')) {
		CompLiteral();
    } else {
		CompFunctionCall();
    }
    SKIP_BLANKS;
}

void mlXPathParserContext::CompFilterExpr() {
    CompPrimaryExpr();
    CHECK_ERROR;
    SKIP_BLANKS;
    
    while (CUR == '[') {
		CompPredicate(1);
		SKIP_BLANKS;
    }
}

wchar_t* mlXPathParserContext::ScanName() {
    int len = 0, l;
    int c;
    const wchar_t *lcur;
    wchar_t *ret;

    lcur = cur;

    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || // accelerators 
	(!IS_LETTER(c) && (c != '_') &&
         (c != ':'))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && // test bigname.xml 
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || (c == ':') || 
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	len += l;
	NEXTL(l);
	c = CUR_CHAR(l);
    }
    ret = Strndup(lcur, cur - lcur);
    cur = lcur;
    return(ret);
}

int IsNodeType(const wchar_t* name) {
    if (name == NULL)
		return(0);

    if (isEqual(name, L"node"))
		return(1);
    if (isEqual(name, L"text"))
		return(1);
    if (isEqual(name, L"comment"))
		return(1);
    if (isEqual(name, L"processing-instruction"))
		return(1);
    return(0);
}

int mlXPathParserContext::CompExprAdd(mlXPathCompExpr* comp, int ch1, int ch2,
   mlXPathOp op, int value, int value2, int value3, void *value4, void *value5) 
{
    if (comp->nbStep >= comp->maxStep) {
		mlXPathStepOp *real;

		comp->maxStep *= 2;

		//real = (mlXPathStepOp *) MP_NEW_ARR( char, comp->maxStep * sizeof(mlXPathStepOp));
		real = MP_NEW_ARR( mlXPathStepOp, comp->maxStep);
		if (real == NULL) {
			comp->maxStep /= 2;
//			xmlXPathErrMemory(NULL, "adding step\n");
			return(-1);
		}
		for(int ii = 0; ii < comp->nbStep; ii++){
			real[ii] = comp->steps[ii];
		}
		comp->steps = real;
    }
    comp->last = comp->nbStep;
    comp->steps[comp->nbStep].ch1 = ch1;
    comp->steps[comp->nbStep].ch2 = ch2;
    comp->steps[comp->nbStep].op = op;
    comp->steps[comp->nbStep].value = value;
    comp->steps[comp->nbStep].value2 = value2;
    comp->steps[comp->nbStep].value3 = value3;
    if ((comp->dict != NULL) &&
        ((op == XPATH_OP_FUNCTION) || (op == XPATH_OP_VARIABLE) ||
	 (op == XPATH_OP_COLLECT))) {
        if (value4 != NULL) {
	    comp->steps[comp->nbStep].value4 = (wchar_t *)
	        (void *)comp->dict->Lookup((wchar_t *)value4, -1);
	    xmlFree(value4);
	} else
	    comp->steps[comp->nbStep].value4 = NULL;
        if (value5 != NULL) {
	    comp->steps[comp->nbStep].value5 = (wchar_t *)
	        (void *)comp->dict->Lookup((wchar_t *)value5, -1);
	    xmlFree(value5);
	} else
	    comp->steps[comp->nbStep].value5 = NULL;
    } else {
	comp->steps[comp->nbStep].value4 = value4;
	comp->steps[comp->nbStep].value5 = value5;
    }
    comp->steps[comp->nbStep].cache = NULL;
    return(comp->nbStep++);
}

wchar_t* mlXPathParserContext::ParseNCName() {
    const wchar_t *in;
    wchar_t *ret;
    int count = 0;

    if ((this->cur == NULL)) return(NULL);
    //
     // Accelerator for simple ASCII names
     
    in = this->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '.') ||
	       (*in == '-'))
	    in++;
	if ((*in == ' ') || (*in == '>') || (*in == '/') ||
            (*in == '[') || (*in == ']') || (*in == ':') ||
            (*in == '@') || (*in == '*')) {
	    count = in - this->cur;
	    if (count == 0)
		return(NULL);
	    ret = Strndup(this->cur, count);
	    this->cur = in;
	    return(ret);
	}
    }
    return(ParseNameComplex(0));
}

wchar_t* mlXPathParserContext::ParseQName(wchar_t **prefix) {
    wchar_t *ret = NULL;

    *prefix = NULL;
    ret = ParseNCName();
    if (CUR == ':') {
        *prefix = ret;
	NEXT;
	ret = ParseNCName();
    }
    return(ret);
}

wchar_t* mlXPathParserContext::ParseName() {
    const wchar_t *in;
    wchar_t *ret;
    int count = 0;

    if ((this->cur == NULL)) return(NULL);
    //
     // Accelerator for simple ASCII names
     
    in = this->cur;
    if (((*in >= 0x61) && (*in <= 0x7A)) ||
	((*in >= 0x41) && (*in <= 0x5A)) ||
	(*in == '_') || (*in == ':')) {
	in++;
	while (((*in >= 0x61) && (*in <= 0x7A)) ||
	       ((*in >= 0x41) && (*in <= 0x5A)) ||
	       ((*in >= 0x30) && (*in <= 0x39)) ||
	       (*in == '_') || (*in == '-') ||
	       (*in == ':') || (*in == '.'))
	    in++;
	if ((*in > 0) && (*in < 0x80)) {
	    count = in - this->cur;
	    ret = Strndup(this->cur, count);
	    this->cur = in;
	    return(ret);
	}
    }
    return(ParseNameComplex(1));
}

wchar_t* mlXPathParserContext::ParseNameComplex(int qualified) {
    wchar_t buf[XML_MAX_NAMELEN + 5];
    int len = 0, l;
    int c;

    //
     // Handler for more complex cases
     
    c = CUR_CHAR(l);
    if ((c == ' ') || (c == '>') || (c == '/') || // accelerators 
        (c == '[') || (c == ']') || (c == '@') || // accelerators 
        (c == '*') || // accelerators 
	(!IS_LETTER(c) && (c != '_') &&
         ((qualified) && (c != ':')))) {
	return(NULL);
    }

    while ((c != ' ') && (c != '>') && (c != '/') && // test bigname.xml 
	   ((IS_LETTER(c)) || (IS_DIGIT(c)) ||
            (c == '.') || (c == '-') ||
	    (c == '_') || ((qualified) && (c == ':')) || 
	    (IS_COMBINING(c)) ||
	    (IS_EXTENDER(c)))) {
	COPY_BUF(l,buf,len,c);
	NEXTL(l);
	c = CUR_CHAR(l);
	if (len >= XML_MAX_NAMELEN) {
	    //
	     // Okay someone managed to make a huge name, so he's ready to pay
	     // for the processing speed.
	     
	    wchar_t *buffer;
	    int max = len * 2;
	    
	    buffer = MP_NEW_ARR( wchar_t, max);
	    if (buffer == NULL) {
			XP_ERROR0(XPATH_MEMORY_ERROR);
	    }
	    memcpy(buffer, buf, len);
	    while ((IS_LETTER(c)) || (IS_DIGIT(c)) || // test bigname.xml 
		   (c == '.') || (c == '-') ||
		   (c == '_') || ((qualified) && (c == ':')) || 
		   (IS_COMBINING(c)) ||
		   (IS_EXTENDER(c))) {
		if (len + 10 > max) {
		    max *= 2;
			MP_DELETE_ARR( buffer);
		    buffer = MP_NEW_ARR( wchar_t, max);
		    if (buffer == NULL) {
			XP_ERROR0(XPATH_MEMORY_ERROR);
		    }
		}
		COPY_BUF(l,buffer,len,c);
		NEXTL(l);
		c = CUR_CHAR(l);
	    }
	    buffer[len] = 0;
	    return(buffer);
	}
    }
    if (len == 0)
	return(NULL);
    return(Strndup(buf, len));
}

#define MAX_FRAC 20

//
 // These are used as divisors for the fractional part of a number.
 // Since the table includes 1.0 (representing '0' fractional digits),
 // it must be dimensioned at MAX_FRAC+1 (bug 133921)
 
static double my_pow10[MAX_FRAC+1] = {
    1.0, 10.0, 100.0, 1000.0, 10000.0,
    100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0,
    10000000000.0, 100000000000.0, 1000000000000.0, 10000000000000.0,
    100000000000000.0,
    1000000000000000.0, 10000000000000000.0, 100000000000000000.0,
    1000000000000000000.0, 10000000000000000000.0, 100000000000000000000.0
};

double mlXPathParserContext::StringEvalNumber(const wchar_t *str) {
    const wchar_t *cur = str;
    double ret;
    int ok = 0;
    int isneg = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif
    if (cur == NULL) return(0);
    while (IS_BLANK_CH(*cur)) cur++;
    if ((*cur != '.') && ((*cur < '0') || (*cur > '9')) && (*cur != '-')) {
        return(xmlXPathNAN);
    }
    if (*cur == '-') {
	isneg = 1;
	cur++;
    }

#ifdef __GNUC__
    //
     // tmp/temp is a workaround against a gcc compiler bug
     // http://veillard.com/gcc.bug
     
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10;
	tmp = (*cur - '0');
	ok = 1;
	cur++;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	ok = 1;
	cur++;
    }
#endif

    if (*cur == '.') {
	int v, frac = 0;
	double fraction = 0;

        cur++;
	if (((*cur < '0') || (*cur > '9')) && (!ok)) {
	    return(xmlXPathNAN);
	}
	while (((*cur >= '0') && (*cur <= '9')) && (frac < MAX_FRAC)) {
	    v = (*cur - '0');
	    fraction = fraction * 10 + v;
	    frac = frac + 1;
	    cur++;
	}
	fraction /= my_pow10[frac];
	ret = ret + fraction;
	while ((*cur >= '0') && (*cur <= '9'))
	    cur++;
    }
    if ((*cur == 'e') || (*cur == 'E')) {
      cur++;
      if (*cur == '-') {
	is_exponent_negative = 1;
	cur++;
      } else if (*cur == '+') {
        cur++;
      }
      while ((*cur >= '0') && (*cur <= '9')) {
	exponent = exponent * 10 + (*cur - '0');
	cur++;
      }
    }
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur != 0) return(xmlXPathNAN);
    if (isneg) ret = -ret;
    if (is_exponent_negative) exponent = -exponent;
    ret *= pow(10.0, (double)exponent);
    return(ret);
}

void mlXPathParserContext::CompNumber()
{
    double ret = 0.0;
    double mult = 1;
    int ok = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif

    CHECK_ERROR;
    if ((CUR != '.') && ((CUR < '0') || (CUR > '9'))) {
        XP_ERROR(XPATH_NUMBER_ERROR);
    }
#ifdef __GNUC__
    //
     // tmp/temp is a workaround against a gcc compiler bug
     // http://veillard.com/gcc.bug
     
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10;
	tmp = (CUR - '0');
        ok = 1;
        NEXT;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10 + (CUR - '0');
	ok = 1;
	NEXT;
    }
#endif
    if (CUR == '.') {
        NEXT;
        if (((CUR < '0') || (CUR > '9')) && (!ok)) {
            XP_ERROR(XPATH_NUMBER_ERROR);
        }
        while ((CUR >= '0') && (CUR <= '9')) {
            mult /= 10;
            ret = ret + (CUR - '0') * mult;
            NEXT;
        }
    }
    if ((CUR == 'e') || (CUR == 'E')) {
        NEXT;
        if (CUR == '-') {
            is_exponent_negative = 1;
            NEXT;
        } else if (CUR == '+') {
	    NEXT;
	}
        while ((CUR >= '0') && (CUR <= '9')) {
            exponent = exponent * 10 + (CUR - '0');
            NEXT;
        }
        if (is_exponent_negative)
            exponent = -exponent;
        ret *= pow(10.0, (double) exponent);
    }
    PUSH_LONG_EXPR(XPATH_OP_VALUE, XPATH_NUMBER, 0, 0,
                   mlXPathNewFloat(ret), NULL);
}

wchar_t* mlXPathParserContext::ParseLiteral() {
    const wchar_t *q;
    wchar_t *ret = NULL;

    if (CUR == '"') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR_CH(CUR)) && (CUR != '"'))
	    NEXT;
	if (!IS_CHAR_CH(CUR)) {
	    XP_ERROR0(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = Strndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR_CH(CUR)) && (CUR != '\''))
	    NEXT;
	if (!IS_CHAR_CH(CUR)) {
	    XP_ERROR0(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = Strndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else {
	XP_ERROR0(XPATH_START_LITERAL_ERROR);
    }
    return(ret);
}

void mlXPathParserContext::CompLiteral() {
    const wchar_t *q;
    wchar_t *ret = NULL;

    if (CUR == '"') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR_CH(CUR)) && (CUR != '"'))
	    NEXT;
	if (!IS_CHAR_CH(CUR)) {
	    XP_ERROR(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = Strndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else if (CUR == '\'') {
        NEXT;
	q = CUR_PTR;
	while ((IS_CHAR_CH(CUR)) && (CUR != '\''))
	    NEXT;
	if (!IS_CHAR_CH(CUR)) {
	    XP_ERROR(XPATH_UNFINISHED_LITERAL_ERROR);
	} else {
	    ret = Strndup(q, CUR_PTR - q);
	    NEXT;
        }
    } else {
	XP_ERROR(XPATH_START_LITERAL_ERROR);
    }
    if (ret == NULL) return;
    PUSH_LONG_EXPR(XPATH_OP_VALUE, XPATH_STRING, 0, 0,
	           mlXPathNewString(ret), NULL);
    xmlFree(ret);
}


void mlXPathParserContext::CompVariableReference() {
    wchar_t *name;
    wchar_t *prefix;

    SKIP_BLANKS;
    if (CUR != '$') {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    NEXT;
    name = ParseQName(&prefix);
    if (name == NULL) {
	XP_ERROR(XPATH_VARIABLE_REF_ERROR);
    }
    this->comp->last = -1;
    PUSH_LONG_EXPR(XPATH_OP_VARIABLE, 0, 0, 0,
	           name, prefix);
    SKIP_BLANKS;
}

#undef BAD_CAST
#define xmlStrEqual isEqual

void mlXPathParserContext::CompPredicate(int filter) {
    int op1 = comp->last;

    SKIP_BLANKS;
    if (CUR != '[') {
	XP_ERROR(XPATH_INVALID_PREDICATE_ERROR);
    }
    NEXT;
    SKIP_BLANKS;

    comp->last = -1;
    CompileExpr();
    CHECK_ERROR;

    if (CUR != ']') {
		XP_ERROR(XPATH_INVALID_PREDICATE_ERROR);
    }

    if (filter)
	PUSH_BINARY_EXPR(XPATH_OP_FILTER, op1, comp->last, 0, 0);
    else
	PUSH_BINARY_EXPR(XPATH_OP_PREDICATE, op1, comp->last, 0, 0);

    NEXT;
    SKIP_BLANKS;
}

wchar_t* mlXPathParserContext::CompNodeTest(mlXPathTestVal *test,
	             mlXPathTypeVal *type, const wchar_t **prefix,
		     wchar_t *name) {
    int blanks;

    if ((test == NULL) || (type == NULL) || (prefix == NULL)) {
		STRANGE;
		return(NULL);
    }
    *type = (mlXPathTypeVal) 0;
    *test = (mlXPathTestVal) 0;
    *prefix = NULL;
    SKIP_BLANKS;

    if ((name == NULL) && (CUR == '*')) {
		//
		 // All elements
		 
		NEXT;
		*test = NODE_TEST_ALL;
		return(NULL);
    }

    if (name == NULL)
		name = ParseNCName();
	if (name == NULL) {
		XP_ERROR0(XPATH_EXPR_ERROR);
    }

    blanks = IS_BLANK_CH(CUR);
    SKIP_BLANKS;
    if (CUR == '(') {
	NEXT;
	//
	 // NodeType or PI search
	 //
	if (xmlStrEqual(name, L"comment"))
	    *type = NODE_TYPE_COMMENT;
	else if (xmlStrEqual(name, L"node"))
	    *type = NODE_TYPE_NODE;
	else if (xmlStrEqual(name, L"processing-instruction"))
	    *type = NODE_TYPE_PI;
	else if (xmlStrEqual(name, L"text"))
	    *type = NODE_TYPE_TEXT;
	else {
	    if (name != NULL)
		xmlFree(name);
	    XP_ERROR0(XPATH_EXPR_ERROR);
	}

	*test = NODE_TEST_TYPE;
	
	SKIP_BLANKS;
	if (*type == NODE_TYPE_PI) {
	    //
	     // Specific case: search a PI by name.
	     
	    if (name != NULL)
			xmlFree(name);
	    name = NULL;
	    if (CUR != ')') {
			name = ParseLiteral();
			CHECK_ERROR 0;
			*test = NODE_TEST_PI;
			SKIP_BLANKS;
	    }
	}
	if (CUR != ')') {
	    if (name != NULL)
		xmlFree(name);
	    XP_ERROR0(XPATH_UNCLOSED_ERROR);
	}
	NEXT;
	return(name);
    }
    *test = NODE_TEST_NAME;
    if ((!blanks) && (CUR == ':')) {
	NEXT;

	//
	 // Since currently the parser context don't have a
	 // namespace list associated:
	 // The namespace name for this prefix can be computed
	 // only at evaluation time. The compilation is done
	 // outside of any context.
	 
	*prefix = name;

	if (CUR == '*') {
	    //
	     // All elements
	     
		NEXT;
	    *test = NODE_TEST_ALL;
	    return(NULL);
	}

	name = ParseNCName();
	if (name == NULL) {
	    XP_ERROR0(XPATH_EXPR_ERROR);
	}
    }
    return(name);
}

mlXPathAxisVal mlXPathParserContext::IsAxisName(const wchar_t *name) {
    mlXPathAxisVal ret = (mlXPathAxisVal) 0;
    switch (name[0]) {
	case 'a':
	    if (xmlStrEqual(name, L"ancestor"))
		ret = AXIS_ANCESTOR;
	    if (xmlStrEqual(name, L"ancestor-or-self"))
		ret = AXIS_ANCESTOR_OR_SELF;
	    if (xmlStrEqual(name, L"attribute"))
		ret = AXIS_ATTRIBUTE;
	    break;
	case 'c':
	    if (xmlStrEqual(name, L"child"))
		ret = AXIS_CHILD;
	    break;
	case 'd':
	    if (xmlStrEqual(name, L"descendant"))
		ret = AXIS_DESCENDANT;
	    if (xmlStrEqual(name, L"descendant-or-self"))
		ret = AXIS_DESCENDANT_OR_SELF;
	    break;
	case 'f':
	    if (xmlStrEqual(name, L"following"))
		ret = AXIS_FOLLOWING;
	    if (xmlStrEqual(name, L"following-sibling"))
		ret = AXIS_FOLLOWING_SIBLING;
	    break;
	case 'n':
	    if (xmlStrEqual(name, L"namespace"))
		ret = AXIS_NAMESPACE;
	    break;
	case 'p':
	    if (xmlStrEqual(name, L"parent"))
		ret = AXIS_PARENT;
	    if (xmlStrEqual(name, L"preceding"))
		ret = AXIS_PRECEDING;
	    if (xmlStrEqual(name, L"preceding-sibling"))
		ret = AXIS_PRECEDING_SIBLING;
	    break;
	case 's':
	    if (xmlStrEqual(name, L"self"))
		ret = AXIS_SELF;
	    break;
    }
    return(ret);
}

void mlXPathParserContext::CompStep() {
#ifdef LIBXML_XPTR_ENABLED
    int rangeto = 0;
    int op2 = -1;
#endif

    SKIP_BLANKS;
    if ((CUR == '.') && (NXT(1) == '.')) {
	SKIP(2);
	SKIP_BLANKS;
	PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_PARENT,
		    NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
    } else if (CUR == '.') {
	NEXT;
	SKIP_BLANKS;
    } else {
	wchar_t *name = NULL;
	const wchar_t *prefix = NULL;
	mlXPathTestVal test;
	mlXPathAxisVal axis = (mlXPathAxisVal) 0;
	mlXPathTypeVal type;
	int op1;

	//
	 // The modification needed for XPointer change to the production
	 
#ifdef LIBXML_XPTR_ENABLED
	if (this->xptr) {
	    name = ParseNCName();
	    if ((name != NULL) && (isEqual(name, L"range-to"))) {
                op2 = this->comp->last;
		xmlFree(name);
		SKIP_BLANKS;
		if (CUR != '(') {
		    XP_ERROR(XPATH_EXPR_ERROR);
		}
		NEXT;
		SKIP_BLANKS;

		CompileExpr();
		// PUSH_BINARY_EXPR(XPATH_OP_RANGETO, op2, this->comp->last, 0, 0); 
		CHECK_ERROR;

		SKIP_BLANKS;
		if (CUR != ')') {
		    XP_ERROR(XPATH_EXPR_ERROR);
		}
		NEXT;
		rangeto = 1;
		goto eval_predicates;
	    }
	}
#endif
	if (CUR == '*') {
	    axis = AXIS_CHILD;
	} else {
	    if (name == NULL)
		name = ParseNCName();
	    if (name != NULL) {
		axis = IsAxisName(name);
		if (axis != 0) {
		    SKIP_BLANKS;
		    if ((CUR == ':') && (NXT(1) == ':')) {
			SKIP(2);
			xmlFree(name);
			name = NULL;
		    } else {
			// an element name can conflict with an axis one :-\ 
			axis = AXIS_CHILD;
		    }
		} else {
		    axis = AXIS_CHILD;
		}
	    } else if (CUR == '@') {
		NEXT;
		axis = AXIS_ATTRIBUTE;
	    } else {
		axis = AXIS_CHILD;
	    }
	}

	CHECK_ERROR;

	name = CompNodeTest(&test, &type, &prefix, name);
	if (test == 0)
	    return;

#ifdef DEBUG_STEP
	xmlGenericError(xmlGenericErrorContext,
		"Basis : computing new set\n");
#endif

#ifdef DEBUG_STEP
	xmlGenericError(xmlGenericErrorContext, "Basis : ");
	if (this->value == NULL)
	    xmlGenericError(xmlGenericErrorContext, "no value\n");
	else if (this->value->nodesetval == NULL)
	    xmlGenericError(xmlGenericErrorContext, "Empty\n");
	else
	    xmlGenericErrorContextNodeSet(stdout, this->value->nodesetval);
#endif

#ifdef LIBXML_XPTR_ENABLED
eval_predicates:
#endif
	op1 = this->comp->last;
	this->comp->last = -1;

	SKIP_BLANKS;
	while (CUR == '[') {
	    CompPredicate(0);
	}

#ifdef LIBXML_XPTR_ENABLED
	if (rangeto) {
	    PUSH_BINARY_EXPR(XPATH_OP_RANGETO, op2, op1, 0, 0);
	} else
#endif
	    PUSH_FULL_EXPR(XPATH_OP_COLLECT, op1, this->comp->last, axis,
			   test, type, (void *)prefix, (void *)name);

    }
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext, "Step : ");
    if (this->value == NULL)
	xmlGenericError(xmlGenericErrorContext, "no value\n");
    else if (this->value->nodesetval == NULL)
	xmlGenericError(xmlGenericErrorContext, "Empty\n");
    else
	xmlGenericErrorContextNodeSet(xmlGenericErrorContext,
		this->value->nodesetval);
#endif
}

void mlXPathParserContext::CompRelativeLocationPath() {
    SKIP_BLANKS;
    if ((CUR == '/') && (NXT(1) == '/')) {
	SKIP(2);
	SKIP_BLANKS;
	PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
		         NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
    } else if (CUR == '/') {
	    NEXT;
	SKIP_BLANKS;
    }
    CompStep();
    SKIP_BLANKS;
    while (CUR == '/') {
	if ((CUR == '/') && (NXT(1) == '/')) {
	    SKIP(2);
	    SKIP_BLANKS;
	    PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
			     NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
	    CompStep();
	} else if (CUR == '/') {
	    NEXT;
	    SKIP_BLANKS;
	    CompStep();
	}
	SKIP_BLANKS;
    }
}

void mlXPathParserContext::CompLocationPath() {
    SKIP_BLANKS;
    if (CUR != '/') {
        CompRelativeLocationPath();
    } else {
		while (CUR == '/') {
			if ((CUR == '/') && (NXT(1) == '/')) {
			SKIP(2);
			SKIP_BLANKS;
			PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
					 NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
			CompRelativeLocationPath();
			} else if (CUR == '/') {
			NEXT;
			SKIP_BLANKS;
			if ((CUR != 0 ) &&
				((IS_ASCII_LETTER(CUR)) || (CUR == '_') || (CUR == '.') ||
				 (CUR == '@') || (CUR == '*')))
				CompRelativeLocationPath();
			}
		}
    }
}

void mlXPathParserContext::CompPathExpr() {
    int lc = 1;           // Should we branch to LocationPath ?         
    wchar_t *name = NULL; // we may have to preparse a name to find out 

    SKIP_BLANKS;
    if ((CUR == L'$') || (CUR == L'(') || 
    	(IS_ASCII_DIGIT(CUR)) ||
        (CUR == '\'') || (CUR == '"') ||
	(CUR == '.' && IS_ASCII_DIGIT(NXT(1)))) {
	lc = 0;
    } else if (CUR == '*') {
	// relative or absolute location path 
	lc = 1;
    } else if (CUR == '/') {
	// relative or absolute location path 
	lc = 1;
    } else if (CUR == '@') {
	// relative abbreviated attribute location path 
	lc = 1;
    } else if (CUR == '.') {
	// relative abbreviated attribute location path 
	lc = 1;
    } else {
	//
	 // Problem is finding if we have a name here whether it's:
	 //   - a nodetype
	 //   - a function call in which case it's followed by '('
	 //   - an axis in which case it's followed by ':'
	 //   - a element name
	 // We do an a priori analysis here rather than having to
	 // maintain parsed token content through the recursive function
	 // calls. This looks uglier but makes the code easier to
	 // read/write/debug.
	 
	SKIP_BLANKS;
	name = ScanName();
	if ((name != NULL) && (wcsstr(name, L"::") != NULL)) {
#ifdef DEBUG_STEP
	    xmlGenericError(xmlGenericErrorContext,
		    "PathExpr: Axis\n");
#endif
	    lc = 1;
	    MP_DELETE_ARR( name);
	} else if (name != NULL) {
	    int len = wcslen(name);

	    
	    while (NXT(len) != 0) {
		if (NXT(len) == '/') {
		    // element name 
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if (IS_BLANK_CH(NXT(len))) {
		    // ignore blanks 
		    ;
		} else if (NXT(len) == ':') {
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if ((NXT(len) == '(')) {
		    // Note Type or Function 
		    if (IsNodeType(name)) {
#ifdef DEBUG_STEP
		        xmlGenericError(xmlGenericErrorContext,
				"PathExpr: Type search\n");
#endif
			lc = 1;
		    } else {
#ifdef DEBUG_STEP
		        xmlGenericError(xmlGenericErrorContext,
				"PathExpr: function call\n");
#endif
			lc = 0;
		    }
                    break;
		} else if ((NXT(len) == '[')) {
		    // element name 
#ifdef DEBUG_STEP
		    xmlGenericError(xmlGenericErrorContext,
			    "PathExpr: AbbrRelLocation\n");
#endif
		    lc = 1;
		    break;
		} else if ((NXT(len) == '<') || (NXT(len) == '>') ||
			   (NXT(len) == '=')) {
		    lc = 1;
		    break;
		} else {
		    lc = 1;
		    break;
		}
		len++;
	    }
	    if (NXT(len) == 0) {
#ifdef DEBUG_STEP
		xmlGenericError(xmlGenericErrorContext,
			"PathExpr: AbbrRelLocation\n");
#endif
		// element name 
		lc = 1;
	    }
	    xmlFree(name);
	} else {
	    // make sure all cases are covered explicitly 
	    XP_ERROR(XPATH_EXPR_ERROR);
	}
    } 

    if (lc) {
	if (CUR == '/') {
	    PUSH_LEAVE_EXPR(XPATH_OP_ROOT, 0, 0);
	} else {
	    PUSH_LEAVE_EXPR(XPATH_OP_NODE, 0, 0);
	}
	CompLocationPath();
    } else {
	CompFilterExpr();
	CHECK_ERROR;
	if ((CUR == '/') && (NXT(1) == '/')) {
	    SKIP(2);
	    SKIP_BLANKS;

	    PUSH_LONG_EXPR(XPATH_OP_COLLECT, AXIS_DESCENDANT_OR_SELF,
		    NODE_TEST_TYPE, NODE_TYPE_NODE, NULL, NULL);
	    PUSH_UNARY_EXPR(XPATH_OP_RESET, this->comp->last, 1, 0);

	    CompRelativeLocationPath();
	} else if (CUR == '/') {
	    CompRelativeLocationPath();
	}
    }
    SKIP_BLANKS;
}

void mlXPathParserContext::CompUnionExpr() {
    CompPathExpr();
    CHECK_ERROR;
    SKIP_BLANKS;
    while (CUR == '|') {
	int op1 = this->comp->last;
	PUSH_LEAVE_EXPR(XPATH_OP_NODE, 0, 0);

	NEXT;
	SKIP_BLANKS;
	CompPathExpr();

	PUSH_BINARY_EXPR(XPATH_OP_UNION, op1, this->comp->last, 0, 0);

	SKIP_BLANKS;
    }
}

void mlXPathParserContext::CompUnaryExpr() {
    int minus = 0;
    int found = 0;

    SKIP_BLANKS;
    while (CUR == '-') {
        minus = 1 - minus;
	found = 1;
	NEXT;
	SKIP_BLANKS;
    }

    CompUnionExpr();
    CHECK_ERROR;
    if (found) {
	if (minus)
	    PUSH_UNARY_EXPR(XPATH_OP_PLUS, this->comp->last, 2, 0);
	else
	    PUSH_UNARY_EXPR(XPATH_OP_PLUS, this->comp->last, 3, 0);
    }
}

void mlXPathParserContext::CompMultiplicativeExpr() {
    CompUnaryExpr();
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '*') || 
           ((CUR == 'd') && (NXT(1) == 'i') && (NXT(2) == 'v')) ||
           ((CUR == 'm') && (NXT(1) == 'o') && (NXT(2) == 'd'))) {
	int op = -1;
	int op1 = this->comp->last;

    if (CUR == '*') {
	    op = 0;
	    NEXT;
	} else if (CUR == 'd') {
	    op = 1;
	    SKIP(3);
	} else if (CUR == 'm') {
	    op = 2;
	    SKIP(3);
	}
	SKIP_BLANKS;
        CompUnaryExpr();
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_MULT, op1, this->comp->last, op, 0);
	SKIP_BLANKS;
    }
}

void mlXPathParserContext::CompAdditiveExpr() {
    CompMultiplicativeExpr();
    CHECK_ERROR;
    SKIP_BLANKS;
    while ((CUR == '+') || (CUR == '-')) {
	int plus;
	int op1 = this->comp->last;

        if (CUR == '+') plus = 1;
	else plus = 0;
	NEXT;
	SKIP_BLANKS;
        CompMultiplicativeExpr();
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_PLUS, op1, this->comp->last, plus, 0);
	SKIP_BLANKS;
    }
}

void mlXPathParserContext::CompRelationalExpr() {
	CompAdditiveExpr();
	CHECK_ERROR;
	SKIP_BLANKS;
	while ((CUR == '<') ||
		   (CUR == '>') ||
		   ((CUR == '<') && (NXT(1) == '=')) ||
		   ((CUR == '>') && (NXT(1) == '='))) {
	int inf, strict;
	int op1 = this->comp->last;

		if (CUR == '<') inf = 1;
	else inf = 0;
	if (NXT(1) == '=') strict = 0;
	else strict = 1;
	NEXT;
	if (!strict) NEXT;
	SKIP_BLANKS;
		CompAdditiveExpr();
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_CMP, op1, this->comp->last, inf, strict);
	SKIP_BLANKS;
	}
}

void mlXPathParserContext::CompEqualityExpr()
{
	CompRelationalExpr();
	CHECK_ERROR;
	SKIP_BLANKS;
	while ((CUR == '=') || ((CUR == '!') && (NXT(1) == '='))) {
	int eq;
	int op1 = this->comp->last;

		if (CUR == '=') eq = 1;
	else eq = 0;
	NEXT;
	if (!eq) NEXT;
	SKIP_BLANKS;
		CompRelationalExpr();
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_EQUAL, op1, this->comp->last, eq, 0);
	SKIP_BLANKS;
	}
}

void mlXPathParserContext::CompAndExpr()
{
	CompEqualityExpr();
	CHECK_ERROR;
	SKIP_BLANKS;
	while ((CUR == 'a') && (NXT(1) == 'n') && (NXT(2) == 'd')) {
	int op1 = this->comp->last;
		SKIP(3);
	SKIP_BLANKS;
		CompEqualityExpr();
	CHECK_ERROR;
	PUSH_BINARY_EXPR(XPATH_OP_AND, op1, this->comp->last, 0, 0);
	SKIP_BLANKS;
	}
}

mlNodePtr mlXPathNodeSetDupNs(mlNodePtr node, mlNsPtr ns) {
    mlNsPtr cur;

    if ((ns == NULL) || (ns->type != XML_NAMESPACE_DECL))
		return(NULL);
    if ((node == NULL) || (node->nodeType == XML_NAMESPACE_DECL))
		return((mlNodePtr) ns);

    //
     // Allocate a new Namespace and fill the fields.
     
    MP_NEW_V( cur, mlNs, XML_NAMESPACE_DECL);
    if (ns->href != NULL)
		cur->href = Strdup(ns->href); 
    if (ns->prefix != NULL)
	cur->prefix = Strdup(ns->prefix); 
    cur->next = (mlNsPtr) node;
    return((mlNodePtr) cur);
}


void mlNodeSet::Add(mlNodePtr val){
	mlNodeVect::const_iterator cvi;
	for(cvi = nodeTab.begin(); cvi != nodeTab.end(); cvi++){
        if (*cvi == val) return;
	}
    if (val->nodeType == XML_NAMESPACE_DECL) {
		mlNsPtr ns = (mlNsPtr) val;

		nodeTab.push_back(mlXPathNodeSetDupNs((mlNodePtr) ns->next, ns));
    }else
		nodeTab.push_back(val);
}

#define CHECK_TYPE(typeval)						\
    if ((this->value == NULL) || (this->value->type != typeval))	\
        XP_ERROR(XPATH_INVALID_TYPE)

#define CHECK_TYPE0(typeval)						\
    if ((this->value == NULL) || (this->value->type != typeval))	\
        XP_ERROR0(XPATH_INVALID_TYPE)

#define CHECK_ARITY(x)							\
    if (nargs != (x))							\
        XP_ERROR(XPATH_INVALID_ARITY);

void mlXPathNodeSetAdd(mlNodeSetPtr cur, mlNodePtr val) {
	cur->Add(val);
}

const xmlChar * mlXPathNsLookup(mlXPathContext* ctxt, const xmlChar *prefix) {
    if (ctxt == NULL)
		return(NULL);
    //if (prefix == NULL)	return(NULL);   // Pathed on 29.08.2005 by BDima to fix namespace bug

#ifdef XML_XML_NAMESPACE
    if (xmlStrEqual(prefix, (const xmlChar *) "xml"))
		return(XML_XML_NAMESPACE);
#endif

    if (ctxt->namespaces != NULL) {
		int i;

		for (i = 0;i < ctxt->nsNr;i++) {
			if ((ctxt->namespaces[i] != NULL) &&   // Pathed on 29.08.2005 by BDima to fix namespace bug
			    ((ctxt->namespaces[i]->prefix == NULL || *ctxt->namespaces[i]->prefix == 0) && 
				 (prefix == NULL || *prefix == 0)) ||
			    (isEqual(ctxt->namespaces[i]->prefix, prefix)))
			return(ctxt->namespaces[i]->href);
		}
    }

    return((const xmlChar *) ctxt->nsHash->Lookup(prefix));
}

typedef mlXPathParserContext* mlXPathParserContextPtr;
typedef mlXMLAttr* mlAttrPtr;

mlNodePtr mlXPathNextAncestor(mlXPathParserContext* ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    //
     // the parent of an attribute or namespace node is the element
     // to which the attribute or namespace node is attached
     // !!!!!!!!!!!!!
     
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->nodeType) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
	    case XML_DTD_NODE:
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_ENTITY_DECL:
            case XML_NOTATION_NODE:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		if (ctxt->context->node->GetParentNode() == NULL)
		    return((mlNodePtr) ctxt->context->doc);
		if ((ctxt->context->node->GetParentNode()->nodeType == XML_ELEMENT_NODE) &&
		    ((ctxt->context->node->GetParentNode()->GetNodeName()[0] == ' ') ||
		     (xmlStrEqual(ctxt->context->node->GetParentNode()->GetNodeName(),
				 L"fake node libxslt"))))
		    return(NULL);
		return(ctxt->context->node->GetParentNode());
            case XML_ATTRIBUTE_NODE: {
		mlXMLAttr* tmp = (mlXMLAttr*) ctxt->context->node;

		return(tmp->GetParentNode());
	    }
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
                return(NULL);
	    case XML_NAMESPACE_DECL: {
		mlNsPtr ns = (mlNsPtr) ctxt->context->node;
		
		if ((ns->next != NULL) &&
		    (ns->next->type != XML_NAMESPACE_DECL))
		    return((mlNodePtr) ns->next);
		// Bad, how did that namespace end up here ?
                return(NULL);
	    }
	}
	return(NULL);
    }
    if (cur == ctxt->context->doc->GetFirstChildNode())
	return((mlNodePtr) ctxt->context->doc);
    if (cur == (mlNodePtr) ctxt->context->doc)
	return(NULL);
    switch (cur->nodeType) {
	case XML_ELEMENT_NODE:
	case XML_TEXT_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_ENTITY_REF_NODE:
	case XML_ENTITY_NODE:
	case XML_PI_NODE:
	case XML_COMMENT_NODE:
	case XML_NOTATION_NODE:
	case XML_DTD_NODE:
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
	case XML_XINCLUDE_START:
	case XML_XINCLUDE_END:
	    if (cur->GetParentNode() == NULL)
		return(NULL);
	    if ((cur->GetParentNode()->nodeType == XML_ELEMENT_NODE) &&
		((cur->GetParentNode()->GetNodeName()[0] == ' ') ||
		 (xmlStrEqual(cur->GetParentNode()->GetNodeName(),
			      L"fake node libxslt"))))
		return(NULL);
	    return(cur->GetParentNode());
	case XML_ATTRIBUTE_NODE: {
	    mlAttrPtr att = (mlAttrPtr) ctxt->context->node;

	    return(att->GetParentNode());
	}
	case XML_NAMESPACE_DECL: {
	    mlNsPtr ns = (mlNsPtr) ctxt->context->node;
		
	    if ((ns->next != NULL) &&
	        (ns->next->type != XML_NAMESPACE_DECL))
	        return((mlNodePtr) ns->next);
	    // Bad, how did that namespace end up here ?
            return(NULL);
	}
	case XML_DOCUMENT_NODE:
	case XML_DOCUMENT_TYPE_NODE:
	case XML_DOCUMENT_FRAG_NODE:
	case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	case XML_DOCB_DOCUMENT_NODE:
#endif
	    return(NULL);
    }
    return(NULL);
}

mlNodePtr mlXPathNextChild(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->nodeType) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
            case XML_NOTATION_NODE:
            case XML_DTD_NODE:
		return(ctxt->context->node->GetFirstChildNode());
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
		return(((mlXMLDocument*) ctxt->context->node)->GetFirstChildNode());
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_ENTITY_DECL:
            case XML_ATTRIBUTE_NODE:
	    case XML_NAMESPACE_DECL:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
		return(NULL);
	}
	return(NULL);
    }
    if ((cur->nodeType == XML_DOCUMENT_NODE) ||
        (cur->nodeType == XML_HTML_DOCUMENT_NODE))
	return(NULL);
    return(cur->GetNextSiblingNode());
}

mlNodePtr mlXPathNextAncestorOrSelf(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL)
        return(ctxt->context->node);
    return(mlXPathNextAncestor(ctxt, cur));
}

mlNodePtr mlXPathNextAttribute(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (ctxt->context->node == NULL)
	return(NULL);
    if (ctxt->context->node->nodeType != XML_ELEMENT_NODE)
	return(NULL);
    if (cur == NULL) {
        if (ctxt->context->node == (mlNodePtr) ctxt->context->doc)
	    return(NULL);
        return((mlNodePtr)ctxt->context->node->GetFirstAttrNode());
    }
    return((mlNodePtr)cur->GetNextSiblingNode());
}

mlNodePtr mlXPathNextDescendant(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
	if (ctxt->context->node == NULL)
	    return(NULL);
	if ((ctxt->context->node->nodeType == XML_ATTRIBUTE_NODE) ||
	    (ctxt->context->node->nodeType == XML_NAMESPACE_DECL))
	    return(NULL);

        if (ctxt->context->node == (mlNodePtr) ctxt->context->doc)
			return(ctxt->context->doc->GetFirstChildNode());
        return(ctxt->context->node->GetFirstChildNode());
    }

    if (cur->GetFirstChildNode() != NULL) {
	//
	 // Do not descend on entities declarations
	 
    	if (cur->GetFirstChildNode()->nodeType != XML_ENTITY_DECL) {
	    cur = cur->GetFirstChildNode();
	    //
	     // Skip DTDs
	     
	    if (cur->nodeType != XML_DTD_NODE)
			return(cur);
	}
    }

    if (cur == ctxt->context->node) return(NULL);

    while (cur->GetNextSiblingNode() != NULL) {
		cur = cur->GetNextSiblingNode();
		if ((cur->nodeType != XML_ENTITY_DECL) &&
			(cur->nodeType != XML_DTD_NODE))
			return(cur);
    }
    
    do {
        cur = cur->GetParentNode();
	if (cur == NULL) return(NULL);
	if (cur == ctxt->context->node) return(NULL);
	if (cur->GetNextSiblingNode() != NULL) {
	    cur = cur->GetNextSiblingNode();
	    return(cur);
	}
    } while (cur != NULL);
    return(cur);
}

mlNodePtr mlXPathNextDescendantOrSelf(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
	if (ctxt->context->node == NULL)
	    return(NULL);
	if ((ctxt->context->node->nodeType == XML_ATTRIBUTE_NODE) ||
	    (ctxt->context->node->nodeType == XML_NAMESPACE_DECL))
	    return(NULL);
        return(ctxt->context->node);
    }

    return(mlXPathNextDescendant(ctxt, cur));
}

mlNodePtr mlXPathNextFollowing(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur != NULL && cur->GetFirstChildNode() != NULL)
        return cur->GetFirstChildNode();
    if (cur == NULL) cur = ctxt->context->node;
    if (cur == NULL) return(NULL) ; // ERROR
    if (cur->GetNextSiblingNode() != NULL) return(cur->GetNextSiblingNode()) ;
    do {
        cur = cur->GetParentNode();
        if (cur == NULL) return(NULL);
        if (cur == (mlNodePtr) ctxt->context->doc) return(NULL);
        if (cur->GetNextSiblingNode() != NULL) return(cur->GetNextSiblingNode());
    } while (cur != NULL);
    return(cur);
}

mlNodePtr mlXPathNextFollowingSibling(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if ((ctxt->context->node->nodeType == XML_ATTRIBUTE_NODE) ||
	(ctxt->context->node->nodeType == XML_NAMESPACE_DECL))
	return(NULL);
    if (cur == (mlNodePtr) ctxt->context->doc)
        return(NULL);
    if (cur == NULL)
        return(ctxt->context->node->GetNextSiblingNode());
    return(cur->GetNextSiblingNode());
}

mlNsPtr* mlGetNsList(mlXMLDocument* doc, mlNodePtr node)
{
//    mlNsPtr cur;
    mlNsPtr *ret = NULL;
    int nbns = 0;
    int maxns = 10;
//    int i;

//    while (node != NULL) {
//        if (node->nodeType == XML_ELEMENT_NODE) {
//            cur = node->nsDef;
//            while (cur != NULL) {
//                if (ret == NULL) {
//                    ret = new mlNs[maxns + 1];
//                    if (ret == NULL) {
//						mlXPathErrMemory("getting namespace list");
//                        return (NULL);
//                    }
//                    ret[nbns] = NULL;
//                }
//                for (i = 0; i < nbns; i++) {
//                    if ((cur->prefix == ret[i]->prefix) ||
//                        (xmlStrEqual(cur->prefix, ret[i]->prefix)))
//                        break;
//                }
//                if (i >= nbns) {
//                    if (nbns >= maxns) {
//                        maxns *= 2;
//                        ret = (xmlNsPtr *) xmlRealloc(ret,
//                                                      (maxns +
//                                                       1) *
//                                                      sizeof(xmlNsPtr));
//                        if (ret == NULL) {
//							mlXPathErrMemory("getting namespace list");
//                            return (NULL);
//                        }
//                    }
//                    ret[nbns++] = cur;
//                    ret[nbns] = NULL;
//                }
//
//                cur = cur->next;
//            }
//        }
//        node = node->parent;
//    }
    return (ret);
}

static mlNs mlXPathXMLNamespaceStruct;  // = {
//    NULL,
//    XML_NAMESPACE_DECL,
//    XML_XML_NAMESPACE,
//    L"xml",
//    NULL
//};
static mlNsPtr mlXPathXMLNamespace = &mlXPathXMLNamespaceStruct;

mlNodePtr mlXPathNextNamespace(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (ctxt->context->node->nodeType != XML_ELEMENT_NODE) return(NULL);
//    if (ctxt->context->tmpNsList == NULL && cur != (mlNodePtr) mlXPathXMLNamespace) {
//        if (ctxt->context->tmpNsList != NULL)
//	    xmlFree(ctxt->context->tmpNsList);
//	ctxt->context->tmpNsList = 
//	    mlGetNsList(ctxt->context->doc, ctxt->context->node);
//	ctxt->context->tmpNsNr = 0;
//	if (ctxt->context->tmpNsList != NULL) {
//	    while (ctxt->context->tmpNsList[ctxt->context->tmpNsNr] != NULL) {
//		ctxt->context->tmpNsNr++;
//	    }
//	}
//	return((mlNodePtr) mlXPathXMLNamespace);
//    }
//    if (ctxt->context->tmpNsNr > 0) {
//	return (xmlNodePtr)ctxt->context->tmpNsList[--ctxt->context->tmpNsNr];
//    } else {
//	if (ctxt->context->tmpNsList != NULL)
//	    xmlFree(ctxt->context->tmpNsList);
//	ctxt->context->tmpNsList = NULL;
//	return(NULL);
//    }
return NULL;
}

mlNodePtr mlXPathNextParent(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    //
     // the parent of an attribute or namespace node is the element
     // to which the attribute or namespace node is attached
     // Namespace handling !!!
     
    if (cur == NULL) {
	if (ctxt->context->node == NULL) return(NULL);
	switch (ctxt->context->node->nodeType) {
            case XML_ELEMENT_NODE:
            case XML_TEXT_NODE:
            case XML_CDATA_SECTION_NODE:
            case XML_ENTITY_REF_NODE:
            case XML_ENTITY_NODE:
            case XML_PI_NODE:
            case XML_COMMENT_NODE:
            case XML_NOTATION_NODE:
            case XML_DTD_NODE:
	    case XML_ELEMENT_DECL:
	    case XML_ATTRIBUTE_DECL:
	    case XML_XINCLUDE_START:
	    case XML_XINCLUDE_END:
	    case XML_ENTITY_DECL:
		if (ctxt->context->node->GetParentNode() == NULL)
		    return((mlNodePtr) ctxt->context->doc);
		if ((ctxt->context->node->GetParentNode()->nodeType == XML_ELEMENT_NODE) &&
		    ((ctxt->context->node->GetParentNode()->GetNodeName()[0] == ' ') ||
		     (xmlStrEqual(ctxt->context->node->GetParentNode()->GetNodeName(),
				 L"fake node libxslt"))))
		    return(NULL);
		return(ctxt->context->node->GetParentNode());
            case XML_ATTRIBUTE_NODE: {
		mlAttrPtr att = (mlAttrPtr) ctxt->context->node;

		return(att->GetParentNode());
	    }
            case XML_DOCUMENT_NODE:
            case XML_DOCUMENT_TYPE_NODE:
            case XML_DOCUMENT_FRAG_NODE:
            case XML_HTML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
	    case XML_DOCB_DOCUMENT_NODE:
#endif
                return(NULL);
	    case XML_NAMESPACE_DECL: {
		mlNsPtr ns = (mlNsPtr) ctxt->context->node;
		
		if ((ns->next != NULL) &&
		    (ns->next->type != XML_NAMESPACE_DECL))
		    return((mlNodePtr) ns->next);
                return(NULL);
	    }
	}
    }
    return(NULL);
}

mlNodePtr mlXPathNextPrecedingInternal(mlXPathParserContextPtr ctxt, mlNodePtr cur)
{
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL) {
        cur = ctxt->context->node;
        if (cur == NULL)
            return (NULL);
	if (cur->nodeType == XML_NAMESPACE_DECL)
	    cur = (mlNodePtr)((mlNsPtr)cur)->next;
        ctxt->ancestor = cur->GetParentNode();
    }
    if ((cur->GetPrevSiblingNode() != NULL) && (cur->GetPrevSiblingNode()->nodeType == XML_DTD_NODE))
	cur = cur->GetPrevSiblingNode();
    while (cur->GetPrevSiblingNode() == NULL) {
        cur = cur->GetParentNode();
        if (cur == NULL)
            return (NULL);
        if (cur == ctxt->context->doc->GetFirstChildNode())
            return (NULL);
        if (cur != ctxt->ancestor)
            return (cur);
        ctxt->ancestor = cur->GetParentNode();
    }
    cur = cur->GetPrevSiblingNode();
    while (cur->GetLastChildNode() != NULL)
        cur = cur->GetLastChildNode();
    return (cur);
}

mlNodePtr mlXPathNextPrecedingSibling(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if ((ctxt->context->node->nodeType == XML_ATTRIBUTE_NODE) ||
	(ctxt->context->node->nodeType == XML_NAMESPACE_DECL))
	return(NULL);
    if (cur == (mlNodePtr) ctxt->context->doc)
        return(NULL);
    if (cur == NULL)
        return(ctxt->context->node->GetPrevSiblingNode());
    if ((cur->GetPrevSiblingNode() != NULL) && (cur->GetPrevSiblingNode()->nodeType == XML_DTD_NODE)) {
		cur = cur->GetPrevSiblingNode();
	if (cur == NULL)
	    return(ctxt->context->node->GetPrevSiblingNode());
    }
    return(cur->GetPrevSiblingNode());
}

mlNodePtr mlXPathNextSelf(mlXPathParserContextPtr ctxt, mlNodePtr cur) {
    if ((ctxt == NULL) || (ctxt->context == NULL)) return(NULL);
    if (cur == NULL)
        return(ctxt->context->node);
    return(NULL);
}

mlXPathObjectPtr mlXPathWrapNodeSet(mlNodeSetPtr val) {
    mlXPathObjectPtr ret;

	ret = MP_NEW( mlXPathObject);
    ret->type = XPATH_NODESET;
    ret->nodesetval = val;
    return(ret);
}

void mlXPathNodeSetAddUnique(mlNodeSetPtr cur, mlNodePtr val) {
    if ((cur == NULL) || (val == NULL)) return;

    // @@ with_ns to check whether namespace nodes should be looked at @@
    //
     
    if (val->nodeType == XML_NAMESPACE_DECL) {
		mlNsPtr ns = (mlNsPtr) val;

		cur->nodeTab.push_back(mlXPathNodeSetDupNs((mlNodePtr) ns->next, ns));
    } else
		cur->nodeTab.push_back(val);
}

mlNodeSetPtr mlXPathNodeSetCreate(mlNodePtr val) {
    mlNodeSetPtr ret;

	ret = MP_NEW( mlNodeSet);
    if (val != NULL) {
		if (val->nodeType == XML_NAMESPACE_DECL) {
			mlNsPtr ns = (mlNsPtr) val;

			ret->nodeTab.push_back(mlXPathNodeSetDupNs((mlNodePtr) ns->next, ns));
		} else
			ret->nodeTab.push_back(val);
    }
    return(ret);
}

int mlXPathCmpNodes(mlNodePtr node1, mlNodePtr node2) {
    int depth1, depth2;
    int attr1 = 0, attr2 = 0;
    mlNodePtr attrNode1 = NULL, attrNode2 = NULL;
    mlNodePtr cur, root;

    if ((node1 == NULL) || (node2 == NULL))
		return(-2);
    //
     // a couple of optimizations which will avoid computations in most cases
     
    if (node1->nodeType == XML_ATTRIBUTE_NODE) {
		attr1 = 1;
		attrNode1 = node1;
		node1 = node1->GetParentNode();
    }
    if (node2->nodeType == XML_ATTRIBUTE_NODE) {
		attr2 = 1;
		attrNode2 = node2;
		node2 = node2->GetParentNode();
    }
    if (node1 == node2) {
		if (attr1 == attr2) {
			// not required, but we keep attributes in order
			if (attr1 != 0) {
				cur = attrNode2->GetPrevSiblingNode();
			while (cur != NULL) {
				if (cur == attrNode1)
					return (1);
				cur = cur->GetPrevSiblingNode();
			}
			return (-1);
			}
			return(0);
		}
		if (attr2 == 1)
			return(1);
		return(-1);
    }
    if ((node1->nodeType == XML_NAMESPACE_DECL) ||
        (node2->nodeType == XML_NAMESPACE_DECL))
		return(1);
    if (node1 == node2->GetPrevSiblingNode())
		return(1);
    if (node1 == node2->GetNextSiblingNode())
		return(-1);

    //
     // Speedup using document order if available.
     
    if ((node1->nodeType == XML_ELEMENT_NODE) &&
	(node2->nodeType == XML_ELEMENT_NODE) &&
	(0 > (long) node1->GetContent()) &&
	(0 > (long) node2->GetContent()) &&
	(node1->GetDocumentNode() == node2->GetDocumentNode())) {
	long l1, l2;

	l1 = -((long) node1->GetContent());
	l2 = -((long) node2->GetContent());
	if (l1 < l2)
	    return(1);
	if (l1 > l2)
	    return(-1);
    }

    //
     // compute depth to root
     
    for (depth2 = 0, cur = node2;cur->GetParentNode() != NULL;cur = cur->GetParentNode()) {
		if (cur == node1)
		    return(1);
		depth2++;
    }
    root = cur;
    for (depth1 = 0, cur = node1;cur->GetParentNode() != NULL;cur = cur->GetParentNode()) {
		if (cur == node2)
			return(-1);
		depth1++;
    }
    //
     // Distinct document (or distinct entities :-( ) case.
     
    if (root != cur) {
		return(-2);
    }
    //
     // get the nearest common ancestor.
     
    while (depth1 > depth2) {
		depth1--;
		node1 = node1->GetParentNode();
    }
    while (depth2 > depth1) {
		depth2--;
		node2 = node2->GetParentNode();
    }
    while (node1->GetParentNode() != node2->GetParentNode()) {
		node1 = node1->GetParentNode();
		node2 = node2->GetParentNode();
		// should not happen but just in case ...
		if ((node1 == NULL) || (node2 == NULL))
			return(-2);
    }
    //
     // Find who's first.
     
    if (node1 == node2->GetPrevSiblingNode())
		return(1);
    if (node1 == node2->GetNextSiblingNode())
		return(-1);
    //
     // Speedup using document order if availble.
     
    if ((node1->nodeType == XML_ELEMENT_NODE) &&
	(node2->nodeType == XML_ELEMENT_NODE) &&
	(0 > (long) node1->GetContent()) &&
	(0 > (long) node2->GetContent()) &&
	(node1->GetDocumentNode() == node2->GetDocumentNode())) {
		long l1, l2;

		l1 = -((long) node1->GetContent());
		l2 = -((long) node2->GetContent());
		if (l1 < l2)
			return(1);
		if (l1 > l2)
			return(-1);
    }

    for (cur = node1->GetNextSiblingNode();cur != NULL;cur = cur->GetNextSiblingNode())
	if (cur == node2)
	    return(1);
    return(-1); // assume there is no sibling list corruption
}

void mlXPathNodeSetAddNs(mlNodeSetPtr cur, mlNodePtr node, mlNsPtr ns) {
    int i;

    
    if ((cur == NULL) || (ns == NULL) || (node == NULL) ||
        (ns->type != XML_NAMESPACE_DECL) ||
	(node->nodeType != XML_ELEMENT_NODE))
	return;

    // @@ with_ns to check whether namespace nodes should be looked at @@
    //
     // prevent duplicates
     
    for (i = 0;i < cur->GetSize();i++) {
        if ((cur->nodeTab[i] != NULL) &&
	    (cur->nodeTab[i]->nodeType == XML_NAMESPACE_DECL) &&
	    (((mlNsPtr)cur->nodeTab[i])->next == (mlNsPtr) node) &&
	    (xmlStrEqual(ns->prefix, ((mlNsPtr)cur->nodeTab[i])->prefix)))
	    return;
    }

    cur->nodeTab.push_back(mlXPathNodeSetDupNs(node, ns));
}

typedef mlNodePtr (*mlXPathTraversalFunction)
                    (mlXPathParserContext* ctxt, mlNodePtr cur);

int mlXPathParserContext::NodeCollectAndTest(mlXPathStepOp* op,
			   mlNodePtr * first, mlNodePtr * last)
{
    mlXPathAxisVal axis = (mlXPathAxisVal) op->value;
    mlXPathTestVal test = (mlXPathTestVal) op->value2;
    mlXPathTypeVal type = (mlXPathTypeVal) op->value3;
    const xmlChar *prefix = (const xmlChar *)op->value4;
    const xmlChar *name = (const xmlChar *)op->value5;
    const xmlChar *URI = (const xmlChar *)NULL;

#ifdef DEBUG_STEP
    int n = 0;
#endif
    int i, t = 0;
    mlNodeSetPtr ret, list;
    mlXPathTraversalFunction next = NULL;
    void (*addNode) (mlNodeSetPtr, mlNodePtr);
    mlNodeSetPtr (*mergeNodeSet) (mlNodeSetPtr, mlNodeSetPtr);
    mlNodePtr cur = NULL;
    mlXPathObjectPtr obj;
    mlNodeSetPtr nodelist;
    mlNodePtr tmp;

    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop();
    addNode = mlXPathNodeSetAdd;
    mergeNodeSet = mlXPathNodeSetMerge;
	URI = this->context->NsLookup(prefix); // Pathed on 29.08.2005 by BDima to fix namespace bug
    if (prefix != NULL) {
        if (URI == NULL)
            XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
    } else if (URI != NULL)
		prefix = (xmlChar*)"";
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext, "new step : ");
#endif
    switch (axis) {
        case AXIS_ANCESTOR:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'ancestors' ");
#endif
            first = NULL;
            next = mlXPathNextAncestor;
            break;
        case AXIS_ANCESTOR_OR_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'ancestors-or-self' ");
#endif
            first = NULL;
            next = mlXPathNextAncestorOrSelf;
            break;
        case AXIS_ATTRIBUTE:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'attributes' ");
#endif
            first = NULL;
	    last = NULL;
            next = mlXPathNextAttribute;
	    mergeNodeSet = mlXPathNodeSetMergeUnique;
            break;
        case AXIS_CHILD:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'child' ");
#endif
	    last = NULL;
            next = mlXPathNextChild;
	    mergeNodeSet = mlXPathNodeSetMergeUnique;
            break;
        case AXIS_DESCENDANT:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'descendant' ");
#endif
	    last = NULL;
            next = mlXPathNextDescendant;
            break;
        case AXIS_DESCENDANT_OR_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'descendant-or-self' ");
#endif
	    last = NULL;
            next = mlXPathNextDescendantOrSelf;
            break;
        case AXIS_FOLLOWING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'following' ");
#endif
	    last = NULL;
            next = mlXPathNextFollowing;
            break;
        case AXIS_FOLLOWING_SIBLING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'following-siblings' ");
#endif
	    last = NULL;
            next = mlXPathNextFollowingSibling;
            break;
        case AXIS_NAMESPACE:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'namespace' ");
#endif
            first = NULL;
	    last = NULL;
            next = (mlXPathTraversalFunction) mlXPathNextNamespace;
	    mergeNodeSet = mlXPathNodeSetMergeUnique;
            break;
        case AXIS_PARENT:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'parent' ");
#endif
            first = NULL;
            next = mlXPathNextParent;
            break;
        case AXIS_PRECEDING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'preceding' ");
#endif
            first = NULL;
            next = mlXPathNextPrecedingInternal;
            break;
        case AXIS_PRECEDING_SIBLING:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'preceding-sibling' ");
#endif
            first = NULL;
            next = mlXPathNextPrecedingSibling;
            break;
        case AXIS_SELF:
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, "axis 'self' ");
#endif
            first = NULL;
	    last = NULL;
            next = mlXPathNextSelf;
	    mergeNodeSet = mlXPathNodeSetMergeUnique;
            break;
    }
    if (next == NULL)
        return(0);

    nodelist = obj->nodesetval;
    if (nodelist == NULL) {
        delete(obj);
        valuePush(mlXPathWrapNodeSet(NULL));
        return(0);
    }
    addNode = mlXPathNodeSetAddUnique;
    ret = NULL;
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext,
                    " context contains %d nodes\n", nodelist->nodeNr);
    switch (test) {
        case NODE_TEST_NONE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for none !!!\n");
            break;
        case NODE_TEST_TYPE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for type %d\n", type);
            break;
        case NODE_TEST_PI:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for PI !!!\n");
            break;
        case NODE_TEST_ALL:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for *\n");
            break;
        case NODE_TEST_NS:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for namespace %s\n",
                            prefix);
            break;
        case NODE_TEST_NAME:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for name %s\n", name);
            if (prefix != NULL)
                xmlGenericError(xmlGenericErrorContext,
                                "           with namespace %s\n", prefix);
            break;
    }
    xmlGenericError(xmlGenericErrorContext, "Testing : ");
#endif
    /*
     * 2.3 Node Tests
     *  - For the attribute axis, the principal node type is attribute. 
     *  - For the namespace axis, the principal node type is namespace. 
     *  - For other axes, the principal node type is element. 
     *
     * A node test * is true for any node of the
     * principal node type. For example, child::* will
     * select all element children of the context node
     */
    tmp = this->context->node;
    for (i = 0; i < nodelist->GetSize(); i++) {
        this->context->node = nodelist->nodeTab[i];

        cur = NULL;
        list = mlXPathNodeSetCreate(NULL);
        do {
            cur = next(this, cur);
            if (cur == NULL)
                break;
            if ((first != NULL) && (*first == cur))
                break;
	    if (((t % 256) == 0) &&
	        (first != NULL) && (*first != NULL) &&
		(mlXPathCmpNodes(*first, cur) >= 0))
		break;
	    if ((last != NULL) && (*last == cur))
		break;
	    if (((t % 256) == 0) &&
		(last != NULL) && (*last != NULL) &&
		(mlXPathCmpNodes(cur, *last) >= 0))
		break;
            t++;
#ifdef DEBUG_STEP
            xmlGenericError(xmlGenericErrorContext, " %s", cur->name);
#endif
            switch (test) {
                case NODE_TEST_NONE:
                    this->context->node = tmp;
                    STRANGE return(t);
                case NODE_TEST_TYPE:
                    if ((cur->nodeType == type) ||
                        ((type == NODE_TYPE_NODE) &&
                         ((cur->nodeType == XML_DOCUMENT_NODE) ||
                          (cur->nodeType == XML_HTML_DOCUMENT_NODE) ||
                          (cur->nodeType == XML_ELEMENT_NODE) ||
                          (cur->nodeType == XML_NAMESPACE_DECL) ||
                          (cur->nodeType == XML_ATTRIBUTE_NODE) ||
                          (cur->nodeType == XML_PI_NODE) ||
                          (cur->nodeType == XML_COMMENT_NODE) ||
                          (cur->nodeType == XML_CDATA_SECTION_NODE) ||
                          (cur->nodeType == XML_TEXT_NODE))) ||
			((type == NODE_TYPE_TEXT) &&
			 (cur->nodeType == XML_CDATA_SECTION_NODE))) {
#ifdef DEBUG_STEP
                        n++;
#endif
                        addNode(list, cur);
                    }
                    break;
                case NODE_TEST_PI:
                    if (cur->nodeType == XML_PI_NODE) {
                        if ((name != NULL) &&
                            (!xmlStrEqual(name, cur->GetNodeName())))
                            break;
#ifdef DEBUG_STEP
                        n++;
#endif
                        addNode(list, cur);
                    }
                    break;
                case NODE_TEST_ALL:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->nodeType == XML_ATTRIBUTE_NODE) {
#ifdef DEBUG_STEP
                            n++;
#endif
                            addNode(list, cur);
                        }
                    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->nodeType == XML_NAMESPACE_DECL) {
#ifdef DEBUG_STEP
                            n++;
#endif
                            mlXPathNodeSetAddNs(list, this->context->node, 
				                 (mlNsPtr) cur);
                        }
                    } else {
                        if (cur->nodeType == XML_ELEMENT_NODE) {
                            if (prefix == NULL) {
#ifdef DEBUG_STEP
                                n++;
#endif
                                addNode(list, cur);
                            } else if ((cur->ns != NULL) &&
                                       (xmlStrEqual(URI, cur->ns->href))) {
#ifdef DEBUG_STEP
                                n++;
#endif
                                addNode(list, cur);
                            }
                        }
                    }
                    break;
                case NODE_TEST_NS:{
                        TODO;
                        break;
                    }
                case NODE_TEST_NAME:
                    switch (cur->nodeType) {
                        case XML_ELEMENT_NODE:
                            if (xmlStrEqual(name, cur->GetNodeName())) {
                                if (prefix == NULL) {
                                    if (cur->ns == NULL) {
#ifdef DEBUG_STEP
                                        n++;
#endif
                                        addNode(list, cur);
                                    }
                                } else {
                                    if ((cur->ns != NULL) &&
                                        (xmlStrEqual(URI,
                                                     cur->ns->href))) {
#ifdef DEBUG_STEP
                                        n++;
#endif
                                        addNode(list, cur);
                                    }
                                }
                            }
                            break;
                        case XML_ATTRIBUTE_NODE:{
                                mlAttrPtr attr = (mlAttrPtr) cur;

                                if (xmlStrEqual(name, attr->GetNodeName())) {
                                    if (prefix == NULL || *prefix == 0) {
                                        if ((attr->ns == NULL) ||
                                            (attr->ns->prefix == NULL)) {
#ifdef DEBUG_STEP
                                            n++;
#endif
                                            addNode(list,
                                                    (mlNodePtr) attr);
                                        }
                                    } else {
                                        if ((attr->ns != NULL) &&
                                            (xmlStrEqual(URI,
                                                         attr->ns->
                                                         href))) {
#ifdef DEBUG_STEP
                                            n++;
#endif
                                            addNode(list,
                                                    (mlNodePtr) attr);
                                        }
                                    }
                                }
                                break;
                            }
                        case XML_NAMESPACE_DECL:
                            if (cur->nodeType == XML_NAMESPACE_DECL) {
                                mlNsPtr ns = (mlNsPtr) cur;

                                if ((ns->prefix != NULL) && (name != NULL)
                                    && (xmlStrEqual(ns->prefix, name))) {
#ifdef DEBUG_STEP
                                    n++;
#endif
				    mlXPathNodeSetAddNs(list,
					this->context->node, (mlNsPtr) cur);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    break;
            }
        } while (cur != NULL);

        //
         // If there is some predicate filtering do it now
         
        if ((op->ch2 != -1) && (list != NULL) && (list->GetSize() > 0)) {
            mlXPathObjectPtr obj2;

            valuePush(mlXPathWrapNodeSet(list));
            CompOpEval(&this->comp->steps[op->ch2]);
            CHECK_TYPE0(XPATH_NODESET);
            obj2 = valuePop();
            list = obj2->nodesetval;
            obj2->nodesetval = NULL;
            MP_DELETE( obj2);
        }
        if (ret == NULL) {
            ret = list;
        } else {
            ret = mergeNodeSet(ret, list);
            MP_DELETE( list);
        }
    }
    this->context->node = tmp;
#ifdef DEBUG_STEP
    xmlGenericError(xmlGenericErrorContext,
                    "\nExamined %d nodes, found %d nodes at that step\n",
                    t, n);
#endif
    valuePush(mlXPathWrapNodeSet(ret));
    if ((obj->boolval) && (obj->user != NULL)) {
	this->value->boolval = 1;
	this->value->user = obj->user;
	obj->user = NULL;
	obj->boolval = 0;
    }
    delete(obj);
    return(t);
}

mlXPathObjectPtr mlXPathVariableLookupNS(mlXPathContext* ctxt, const xmlChar *name,
			 const xmlChar *ns_uri) {
    if (ctxt == NULL)
	return(NULL);

    if (ctxt->varLookupFunc != NULL) {
	mlXPathObjectPtr ret;

	ret = ((mlXPathVariableLookupFunc)ctxt->varLookupFunc)
	        (ctxt->varLookupData, name, ns_uri);
	if (ret != NULL) return(ret);
    }

    if (ctxt->varHash == NULL)
	return(NULL);
    if (name == NULL)
	return(NULL);

    return(mlXPathObjectCopy((xmlXPathObjectPtr)ctxt->varHash->Lookup2(name, ns_uri)));
}

mlXPathObjectPtr mlXPathVariableLookup(mlXPathContext* ctxt, const xmlChar *name) {
    if (ctxt == NULL)
	return(NULL);

    if (ctxt->varLookupFunc != NULL) {
	mlXPathObjectPtr ret;

	ret = ((mlXPathVariableLookupFunc)ctxt->varLookupFunc)
	        (ctxt->varLookupData, name, NULL);
	return(ret);
    }
    return(mlXPathVariableLookupNS(ctxt, name, NULL));
}

int mlXPathParserContext::NodeCollectAndTestNth(
                              mlXPathStepOp* op, int indx,
                              mlNodePtr* first, mlNodePtr* last)
{
    mlXPathAxisVal axis = (mlXPathAxisVal) op->value;
    mlXPathTestVal test = (mlXPathTestVal) op->value2;
    mlXPathTypeVal type = (mlXPathTypeVal) op->value3;
    const xmlChar *prefix = (const wchar_t*)op->value4;
    const xmlChar *name = (const wchar_t*)op->value5;
    const xmlChar *URI = NULL;
    int n = 0, t = 0;

    int i;
    mlNodeSetPtr list;
    mlXPathTraversalFunction next = NULL;
    void (*addNode) (mlNodeSetPtr, mlNodePtr);
    mlNodePtr cur = NULL;
    mlXPathObjectPtr obj;
    mlNodeSetPtr nodelist;
    mlNodePtr tmp;

    CHECK_TYPE0(XPATH_NODESET);
    obj = valuePop();
    addNode = mlXPathNodeSetAdd;
    if (prefix != NULL) {
        URI = mlXPathNsLookup(this->context, prefix);
        if (URI == NULL)
            XP_ERROR0(XPATH_UNDEF_PREFIX_ERROR);
    }
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext, "new step : ");
    if (first != NULL) {
	if (*first != NULL)
	    xmlGenericError(xmlGenericErrorContext, "first = %s ",
		    (*first)->name);
	else
	    xmlGenericError(xmlGenericErrorContext, "first = NULL ");
    }
    if (last != NULL) {
	if (*last != NULL)
	    xmlGenericError(xmlGenericErrorContext, "last = %s ",
		    (*last)->name);
	else
	    xmlGenericError(xmlGenericErrorContext, "last = NULL ");
    }
#endif
    switch (axis) {
        case AXIS_ANCESTOR:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'ancestors' ");
#endif
            first = NULL;
            next = mlXPathNextAncestor;
            break;
        case AXIS_ANCESTOR_OR_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'ancestors-or-self' ");
#endif
            first = NULL;
            next = mlXPathNextAncestorOrSelf;
            break;
        case AXIS_ATTRIBUTE:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'attributes' ");
#endif
            first = NULL;
	    last = NULL;
            next = mlXPathNextAttribute;
            break;
        case AXIS_CHILD:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'child' ");
#endif
	    last = NULL;
            next = mlXPathNextChild;
            break;
        case AXIS_DESCENDANT:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'descendant' ");
#endif
	    last = NULL;
            next = mlXPathNextDescendant;
            break;
        case AXIS_DESCENDANT_OR_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'descendant-or-self' ");
#endif
	    last = NULL;
            next = mlXPathNextDescendantOrSelf;
            break;
        case AXIS_FOLLOWING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'following' ");
#endif
	    last = NULL;
            next = mlXPathNextFollowing;
            break;
        case AXIS_FOLLOWING_SIBLING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'following-siblings' ");
#endif
	    last = NULL;
            next = mlXPathNextFollowingSibling;
            break;
        case AXIS_NAMESPACE:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'namespace' ");
#endif
	    last = NULL;
            first = NULL;
            next = (mlXPathTraversalFunction) mlXPathNextNamespace;
            break;
        case AXIS_PARENT:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'parent' ");
#endif
            first = NULL;
            next = mlXPathNextParent;
            break;
        case AXIS_PRECEDING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'preceding' ");
#endif
            first = NULL;
            next = mlXPathNextPrecedingInternal;
            break;
        case AXIS_PRECEDING_SIBLING:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext,
                            "axis 'preceding-sibling' ");
#endif
            first = NULL;
            next = mlXPathNextPrecedingSibling;
            break;
        case AXIS_SELF:
#ifdef DEBUG_STEP_NTH
            xmlGenericError(xmlGenericErrorContext, "axis 'self' ");
#endif
            first = NULL;
	    last = NULL;
            next = mlXPathNextSelf;
            break;
    }
    if (next == NULL)
        return(0);

    nodelist = obj->nodesetval;
    if (nodelist == NULL) {
        MP_DELETE( obj);
        valuePush(mlXPathWrapNodeSet(NULL));
        return(0);
    }
    addNode = mlXPathNodeSetAddUnique;
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext,
                    " context contains %d nodes\n", nodelist->nodeNr);
    switch (test) {
        case NODE_TEST_NONE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for none !!!\n");
            break;
        case NODE_TEST_TYPE:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for type %d\n", type);
            break;
        case NODE_TEST_PI:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for PI !!!\n");
            break;
        case NODE_TEST_ALL:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for *\n");
            break;
        case NODE_TEST_NS:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for namespace %s\n",
                            prefix);
            break;
        case NODE_TEST_NAME:
            xmlGenericError(xmlGenericErrorContext,
                            "           searching for name %s\n", name);
            if (prefix != NULL)
                xmlGenericError(xmlGenericErrorContext,
                                "           with namespace %s\n", prefix);
            break;
    }
    xmlGenericError(xmlGenericErrorContext, "Testing : ");
#endif
    /*
     * 2.3 Node Tests
     *  - For the attribute axis, the principal node type is attribute. 
     *  - For the namespace axis, the principal node type is namespace. 
     *  - For other axes, the principal node type is element. 
     *
     * A node test * is true for any node of the
     * principal node type. For example, child::* will
     * select all element children of the context node
     */
    tmp = this->context->node;
    list = mlXPathNodeSetCreate(NULL);
    for (i = 0; i < nodelist->GetSize(); i++) {
        this->context->node = nodelist->nodeTab[i];

        cur = NULL;
        n = 0;
        do {
            cur = next(this, cur);
            if (cur == NULL)
                break;
	    if ((first != NULL) && (*first == cur))
		break;
	    if (((t % 256) == 0) &&
	        (first != NULL) && (*first != NULL) &&
		(mlXPathCmpNodes(*first, cur) >= 0))
		break;
	    if ((last != NULL) && (*last == cur))
		break;
	    if (((t % 256) == 0) &&
	        (last != NULL) && (*last != NULL) &&
		(mlXPathCmpNodes(cur, *last) >= 0))
		break;
            t++;
            switch (test) {
                case NODE_TEST_NONE:
                    this->context->node = tmp;
                    STRANGE return(0);
                case NODE_TEST_TYPE:
                    if ((cur->nodeType == type) ||
                        ((type == NODE_TYPE_NODE) &&
                         ((cur->nodeType == XML_DOCUMENT_NODE) ||
                          (cur->nodeType == XML_HTML_DOCUMENT_NODE) ||
                          (cur->nodeType == XML_ELEMENT_NODE) ||
                          (cur->nodeType == XML_PI_NODE) ||
                          (cur->nodeType == XML_COMMENT_NODE) ||
                          (cur->nodeType == XML_CDATA_SECTION_NODE) ||
                          (cur->nodeType == XML_TEXT_NODE))) ||
			((type == NODE_TYPE_TEXT) &&
			 (cur->nodeType == XML_CDATA_SECTION_NODE))) {
                        n++;
                        if (n == indx)
                            addNode(list, cur);
                    }
                    break;
                case NODE_TEST_PI:
                    if (cur->nodeType == XML_PI_NODE) {
                        if ((name != NULL) &&
                            (!xmlStrEqual(name, cur->GetNodeName())))
                            break;
                        n++;
                        if (n == indx)
                            addNode(list, cur);
                    }
                    break;
                case NODE_TEST_ALL:
                    if (axis == AXIS_ATTRIBUTE) {
                        if (cur->nodeType == XML_ATTRIBUTE_NODE) {
                            n++;
                            if (n == indx)
                                addNode(list, cur);
                        }
                    } else if (axis == AXIS_NAMESPACE) {
                        if (cur->nodeType == XML_NAMESPACE_DECL) {
                            n++;
                            if (n == indx)
								mlXPathNodeSetAddNs(list, this->context->node, (mlNsPtr) cur);
                        }
                    } else {
                        if (cur->nodeType == XML_ELEMENT_NODE) {
                            if (prefix == NULL) {
                                n++;
                                if (n == indx)
                                    addNode(list, cur);
                            } else if ((cur->ns != NULL) &&
                                       (xmlStrEqual(URI, cur->ns->href))) {
                                n++;
                                if (n == indx)
                                    addNode(list, cur);
                            }
                        }
                    }
                    break;
                case NODE_TEST_NS:{
                        TODO;
                        break;
                    }
                case NODE_TEST_NAME:
                    switch (cur->nodeType) {
                        case XML_ELEMENT_NODE:
                            if (xmlStrEqual(name, cur->GetNodeName())) {
                                if (prefix == NULL) {
                                    if (cur->ns == NULL) {
                                        n++;
                                        if (n == indx)
                                            addNode(list, cur);
                                    }
                                } else {
                                    if ((cur->ns != NULL) &&
                                        (xmlStrEqual(URI,
                                                     cur->ns->href))) {
                                        n++;
                                        if (n == indx)
                                            addNode(list, cur);
                                    }
                                }
                            }
                            break;
                        case XML_ATTRIBUTE_NODE:{
                                mlAttrPtr attr = (mlAttrPtr) cur;

                                if (xmlStrEqual(name, attr->GetNodeName())) {
                                    if (prefix == NULL) {
                                        if ((attr->ns == NULL) ||
                                            (attr->ns->prefix == NULL)) {
                                            n++;
                                            if (n == indx)
                                                addNode(list, cur);
                                        }
                                    } else {
                                        if ((attr->ns != NULL) &&
                                            (xmlStrEqual(URI,
                                                         attr->ns->
                                                         href))) {
                                            n++;
                                            if (n == indx)
                                                addNode(list, cur);
                                        }
                                    }
                                }
                                break;
                            }
                        case XML_NAMESPACE_DECL:
                            if (cur->nodeType == XML_NAMESPACE_DECL) {
                                mlNsPtr ns = (mlNsPtr) cur;

                                if ((ns->prefix != NULL) && (name != NULL)
                                    && (xmlStrEqual(ns->prefix, name))) {
                                    n++;
                                    if (n == indx)
					mlXPathNodeSetAddNs(list,
					   this->context->node, (mlNsPtr) cur);
                                }
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                    break;
            }
        } while (n < indx);
    }
    this->context->node = tmp;
#ifdef DEBUG_STEP_NTH
    xmlGenericError(xmlGenericErrorContext,
                    "\nExamined %d nodes, found %d nodes at that step\n",
                    t, list->nodeNr);
#endif
    valuePush(mlXPathWrapNodeSet(list));
    if ((obj->boolval) && (obj->user != NULL)) {
	this->value->boolval = 1;
	this->value->user = obj->user;
	obj->user = NULL;
	obj->boolval = 0;
    }
     MP_DELETE( obj);
    return(t);
}

mlXPathObjectPtr mlXPathNewNodeSet(mlNodePtr val) {
    mlXPathObjectPtr ret;

    ret = MP_NEW( mlXPathObject);
    ret->type = XPATH_NODESET;
    ret->boolval = 0;
    ret->nodesetval = mlXPathNodeSetCreate(val);
    // @@ with_ns to check whether namespace nodes should be looked at @@
    return(ret);
}

void mlXPathParserContext::Root() {
    if ((context == NULL)) return;
    context->node = (mlNodePtr) context->doc;
    valuePush(mlXPathNewNodeSet(context->node));
}

int mlXPathParserContext::CompOpEval(mlXPathStepOp* op)
{
    int total = 0;
    int equal, ret;
    mlXPathCompExpr* comp;
    mlXPathObjectPtr arg1, arg2;
    mlIXMLNode* bak; // mlNodePtr bak;
	mlXMLDocument* bakd; // mlDocPtr bakd;
    int pp;
    int cs;

    CHECK_ERROR0;
    comp = this->comp;
    switch (op->op) {
        case XPATH_OP_END:
            return (0);
        case XPATH_OP_AND:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
		    CHECK_ERROR0;
            BooleanFunction(1);
            if ((this->value == NULL) || (this->value->boolval == 0))
                return (total);
            arg2 = valuePop();
			this->context->doc = bakd;
			this->context->node = bak;
			this->context->proximityPosition = pp;
			this->context->contextSize = cs;
            total += CompOpEval(&comp->steps[op->ch2]);
			if (this->error) {
				MP_DELETE( arg2);
				return(0);
			}
            BooleanFunction(1);
            arg1 = valuePop();
            arg1->boolval &= arg2->boolval;
            valuePush(arg1);
            MP_DELETE( arg2);
            return (total);
        case XPATH_OP_OR:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
		    CHECK_ERROR0;
            BooleanFunction(1);
            if ((this->value == NULL) || (this->value->boolval == 1))
                return (total);
            arg2 = valuePop();
			this->context->doc = bakd;
			this->context->node = bak;
			this->context->proximityPosition = pp;
			this->context->contextSize = cs;
            total += CompOpEval(&comp->steps[op->ch2]);
			if (this->error) {
				MP_DELETE( arg2);
				return(0);
			}
            BooleanFunction(1);
            arg1 = valuePop();
            arg1->boolval |= arg2->boolval;
            valuePush(arg1);
            MP_DELETE( arg2);
            return (total);
        case XPATH_OP_EQUAL:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
			CHECK_ERROR0;
			this->context->doc = bakd;
			this->context->node = bak;
			this->context->proximityPosition = pp;
			this->context->contextSize = cs;
            total += CompOpEval(&comp->steps[op->ch2]);
		    CHECK_ERROR0;
		    if (op->value)
				equal = EqualValues();
		    else
				equal = NotEqualValues();
			valuePush(mlXPathNewBoolean(equal));
            return (total);
        case XPATH_OP_CMP:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
			CHECK_ERROR0;
			this->context->doc = bakd;
			this->context->node = bak;
			this->context->proximityPosition = pp;
			this->context->contextSize = cs;
            total += CompOpEval(&comp->steps[op->ch2]);
		    CHECK_ERROR0;
            ret = CompareValues(op->value, op->value2);
            valuePush(mlXPathNewBoolean(ret));
            return (total);
/*
        case XPATH_OP_PLUS:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
		    CHECK_ERROR0;
            if (op->ch2 != -1) {
				this->context->doc = bakd;
				this->context->node = bak;
				this->context->proximityPosition = pp;
				this->context->contextSize = cs;
                total += CompOpEval(&comp->steps[op->ch2]);
		    }
		    CHECK_ERROR0;
            if (op->value == 0)
                SubValues();
            else if (op->value == 1)
                AddValues();
            else if (op->value == 2)
                ValueFlipSign();
            else if (op->value == 3) {
                CAST_TO_NUMBER;
                CHECK_TYPE0(XPATH_NUMBER);
            }
            return (total);
        case XPATH_OP_MULT:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
			CHECK_ERROR0;
			this->context->doc = bakd;
			this->context->node = bak;
			this->context->proximityPosition = pp;
			this->context->contextSize = cs;
            total += CompOpEval(ctxt, &comp->steps[op->ch2]);
		    CHECK_ERROR0;
            if (op->value == 0)
                MultValues();
            else if (op->value == 1)
                DivValues();
            else if (op->value == 2)
                ModValues();
            return (total);
        case XPATH_OP_UNION:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            total += CompOpEval(&comp->steps[op->ch1]);
	    CHECK_ERROR0;
	    this->context->doc = bakd;
	    this->context->node = bak;
	    this->context->proximityPosition = pp;
	    this->context->contextSize = cs;
            total += CompOpEval(&comp->steps[op->ch2]);
	    CHECK_ERROR0;
            CHECK_TYPE0(XPATH_NODESET);
            arg2 = valuePop();

            CHECK_TYPE0(XPATH_NODESET);
            arg1 = valuePop();

            arg1->nodesetval = NodeSetMerge(arg1->nodesetval,
                                                    arg2->nodesetval);
            valuePush(arg1);
            delete(arg2);
            return (total);
*/
        case XPATH_OP_ROOT:
            Root();
            return (total);
        case XPATH_OP_NODE:
            if (op->ch1 != -1)
                total += CompOpEval(&comp->steps[op->ch1]);
			CHECK_ERROR0;
            if (op->ch2 != -1)
                total += CompOpEval(&comp->steps[op->ch2]);
			CHECK_ERROR0;
            valuePush(mlXPathNewNodeSet(this->context->node));
            return (total);
        case XPATH_OP_RESET:
            if (op->ch1 != -1)
                total += CompOpEval(&comp->steps[op->ch1]);
		    CHECK_ERROR0;
            if (op->ch2 != -1)
                total += CompOpEval(&comp->steps[op->ch2]);
			CHECK_ERROR0;
            this->context->node = NULL;
            return (total);
        case XPATH_OP_COLLECT:{
                if (op->ch1 == -1)
                    return (total);

                total += CompOpEval(&comp->steps[op->ch1]);
				CHECK_ERROR0;

                //
                 // Optimization for [n] selection where n is a number
                 
                if ((op->ch2 != -1) &&
                    (comp->steps[op->ch2].op == XPATH_OP_PREDICATE) &&
                    (comp->steps[op->ch2].ch1 == -1) &&
                    (comp->steps[op->ch2].ch2 != -1) &&
                    (comp->steps[comp->steps[op->ch2].ch2].op ==
                     XPATH_OP_VALUE)) {
                    mlXPathObjectPtr val;

                    val = (mlXPathObjectPtr)(comp->steps[comp->steps[op->ch2].ch2].value4);
                    if ((val != NULL) && (val->type == XPATH_NUMBER)) {
                        int indx = (int) val->floatval;

                        if (val->floatval == (float) indx) {
                            total +=
                                NodeCollectAndTestNth(op,
                                                              indx, NULL,
                                                              NULL);
                            return (total);
                        }
                    }
                }
                total += NodeCollectAndTest(op, NULL, NULL);
                return (total);
            }
        case XPATH_OP_VALUE:
            valuePush(mlXPathObjectCopy((mlXPathObjectPtr) op->value4));
            return (total);
        case XPATH_OP_VARIABLE:{
			mlXPathObjectPtr val;

            if (op->ch1 != -1)
               total += CompOpEval(&comp->steps[op->ch1]);
            if (op->value5 == NULL) {
				val = mlXPathVariableLookup(this->context, (const wchar_t*)op->value4);
				if (val == NULL) {
					this->error = XPATH_UNDEF_VARIABLE_ERROR;
					return(0);
				}
			   valuePush(val);
			} else {
                const xmlChar *URI;

                URI = this->context->NsLookup((const wchar_t*)op->value5);
                if (URI == NULL) {
//                        xmlGenericError(xmlGenericErrorContext,
//                                        "xmlXPathCompOpEval: variable %s bound to undefined prefix %s\n",
//                                        op->value4, op->value5);
                    return (total);
                }
			    val = this->context->VariableLookupNS((const wchar_t*)op->value4, URI);
			    if (val == NULL) {
					this->error = XPATH_UNDEF_VARIABLE_ERROR;
					return(0);
				}
                valuePush(val);
			}
            return (total);
            }
/*
        case XPATH_OP_FUNCTION:{
            mlXPathFunction func;
            const xmlChar *oldFunc, *oldFuncURI;
			int i;

            if (op->ch1 != -1)
				total += CompOpEval(&comp->steps[op->ch1]);
		if (this->valueNr < op->value) {
		    xmlGenericError(xmlGenericErrorContext,
			    "xmlXPathCompOpEval: parameter error\n");
		    this->error = XPATH_INVALID_OPERAND;
		    return (total);
		}
		for (i = 0; i < op->value; i++)
		    if (this->valueTab[(this->valueNr - 1) - i] == NULL) {
			xmlGenericError(xmlGenericErrorContext,
				"xmlXPathCompOpEval: parameter error\n");
			this->error = XPATH_INVALID_OPERAND;
			return (total);
		    }
                if (op->cache != NULL)
                    XML_CAST_FPTR(func) = op->cache;
                else {
                    const xmlChar *URI = NULL;

                    if (op->value5 == NULL)
                        func =
                            xmlXPathFunctionLookup(this->context,
                                                   op->value4);
                    else {
                        URI = xmlXPathNsLookup(this->context, op->value5);
                        if (URI == NULL) {
                            xmlGenericError(xmlGenericErrorContext,
                                            "xmlXPathCompOpEval: function %s bound to undefined prefix %s\n",
                                            op->value4, op->value5);
                            return (total);
                        }
                        func = xmlXPathFunctionLookupNS(this->context,
                                                        op->value4, URI);
                    }
                    if (func == NULL) {
                        xmlGenericError(xmlGenericErrorContext,
                                        "xmlXPathCompOpEval: function %s not found\n",
                                        op->value4);
                        XP_ERROR0(XPATH_UNKNOWN_FUNC_ERROR);
                    }
                    op->cache = XML_CAST_FPTR(func);
                    op->cacheURI = (void *) URI;
                }
                oldFunc = this->context->function;
                oldFuncURI = this->context->functionURI;
                this->context->function = op->value4;
                this->context->functionURI = op->cacheURI;
                func(ctxt, op->value);
                this->context->function = oldFunc;
                this->context->functionURI = oldFuncURI;
                return (total);
            }
        case XPATH_OP_ARG:
			bakd = this->context->doc;
			bak = this->context->node;
			pp = this->context->proximityPosition;
			cs = this->context->contextSize;
            if (op->ch1 != -1)
                total += CompOpEval(&comp->steps[op->ch1]);
			this->context->contextSize = cs;
			this->context->proximityPosition = pp;
			this->context->node = bak;
			this->context->doc = bakd;
			CHECK_ERROR0;
            if (op->ch2 != -1) {
                total += CompOpEval(&comp->steps[op->ch2]);
	        this->context->doc = bakd;
	        this->context->node = bak;
	        CHECK_ERROR0;
	    }
            return (total);
        case XPATH_OP_PREDICATE:
        case XPATH_OP_FILTER:{
			xmlXPathObjectPtr res;
			xmlXPathObjectPtr obj, tmp;
			xmlNodeSetPtr newset = NULL;
			xmlNodeSetPtr oldset;
			xmlNodePtr oldnode;
			xmlDocPtr oldDoc;
                int i;

                //
                 // Optimization for ()[1] selection i.e. the first elem
                 
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
                    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
                    (comp->steps[op->ch2].op == XPATH_OP_VALUE)) {
                    xmlXPathObjectPtr val;

                    val = comp->steps[op->ch2].value4;
                    if ((val != NULL) && (val->type == XPATH_NUMBER) &&
                        (val->floatval == 1.0)) {
                        xmlNodePtr first = NULL;

                        total +=
                            CompOpEvalFirst(ctxt,
                                                    &comp->steps[op->ch1],
                                                    &first);
						CHECK_ERROR0;
                        //
                         // The nodeset should be in document order,
                         // Keep only the first value
                         
                        if ((this->value != NULL) &&
                            (this->value->type == XPATH_NODESET) &&
                            (this->value->nodesetval != NULL) &&
                            (this->value->nodesetval->nodeNr > 1))
                            this->value->nodesetval->nodeNr = 1;
                        return (total);
                    }
                }
                //
                 // Optimization for ()[last()] selection i.e. the last elem
                 
                if ((op->ch1 != -1) && (op->ch2 != -1) &&
                    (comp->steps[op->ch1].op == XPATH_OP_SORT) &&
                    (comp->steps[op->ch2].op == XPATH_OP_SORT)) {
                    int f = comp->steps[op->ch2].ch1;

                    if ((f != -1) &&
                        (comp->steps[f].op == XPATH_OP_FUNCTION) &&
                        (comp->steps[f].value5 == NULL) &&
                        (comp->steps[f].value == 0) &&
                        (comp->steps[f].value4 != NULL) &&
                        (xmlStrEqual
                         (comp->steps[f].value4, BAD_CAST "last"))) {
                        xmlNodePtr last = NULL;

                        total +=
                            CompOpEvalLast(
                                                   &comp->steps[op->ch1],
                                                   &last);
						CHECK_ERROR0;
                        //
                         // The nodeset should be in document order,
                         // Keep only the last value
                         
                        if ((this->value != NULL) &&
                            (this->value->type == XPATH_NODESET) &&
                            (this->value->nodesetval != NULL) &&
                            (this->value->nodesetval->nodeTab != NULL) &&
                            (this->value->nodesetval->nodeNr > 1)) {
                            this->value->nodesetval->nodeTab[0] =
                                this->value->nodesetval->nodeTab[this->
                                                                 value->
                                                                 nodesetval->
                                                                 nodeNr -
                                                                 1];
                            this->value->nodesetval->nodeNr = 1;
                        }
                        return (total);
                    }
                }

                if (op->ch1 != -1)
                    total +=
                        CompOpEval(&comp->steps[op->ch1]);
				CHECK_ERROR0;
                if (op->ch2 == -1)
                    return (total);
                if (this->value == NULL)
                    return (total);

                oldnode = this->context->node;

#ifdef LIBXML_XPTR_ENABLED
                //
                 // Hum are we filtering the result of an XPointer expression
                 
                if (this->value->type == XPATH_LOCATIONSET) {
                    xmlLocationSetPtr newlocset = NULL;
                    xmlLocationSetPtr oldlocset;

                    //
                     // Extract the old locset, and then evaluate the result of the
                     // expression for all the element in the locset. use it to grow
                     // up a new locset.
                     
                    CHECK_TYPE0(XPATH_LOCATIONSET);
                    obj = valuePop();
                    oldlocset = obj->user;
                    this->context->node = NULL;

                    if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
                        this->context->contextSize = 0;
                        this->context->proximityPosition = 0;
                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        res = valuePop();
                        if (res != NULL)
                            delete(res);
                        valuePush(obj);
                        CHECK_ERROR0;
                        return (total);
                    }
                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    for (i = 0; i < oldlocset->locNr; i++) {
                        //
                         // Run the evaluation with a node list made of a
                         // single item in the nodelocset.
                         
                        this->context->node = oldlocset->locTab[i]->user;
                        this->context->contextSize = oldlocset->locNr;
                        this->context->proximityPosition = i + 1;
                        tmp = xmlXPathNewNodeSet(this->context->node);
                        valuePush(tmp);

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        //
                         // The result of the evaluation need to be tested to
                         // decided whether the filter succeeded or not
                         
                        res = valuePop();
                        if (xmlXPathEvaluatePredicateResult(ctxt, res)) {
                            xmlXPtrLocationSetAdd(newlocset,
                                                  xmlXPathObjectCopy
                                                  (oldlocset->locTab[i]));
                        }

                        //
                         // Cleanup
                         
                        if (res != NULL)
                            delete(res);
                        if (this->value == tmp) {
                            res = valuePop();
                            delete(res);
                        }

                        this->context->node = NULL;
                    }

                    //
                     // The result is used as the new evaluation locset.
                     
                    delete(obj);
                    this->context->node = NULL;
                    this->context->contextSize = -1;
                    this->context->proximityPosition = -1;
                    valuePush(xmlXPtrWrapLocationSet(newlocset));
                    this->context->node = oldnode;
                    return (total);
                }
#endif // LIBXML_XPTR_ENABLED

                //
                 // Extract the old set, and then evaluate the result of the
                 // expression for all the element in the set. use it to grow
                 // up a new set.
                 
                CHECK_TYPE0(XPATH_NODESET);
                obj = valuePop();
                oldset = obj->nodesetval;

                oldnode = this->context->node;
		oldDoc = this->context->doc;
                this->context->node = NULL;

                if ((oldset == NULL) || (oldset->nodeNr == 0)) {
                    this->context->contextSize = 0;
                    this->context->proximityPosition = 0;

//                    if (op->ch2 != -1)
//                        total +=
//                            xmlXPathCompOpEval(ctxt,
//                                               &comp->steps[op->ch2]);
//		    CHECK_ERROR0;
//                    res = valuePop();
//                    if (res != NULL)
//                        delete(res);

                    valuePush(obj);
                    this->context->node = oldnode;
                    CHECK_ERROR0;
                } else {
                    //
                     // Initialize the new set.
					 // Also set the xpath document in case things like
					 // key() evaluation are attempted on the predicate
                     
                    newset = NodeSetCreate(NULL);

                    for (i = 0; i < oldset->nodeNr; i++) {
                        //
                         // Run the evaluation with a node list made of
                         // a single item in the nodeset.
                         
                        this->context->node = oldset->nodeTab[i];
						if ((oldset->nodeTab[i]->type != XML_NAMESPACE_DECL) &&
							(oldset->nodeTab[i]->doc != NULL))
								this->context->doc = oldset->nodeTab[i]->doc;
                        tmp = NewNodeSet(this->context->node);
                        valuePush(tmp);
                        this->context->contextSize = oldset->nodeNr;
                        this->context->proximityPosition = i + 1;

                        if (op->ch2 != -1)
                            total += CompOpEval(&comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        //
                         // The result of the evaluation needs to be tested to
                         // decide whether the filter succeeded or not
                         
                        res = valuePop();
                        if (EvaluatePredicateResult(res)) {
                            NodeSetAdd(newset, oldset->nodeTab[i]);
                        }

                        //
                         // Cleanup
                         
                        if (res != NULL)
                             MP_DELETE( res;
                        if (this->value == tmp) {
                            res = valuePop();
                             MP_DELETE( res;
                        }

                        this->context->node = NULL;
                    }

                    //
                     // The result is used as the new evaluation set.
                     
                     MP_DELETE( obj;
                    this->context->node = NULL;
                    this->context->contextSize = -1;
                    this->context->proximityPosition = -1;
					// may want to move this past the '}' later 
					this->context->doc = oldDoc;
                    valuePush(WrapNodeSet(newset));
                }
                this->context->node = oldnode;
                return (total);
            }
        case XPATH_OP_SORT:
            if (op->ch1 != -1)
                total += CompOpEval(ctxt, &comp->steps[op->ch1]);
		    CHECK_ERROR0;
            if ((this->value != NULL) &&
                (this->value->type == XPATH_NODESET) &&
                (this->value->nodesetval != NULL))
                NodeSetSort(this->value->nodesetval);
            return (total);
#ifdef LIBXML_XPTR_ENABLED
        case XPATH_OP_RANGETO:{
                xmlXPathObjectPtr range;
                xmlXPathObjectPtr res, obj;
                xmlXPathObjectPtr tmp;
                xmlLocationSetPtr newlocset = NULL;
		    xmlLocationSetPtr oldlocset;
                xmlNodeSetPtr oldset;
                int i, j;

                if (op->ch1 != -1)
                    total +=
                        xmlXPathCompOpEval(ctxt, &comp->steps[op->ch1]);
                if (op->ch2 == -1)
                    return (total);

                if (this->value->type == XPATH_LOCATIONSET) {
                    //
                     // Extract the old locset, and then evaluate the result of the
                     // expression for all the element in the locset. use it to grow
                     // up a new locset.
                     
                    CHECK_TYPE0(XPATH_LOCATIONSET);
                    obj = valuePop();
                    oldlocset = obj->user;

                    if ((oldlocset == NULL) || (oldlocset->locNr == 0)) {
		        this->context->node = NULL;
                        this->context->contextSize = 0;
                        this->context->proximityPosition = 0;
                        total += xmlXPathCompOpEval(ctxt,&comp->steps[op->ch2]);
                        res = valuePop();
                        if (res != NULL)
                            delete(res);
                        valuePush(obj);
                        CHECK_ERROR0;
                        return (total);
                    }
                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    for (i = 0; i < oldlocset->locNr; i++) {
                        //
                         // Run the evaluation with a node list made of a
                         // single item in the nodelocset.
                         
                        this->context->node = oldlocset->locTab[i]->user;
                        this->context->contextSize = oldlocset->locNr;
                        this->context->proximityPosition = i + 1;
                        tmp = xmlXPathNewNodeSet(this->context->node);
                        valuePush(tmp);

                        if (op->ch2 != -1)
                            total +=
                                xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                        CHECK_ERROR0;

                        res = valuePop();
			if (res->type == XPATH_LOCATIONSET) {
			    xmlLocationSetPtr rloc = 
			        (xmlLocationSetPtr)res->user;
			    for (j=0; j<rloc->locNr; j++) {
			        range = xmlXPtrNewRange(
				  oldlocset->locTab[i]->user,
				  oldlocset->locTab[i]->index,
				  rloc->locTab[j]->user2,
				  rloc->locTab[j]->index2);
				if (range != NULL) {
				    xmlXPtrLocationSetAdd(newlocset, range);
				}
			    }
			} else {
			    range = xmlXPtrNewRangeNodeObject(
				(xmlNodePtr)oldlocset->locTab[i]->user, res);
                            if (range != NULL) {
                                xmlXPtrLocationSetAdd(newlocset,range);
			    }
                        }

                        //
                         // Cleanup
                         
                        if (res != NULL)
                            delete(res);
                        if (this->value == tmp) {
                            res = valuePop();
                            delete(res);
                        }

                        this->context->node = NULL;
                    }
		} else {	// Not a location set
                    CHECK_TYPE0(XPATH_NODESET);
                    obj = valuePop();
                    oldset = obj->nodesetval;
                    this->context->node = NULL;

                    newlocset = xmlXPtrLocationSetCreate(NULL);

                    if (oldset != NULL) {
                        for (i = 0; i < oldset->nodeNr; i++) {
                            //
                             // Run the evaluation with a node list made of a single item
                             // in the nodeset.
                             
                            this->context->node = oldset->nodeTab[i];
                            tmp = xmlXPathNewNodeSet(this->context->node);
                            valuePush(tmp);

                            if (op->ch2 != -1)
                                total +=
                                    xmlXPathCompOpEval(ctxt,
                                                   &comp->steps[op->ch2]);
                            CHECK_ERROR0;

                            res = valuePop();
                            range =
                                xmlXPtrNewRangeNodeObject(oldset->nodeTab[i],
                                                      res);
                            if (range != NULL) {
                                xmlXPtrLocationSetAdd(newlocset, range);
                            }

                            //
                             // Cleanup
                             
                            if (res != NULL)
                                delete(res);
                            if (this->value == tmp) {
                                res = valuePop();
                                delete(res);
                            }

                            this->context->node = NULL;
                        }
                    }
                }

                //
                 // The result is used as the new evaluation set.
                 
                delete(obj);
                this->context->node = NULL;
                this->context->contextSize = -1;
                this->context->proximityPosition = -1;
                valuePush(xmlXPtrWrapLocationSet(newlocset));
                return (total);
            }
#endif // LIBXML_XPTR_ENABLED
*/
    }
    xmlGenericError(xmlGenericErrorContext,
                    "XPath: unknown precompiled operation %d\n", op->op);
    return (total);
}

#define CAST_TO_STRING							\
    if ((this->value != NULL) && (this->value->type != XPATH_STRING))	\
        StringFunction(1);

#define CAST_TO_NUMBER							\
    if ((this->value != NULL) && (this->value->type != XPATH_NUMBER))	\
        NumberFunction(1);

#define CAST_TO_BOOLEAN							\
    if ((this->value != NULL) && (this->value->type != XPATH_BOOLEAN))	\
        BooleanFunction(1);

int trio_isnan (double number)
{
  return _isnan(number) ? 1 : 0;
}

int IsNaN(double val) {
    return(trio_isnan(val));
}

int CastStringToBoolean (const xmlChar *val) {
    if ((val == NULL) || (xmlStrlen(val) == 0))
		return(0);
    return(1);
}

int CastNumberToBoolean (double val) {
     if (IsNaN(val) || (val == 0.0))
		return(0);
     return(1);
}

int CastNodeSetToBoolean (mlNodeSetPtr ns) {
    if ((ns == NULL) || (ns->GetSize() == 0))
		return(0);
    return(1);
}

int CastToBoolean (mlXPathObjectPtr val) {
    int ret = 0;

    if (val == NULL)
		return(0);
    switch (val->type) {
    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	xmlGenericError(xmlGenericErrorContext, "BOOLEAN: undefined\n");
#endif
	ret = 0;
	break;
    case XPATH_NODESET:
    case XPATH_XSLT_TREE:
	ret = CastNodeSetToBoolean(val->nodesetval);
	break;
    case XPATH_STRING:
	ret = CastStringToBoolean(val->stringval);
	break;
    case XPATH_NUMBER:
	ret = CastNumberToBoolean(val->floatval);
	break;
    case XPATH_BOOLEAN:
	ret = val->boolval;
	break;
    case XPATH_USERS:
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
	TODO;
	ret = 0;
	break;
    }
    return(ret);
}

mlXPathObjectPtr ConvertBoolean(mlXPathObjectPtr val) {
    mlXPathObjectPtr ret;

    if (val == NULL)
		return(mlXPathNewBoolean(0));
    if (val->type == XPATH_BOOLEAN)
		return(val);
    ret = mlXPathNewBoolean(CastToBoolean(val));
    MP_DELETE( val);
    return(ret);
}

void mlXPathParserContext::BooleanFunction(int nargs) {
    mlXPathObjectPtr cur;

    CHECK_ARITY(1);
    cur = valuePop();
    if (cur == NULL) XP_ERROR(XPATH_INVALID_OPERAND);
    cur = ConvertBoolean(cur);
    valuePush(cur);
}

void mlXPathParserContext::NotFunction(int nargs) {
    CHECK_ARITY(1);
    CAST_TO_BOOLEAN;
    CHECK_TYPE(XPATH_BOOLEAN);
    this->value->boolval = ! this->value->boolval;
}

void mlXPathParserContext::TrueFunction(int nargs) {
    CHECK_ARITY(0);
    valuePush(mlXPathNewBoolean(1));
}

void mlXPathParserContext::FalseFunction(int nargs) {
    CHECK_ARITY(0);
    valuePush(mlXPathNewBoolean(0));
}

unsigned int mlXPathStringHash(const xmlChar * string) {
    if (string == NULL)
	return((unsigned int) 0);
    if (string[0] == 0)
	return(0);
    return(((unsigned int) string[0]) +
	   (((unsigned int) string[1]) << 8));
}

unsigned int mlXPathNodeValHash(mlIXMLNodePtr node) {
    int len = 2;
    const xmlChar * string = NULL;
    mlIXMLNodePtr tmp = NULL;
    unsigned int ret = 0;

    if (node == NULL)
		return(0);

    if (node->nodeType == XML_DOCUMENT_NODE) {
		tmp = ((mlXMLDocument*)node)->GetRootElement(); // xmlDocGetRootElement((mlDocPtr) node);
		if (tmp == NULL)
			node = node->GetFirstChildNode();
		else
			node = tmp;

		if (node == NULL)
			return(0);
    }

    switch (node->nodeType) {
	case XML_COMMENT_NODE:
	case XML_PI_NODE:
	case XML_CDATA_SECTION_NODE:
	case XML_TEXT_NODE:
	    string = node->GetContent();
	    if (string == NULL)
			return(0);
	    if (string[0] == 0)
			return(0);
	    return(((unsigned int) string[0]) +
		   (((unsigned int) string[1]) << 8));
//	case XML_NAMESPACE_DECL:
//	    string = ((xmlNsPtr)node)->href;
//	    if (string == NULL)
//		return(0);
//	    if (string[0] == 0)
//		return(0);
//	    return(((unsigned int) string[0]) +
//		   (((unsigned int) string[1]) << 8));
	case XML_ATTRIBUTE_NODE:
	    tmp = node->GetFirstChildNode();
	    break;
	case XML_ELEMENT_NODE:
	    tmp = node->GetFirstChildNode();
	    break;
	default:
	    return(0);
    }
    while (tmp != NULL) {
		switch (tmp->nodeType) {
			case XML_COMMENT_NODE:
			case XML_PI_NODE:
			case XML_CDATA_SECTION_NODE:
			case XML_TEXT_NODE:
				string = tmp->GetContent();
			break;
	//	    case mlIXMLNode::NAMESPACE_DECL:
	//		string = ((xmlNsPtr)tmp)->href;
			break;
			default:
			break;
		}
		if ((string != NULL) && (string[0] != 0)) {
			if (len == 1) {
			return(ret + (((unsigned int) string[0]) << 8));
			}
			if (string[1] == 0) {
			len = 1;
			ret = (unsigned int) string[0];
			} else {
			return(((unsigned int) string[0]) +
				   (((unsigned int) string[1]) << 8));
			}
		}
		//
		 // Skip to next node
		 
		if ((tmp->GetFirstChild() != NULL) && (tmp->nodeType != XML_DTD_NODE)) {
			if (tmp->GetFirstChildNode()->nodeType != XML_ENTITY_DECL) {
				tmp = tmp->GetFirstChildNode();
				continue;
			}
		}
		if (tmp == node)
			break;

		if (tmp->GetNextSiblingNode() != NULL) {
			tmp = tmp->GetNextSiblingNode();
			continue;
		}
		
		do {
			tmp = tmp->GetParentNode();
			if (tmp == NULL)
				break;
			if (tmp == node) {
				tmp = NULL;
				break;
			}
			if (tmp->GetNextSiblingNode() != NULL) {
				tmp = tmp->GetNextSiblingNode();
				break;
			}
		} while (tmp != NULL);
    }
    return(ret);
}

struct mlBuffer {
	mlString content;
    // xmlChar *content;		// The buffer content UTF8
    // unsigned int use;		// The buffer size used
    // unsigned int size;		// The buffer size
    // xmlBufferAllocationScheme alloc; // The realloc method
	void Cat(const wchar_t* str){
		content += str;
	}
	const wchar_t* GetContent(){
		return content.c_str();
	}
};
typedef mlBuffer *mlBufferPtr;

int mlNodeBufGetContent(mlBufferPtr buffer, mlNodePtr cur)
{
    if ((cur == NULL) || (buffer == NULL)) return(-1);
    switch (cur->nodeType) {
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
			buffer->Cat(cur->GetContent());
            break;
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:{
                mlNodePtr tmp = cur;

                while (tmp != NULL) {
                    switch (tmp->nodeType) {
                        case XML_CDATA_SECTION_NODE:
                        case XML_TEXT_NODE:
                            if (tmp->GetContent() != NULL)
                                buffer->Cat(tmp->GetContent());
                            break;
                        case XML_ENTITY_REF_NODE:
							mlNodeBufGetContent(buffer, tmp->GetFirstChildNode());
							break;
                        default:
                            break;
                    }
                    //
                     // Skip to next node
                     
                    if (tmp->GetFirstChildNode() != NULL) {
                        if (tmp->GetFirstChildNode()->nodeType != XML_ENTITY_DECL) {
                            tmp = tmp->GetFirstChildNode();
                            continue;
                        }
                    }
                    if (tmp == cur)
                        break;

                    if (tmp->GetNextSiblingNode() != NULL) {
                        tmp = tmp->GetNextSiblingNode();
                        continue;
                    }

                    do {
                        tmp = tmp->GetParentNode();
                        if (tmp == NULL)
                            break;
                        if (tmp == cur) {
                            tmp = NULL;
                            break;
                        }
                        if (tmp->GetNextSiblingNode() != NULL) {
                            tmp = tmp->GetNextSiblingNode();
                            break;
                        }
                    } while (tmp != NULL);
                }
		break;
            }
        case XML_ATTRIBUTE_NODE:{
			buffer->Cat(cur->GetContent());
//            xmlAttrPtr attr = (xmlAttrPtr) cur;
//			mlNodePtr tmp = attr->children;
//
//			while (tmp != NULL) {
//				if (tmp->nodeType == XML_TEXT_NODE)
//					buffer->Cat(tmp->GetContent());
//				else
//					mlNodeBufGetContent(buffer, tmp);
//				tmp = tmp->next;
//			}
            break;
            }
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
			buffer->Cat(cur->GetContent());
            break;
        case XML_ENTITY_REF_NODE:{
			// ??
//                xmlEntityPtr ent;
//                xmlNodePtr tmp;
//
//                // lookup entity declaration
//                ent = xmlGetDocEntity(cur->doc, cur->name);
//                if (ent == NULL)
//                    return(-1);
//
//                // an entity content can be any "well balanced chunk",
//                 // i.e. the result of the content [43] production:
//                 // http://www.w3.org/TR/REC-xml#NT-content
//                 // -> we iterate through child nodes and recursive call
//                 // xmlNodeGetContent() which handles all possible node types */
//                tmp = ent->children;
//                while (tmp) {
//		    xmlNodeBufGetContent(buffer, tmp);
//                    tmp = tmp->next;
//                }
		break;
            }
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            break;
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE:
			cur = cur->GetFirstChildNode();
			while (cur!= NULL) {
			if ((cur->nodeType == XML_ELEMENT_NODE) ||
				(cur->nodeType == XML_TEXT_NODE) ||
				(cur->nodeType == XML_CDATA_SECTION_NODE)) {
				mlNodeBufGetContent(buffer, cur);
			}
			cur = cur->GetNextSiblingNode();
			}
			break;
        case XML_NAMESPACE_DECL:
//			xmlBufferCat(buffer, ((xmlNsPtr) cur)->href);
			break;
        case XML_ELEMENT_DECL:
        case XML_ATTRIBUTE_DECL:
        case XML_ENTITY_DECL:
            break;
    }
    return(0);
}

xmlChar* mlNodeGetContent(mlIXMLNodePtr cur)
{
    if (cur == NULL)
        return (NULL);
    switch (cur->nodeType) {
        case XML_DOCUMENT_FRAG_NODE:
        case XML_ELEMENT_NODE:{
                mlBufferPtr buffer;
                xmlChar *ret;

				buffer = MP_NEW( mlBuffer); // buffer = xmlBufferCreateSize(64);
                if (buffer == NULL)
                    return (NULL);
				mlNodeBufGetContent(buffer, cur);
                // ret = buffer->content;
                // buffer->content = NULL;
				ret = Strdup(buffer->content.c_str());
                MP_DELETE( buffer);
                return (ret);
            }
        case XML_ATTRIBUTE_NODE:{
			return (xmlChar*)cur->GetContent();
			// ??
//                xmlAttrPtr attr = (xmlAttrPtr) cur;
//
//                if (attr->parent != NULL)
//                    return (mlNodeListGetString
//                            (attr->parent->doc, attr->children, 1));
//                else
//                    return (mlNodeListGetString(NULL, attr->children, 1));
                break;
            }
        case XML_COMMENT_NODE:
        case XML_PI_NODE:
            if (cur->GetContent() != NULL)
                return (Strdup(cur->GetContent()));
            return (NULL);
//        case XML_ENTITY_REF_NODE:{
//                xmlEntityPtr ent;
//                xmlBufferPtr buffer;
//                xmlChar *ret;
//
//                /* lookup entity declaration */
//                ent = xmlGetDocEntity(cur->doc, cur->name);
//                if (ent == NULL)
//                    return (NULL);
//
//                buffer = xmlBufferCreate();
//                if (buffer == NULL)
//                    return (NULL);
//
//                xmlNodeBufGetContent(buffer, cur);
//
//                ret = buffer->content;
//                buffer->content = NULL;
//                xmlBufferFree(buffer);
//                return (ret);
//            }
        case XML_ENTITY_NODE:
        case XML_DOCUMENT_TYPE_NODE:
        case XML_NOTATION_NODE:
        case XML_DTD_NODE:
        case XML_XINCLUDE_START:
        case XML_XINCLUDE_END:
            return (NULL);
        case XML_DOCUMENT_NODE:
#ifdef LIBXML_DOCB_ENABLED
        case XML_DOCB_DOCUMENT_NODE:
#endif
        case XML_HTML_DOCUMENT_NODE: {
	    mlBufferPtr buffer;
	    xmlChar *ret;

	    buffer = MP_NEW( mlBuffer); 
	    if (buffer == NULL)
			return (NULL);

	    mlNodeBufGetContent(buffer, (mlNodePtr) cur);

	    ret = Strdup(buffer->GetContent());
	    MP_DELETE( buffer);
	    return (ret);
	}
//        case XML_NAMESPACE_DECL: {
//	    xmlChar *tmp;
//
//	    tmp = xmlStrdup(((xmlNsPtr) cur)->href);
//            return (tmp);
//	}
        case XML_ELEMENT_DECL:
            // TODO !!!
            return (NULL);
        case XML_ATTRIBUTE_DECL:
            // TODO !!!
            return (NULL);
        case XML_ENTITY_DECL:
            // TODO !!!
            return (NULL);
        case XML_CDATA_SECTION_NODE:
        case XML_TEXT_NODE:
            if (cur->GetContent() != NULL)
                return (Strdup(cur->GetContent()));
            return (NULL);
    }
    return (NULL);
}

int mlXPathEqualNodeSetString(mlXPathObjectPtr arg, const xmlChar * str, int neq)
{
    int i;
    mlNodeSetPtr ns;
    xmlChar *str2;
    unsigned int hash;

    if ((str == NULL) || (arg == NULL) ||
        ((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return (0);
    ns = arg->nodesetval;
    /*
     * A NULL nodeset compared with a string is always false
     * (since there is no node equal, and no node not equal)
     */
    if ((ns == NULL) || (ns->GetSize() <= 0) )
        return (0);
    hash = mlXPathStringHash(str);
    for (i = 0; i < ns->GetSize(); i++) {
        if (mlXPathNodeValHash(ns->nodeTab[i]) == hash) {
            str2 = mlNodeGetContent(ns->nodeTab[i]);
            if ((str2 != NULL) && (xmlStrEqual(str, str2))) {
                xmlFree(str2);
			if (neq)
			    continue;
            return (1);
	    } else if ((str2 == NULL) && (xmlStrEqual(str, L""))) {
		if (neq)
		    continue;
                return (1);
            } else if (neq) {
		if (str2 != NULL)
		    xmlFree(str2);
		return (1);
	    }
            if (str2 != NULL)
                xmlFree(str2);
        } else if (neq)
	    return (1);
    }
    return (0);
}

typedef wchar_t* mlCharPtr;

xmlChar* mlXPathCastNodeToString (mlNodePtr node) {
    return(mlNodeGetContent(node));
}

unsigned long nan[2]={0xffffffff, 0x7fffffff};
#define mlXPathNAN (*(double*)nan)



double mlXPathStringEvalNumber(const xmlChar *str) {
    const xmlChar *cur = str;
    double ret;
    int ok = 0;
    int isneg = 0;
    int exponent = 0;
    int is_exponent_negative = 0;

    if (cur == NULL) return(0);
    while (IS_BLANK_CH(*cur)) cur++;
    if ((*cur != '.') && ((*cur < '0') || (*cur > '9')) && (*cur != '-')) {
        return(mlXPathNAN);
    }
    if (*cur == '-') {
	isneg = 1;
	cur++;
    }


    ret = 0;
    while ((*cur >= '0') && (*cur <= '9')) {
	ret = ret * 10 + (*cur - '0');
	ok = 1;
	cur++;
    }

    if (*cur == '.') {
	int v, frac = 0;
	double fraction = 0;

        cur++;
	if (((*cur < '0') || (*cur > '9')) && (!ok)) {
	    return(mlXPathNAN);
	}
	while (((*cur >= '0') && (*cur <= '9')) && (frac < MAX_FRAC)) {
	    v = (*cur - '0');
	    fraction = fraction * 10 + v;
	    frac = frac + 1;
	    cur++;
	}
	fraction /= my_pow10[frac];
	ret = ret + fraction;
	while ((*cur >= '0') && (*cur <= '9'))
	    cur++;
    }
    if ((*cur == 'e') || (*cur == 'E')) {
      cur++;
      if (*cur == '-') {
	is_exponent_negative = 1;
	cur++;
      } else if (*cur == '+') {
        cur++;
      }
      while ((*cur >= '0') && (*cur <= '9')) {
	exponent = exponent * 10 + (*cur - '0');
	cur++;
      }
    }
    while (IS_BLANK_CH(*cur)) cur++;
    if (*cur != 0) return(mlXPathNAN);
    if (isneg) ret = -ret;
    if (is_exponent_negative) exponent = -exponent;
    ret *= pow(10.0, (double)exponent);
    return(ret);
}

void mlXPathNodeSetSort(mlNodeSetPtr set) {
    int i, j, incr, len;
    mlNodePtr tmp;

    if (set == NULL)
		return;

    // Use Shell's sort to sort the node-set
    len = set->GetSize();
    for (incr = len / 2; incr > 0; incr /= 2) {
	for (i = incr; i < len; i++) {
	    j = i - incr;
	    while (j >= 0) {
		if (mlXPathCmpNodes(set->nodeTab[j],
				     set->nodeTab[j + incr]) == -1) {
		    tmp = set->nodeTab[j];
		    set->nodeTab[j] = set->nodeTab[j + incr];
		    set->nodeTab[j + incr] = tmp;
		    j -= incr;
		} else
		    break;
	    }
	}
    }
}

xmlChar* mlXPathCastNodeSetToString (mlNodeSetPtr ns) {
    if ((ns == NULL) || (ns->GetSize() == 0))
		return(Strdup((const xmlChar *) ""));

    mlXPathNodeSetSort(ns);
    return(mlXPathCastNodeToString(ns->nodeTab[0]));
}

double mlXPathCastStringToNumber(const xmlChar * val) {
    return(mlXPathStringEvalNumber(val));
}

double mlXPathCastNodeSetToNumber(mlNodeSetPtr ns) {
    xmlChar *str;
    double ret;

    if (ns == NULL)
		return(mlXPathNAN);
    str = mlXPathCastNodeSetToString(ns);
    ret = mlXPathCastStringToNumber(str);
     MP_DELETE( str);
    return(ret);
}

double mlXPathCastBooleanToNumber(int val) {
    if (val)
	return(1.0);
    return(0.0);
}

double mlXPathCastToNumber(mlXPathObjectPtr val) {
    double ret = 0.0;

    if (val == NULL)
	return(xmlXPathNAN);
    switch (val->type) {
    case XPATH_UNDEFINED:
#ifdef DEGUB_EXPR
	xmlGenericError(xmlGenericErrorContext, "NUMBER: undefined\n");
#endif
	ret = mlXPathNAN;
	break;
    case XPATH_NODESET:
    case XPATH_XSLT_TREE:
	ret = mlXPathCastNodeSetToNumber(val->nodesetval);
	break;
    case XPATH_STRING:
	ret = mlXPathCastStringToNumber(val->stringval);
	break;
    case XPATH_NUMBER:
	ret = val->floatval;
	break;
    case XPATH_BOOLEAN:
	ret = mlXPathCastBooleanToNumber(val->boolval);
	break;
    case XPATH_USERS:
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
	TODO;
	ret = mlXPathNAN;
	break;
    }
    return(ret);
}

mlXPathObjectPtr mlXPathConvertNumber(mlXPathObjectPtr val) {
    mlXPathObjectPtr ret;

    if (val == NULL)
		return(mlXPathNewFloat(0.0));
    if (val->type == XPATH_NUMBER)
		return(val);
    ret = mlXPathNewFloat(mlXPathCastToNumber(val));
     MP_DELETE( val);
    return(ret);
}

void mlXPathParserContext::NumberFunction(int nargs) {
    mlXPathObjectPtr cur;
    double res;

    if (nargs == 0) {
	if (this->context->node == NULL) {
	    valuePush(mlXPathNewFloat(0.0));
	} else {
	    xmlChar* content = mlNodeGetContent(this->context->node);

	    res = mlXPathStringEvalNumber(content);
	    valuePush(mlXPathNewFloat(res));
	     MP_DELETE( content);
	}
	return;
    }

    CHECK_ARITY(1);
    cur = valuePop();
    cur = mlXPathConvertNumber(cur);
    valuePush(cur);
}

int mlXPathParserContext::EqualNodeSetFloat(
    mlXPathObjectPtr arg, double f, int neq) {
  int i, ret=0;
  mlNodeSetPtr ns;
  xmlChar *str2;
  mlXPathObjectPtr val;
  double v;

    if ((arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return(0);

    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i=0;i<ns->GetSize();i++) {
	    str2 = mlXPathCastNodeToString(ns->nodeTab[i]);
	    if (str2 != NULL) {
		valuePush(mlXPathNewString(str2));
		 MP_DELETE( str2);
		NumberFunction(1);
		val = valuePop();
		v = val->floatval;
		 MP_DELETE( val);
		if (!IsNaN(v)) {
		    if ((!neq) && (v==f)) {
				ret = 1;
				break;
		    } else if ((neq) && (v!=f)) {
				ret = 1;
				break;
		    }
		}
	    }
	}
    }

    return(ret);
}

int mlXPathEqualNodeSets(mlXPathObjectPtr arg1, mlXPathObjectPtr arg2, int neq) {
    int i, j;
    unsigned int *hashs1;
    unsigned int *hashs2;
    xmlChar **values1;
    xmlChar **values2;
    int ret = 0;
    mlNodeSetPtr ns1;
    mlNodeSetPtr ns2;

    if ((arg1 == NULL) ||
	((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)))
        return(0);
    if ((arg2 == NULL) ||
	((arg2->type != XPATH_NODESET) && (arg2->type != XPATH_XSLT_TREE)))
        return(0);

    ns1 = arg1->nodesetval;
    ns2 = arg2->nodesetval;

    if ((ns1 == NULL) || (ns1->GetSize() <= 0))
		return(0);
    if ((ns2 == NULL) || (ns2->GetSize() <= 0))
		return(0);

    //
     // for equal, check if there is a node pertaining to both sets
     
    if (neq == 0)
		for (i = 0;i < ns1->GetSize();i++)
			for (j = 0;j < ns2->GetSize();j++)
			if (ns1->nodeTab[i] == ns2->nodeTab[j])
				return(1);

    values1 = (xmlChar **) MP_NEW_ARR( mlCharPtr, ns1->GetSize());
    if (values1 == NULL) {
        xmlXPathErrMemory(NULL, "comparing nodesets\n");
		return(0);
    }
    hashs1 = MP_NEW_ARR( unsigned int, ns1->GetSize());
    if (hashs1 == NULL) {
        xmlXPathErrMemory(NULL, "comparing nodesets\n");
		 MP_DELETE( values1);
		return(0);
    }
    memset(values1, 0, ns1->GetSize() * sizeof(xmlChar *));
    values2 = (xmlChar **) MP_NEW_ARR( mlCharPtr, ns2->GetSize());
    if (values2 == NULL) {
        xmlXPathErrMemory(NULL, "comparing nodesets\n");
		delete(hashs1);
		delete(values1);
		return(0);
    }
    hashs2 = MP_NEW_ARR( unsigned int, ns2->GetSize());
    if (hashs2 == NULL) {
        xmlXPathErrMemory(NULL, "comparing nodesets\n");
		delete(hashs1);
		delete(values1);
		delete(values2);
		return(0);
    }
    memset(values2, 0, ns2->GetSize() * sizeof(xmlChar *));
    for (i = 0;i < ns1->GetSize();i++) {
		hashs1[i] = mlXPathNodeValHash(ns1->nodeTab[i]);
		for (j = 0;j < ns2->GetSize();j++) {
			if (i == 0)
			hashs2[j] = mlXPathNodeValHash(ns2->nodeTab[j]);
			if (hashs1[i] != hashs2[j]) {
			if (neq) {
				ret = 1;
				break;
			}
			}
			else {
			if (values1[i] == NULL)
				values1[i] = mlNodeGetContent(ns1->nodeTab[i]);
			if (values2[j] == NULL)
				values2[j] = mlNodeGetContent(ns2->nodeTab[j]);
			ret = xmlStrEqual(values1[i], values2[j]) ^ (bool)neq;
			if (ret)
				break;
			}
		}
		if (ret)
			break;
    }
    for (i = 0;i < ns1->GetSize();i++)
	if (values1[i] != NULL)
	    delete(values1[i]);
    for (j = 0;j < ns2->GetSize();j++)
	if (values2[j] != NULL)
	    delete(values2[j]);
    delete(values1);
    delete(values2);
    delete(hashs1);
    delete(hashs2);
    return(ret);
}

int mlXPathCastNumberToBoolean (double val) {
     if (IsNaN(val) || (val == 0.0))
		 return(0);
     return(1);
}

double trio_pinf()
{
  // Cache the result 
  static double result = 0.0;

  if (result == 0.0) {
    
#if defined(INFINITY) && defined(__STDC_IEC_559__)
    result = (double)INFINITY;

#elif defined(USE_IEEE_754)
    result = trio_make_double(ieee_754_infinity_array);

#else
    /*
     * If HUGE_VAL is different from DBL_MAX, then HUGE_VAL is used
     * as infinity. Otherwise we have to resort to an overflow
     * operation to generate infinity.
     */
# if defined(TRIO_PLATFORM_UNIX)
    void (*signal_handler)(int) = signal(SIGFPE, SIG_IGN);
# endif

    result = HUGE_VAL;
    if (HUGE_VAL == DBL_MAX) {
      // Force overflow
      result += HUGE_VAL;
    }
    
# if defined(TRIO_PLATFORM_UNIX)
    signal(SIGFPE, signal_handler);
# endif

#endif
  }
  return result;
}

int trio_isinf (double number)
{
#if defined(TRIO_COMPILER_DECC) && !defined(__linux__)
  /*
   * DECC has an isinf() macro, but it works differently than that
   * of C99, so we use the fp_class() function instead.
   */
  return ((fp_class(number) == FP_POS_INF)
	  ? 1
	  : ((fp_class(number) == FP_NEG_INF) ? -1 : 0));

#elif defined(isinf)
  /*
   * C99 defines isinf() as a macro.
   */
  return isinf(number)
    ? ((number > 0.0) ? 1 : -1)
    : 0;
  
#elif defined(TRIO_COMPILER_MSVC) || defined(TRIO_COMPILER_BCB)
  /*
   * Microsoft Visual C++ and Borland C++ Builder have an _fpclass()
   * function that can be used to detect infinity.
   */
  return ((_fpclass(number) == _FPCLASS_PINF)
	  ? 1
	  : ((_fpclass(number) == _FPCLASS_NINF) ? -1 : 0));

#elif defined(USE_IEEE_754)
  /*
   * Examine IEEE 754 bit-pattern. Infinity must have a special exponent
   * pattern, and an empty mantissa.
   */
  int has_mantissa;
  int is_special_quantity;

  is_special_quantity = trio_is_special_quantity(number, &has_mantissa);
  
  return (is_special_quantity && !has_mantissa)
    ? ((number < 0.0) ? -1 : 1)
    : 0;

#else
  /*
   * Fallback solution.
   */
  int status;
  
# if defined(TRIO_PLATFORM_UNIX)
  void (*signal_handler)(int) = signal(SIGFPE, SIG_IGN);
# endif
  
  double infinity = trio_pinf();
  
  status = ((number == infinity)
	    ? 1
	    : ((number == -infinity) ? -1 : 0));
  
# if defined(TRIO_PLATFORM_UNIX)
  signal(SIGFPE, signal_handler);
# endif
  
  return status;
  
#endif
}

int IsInf(double val) {
    return(trio_isinf(val));
}

int mlXPathParserContext::EqualValuesCommon(mlXPathObjectPtr arg1, mlXPathObjectPtr arg2) {
    int ret = 0;
    //
     // At this point we are assured neither arg1 nor arg2
     // is a nodeset, so we can just pick the appropriate routine.
     
    switch (arg1->type) {
        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
	    xmlGenericError(xmlGenericErrorContext,
		    "Equal: undefined\n");
#endif
	    break;
        case XPATH_BOOLEAN:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: %d boolean %d \n",
			    arg1->boolval, arg2->boolval);
#endif
		    ret = (arg1->boolval == arg2->boolval);
		    break;
		case XPATH_NUMBER:
		    ret = (arg1->boolval ==
			   mlXPathCastNumberToBoolean(arg2->floatval));
		    break;
		case XPATH_STRING:
		    if ((arg2->stringval == NULL) ||
			(arg2->stringval[0] == 0)) ret = 0;
		    else 
			ret = 1;
		    ret = (arg1->boolval == ret);
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_NUMBER:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    ret = (arg2->boolval==
			   mlXPathCastNumberToBoolean(arg1->floatval));
		    break;
		case XPATH_STRING:
		    valuePush(arg2);
		    NumberFunction(1);
		    arg2 = valuePop();
		    // no break on purpose
		case XPATH_NUMBER:
		    // Hand check NaN and Infinity equalities 
		    if (IsNaN(arg1->floatval) ||
		    	    IsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (IsInf(arg1->floatval) == 1) {
		        if (IsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg1->floatval) == -1) {
			if (IsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg2->floatval) == 1) {
			if (IsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg2->floatval) == -1) {
			if (IsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_STRING:
	    switch (arg2->type) {
	        case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		    xmlGenericError(xmlGenericErrorContext,
			    "Equal: undefined\n");
#endif
		    break;
		case XPATH_BOOLEAN:
		    if ((arg1->stringval == NULL) ||
			(arg1->stringval[0] == 0)) ret = 0;
		    else 
			ret = 1;
		    ret = (arg2->boolval == ret);
		    break;
		case XPATH_STRING:
		    ret = xmlStrEqual(arg1->stringval, arg2->stringval);
		    break;
		case XPATH_NUMBER:
		    valuePush(arg1);
		    NumberFunction(1);
		    arg1 = valuePop();
		    // Hand check NaN and Infinity equalities
		    if (IsNaN(arg1->floatval) ||
		    	    IsNaN(arg2->floatval)) {
		        ret = 0;
		    } else if (IsInf(arg1->floatval) == 1) {
			if (IsInf(arg2->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg1->floatval) == -1) {
			if (IsInf(arg2->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg2->floatval) == 1) {
			if (IsInf(arg1->floatval) == 1)
			    ret = 1;
			else
			    ret = 0;
		    } else if (IsInf(arg2->floatval) == -1) {
			if (IsInf(arg1->floatval) == -1)
			    ret = 1;
			else
			    ret = 0;
		    } else {
		        ret = (arg1->floatval == arg2->floatval);
		    }
		    break;
		case XPATH_USERS:
		case XPATH_POINT:
		case XPATH_RANGE:
		case XPATH_LOCATIONSET:
		    TODO
		    break;
		case XPATH_NODESET:
		case XPATH_XSLT_TREE:
		    break;
	    }
	    break;
        case XPATH_USERS:
	case XPATH_POINT:
	case XPATH_RANGE:
	case XPATH_LOCATIONSET:
	    TODO
	    break;
	case XPATH_NODESET:
	case XPATH_XSLT_TREE:
	    break;
    }
     MP_DELETE( arg1);
     MP_DELETE( arg2);
    return(ret);
}

int mlXPathParserContext::EqualValues() {
    mlXPathObjectPtr arg1, arg2, argtmp;
    int ret = 0;

    if ((this == NULL) || (this->context == NULL)) return(0);
    arg2 = valuePop(); 
    arg1 = valuePop();
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    delete(arg1);
	else
	    delete(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if (arg1 == arg2) {
#ifdef DEBUG_EXPR
        xmlGenericError(xmlGenericErrorContext,
		"Equal: by pointer\n");
#endif
        return(1);
    }

    //
     // If either argument is a nodeset, it's a 'special case'
     
    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	//
	 // Hack it to assure arg1 is the nodeset
	 
	if ((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)) {
		argtmp = arg2;
		arg2 = arg1;
		arg1 = argtmp;
	}
	switch (arg2->type) {
	    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		xmlGenericError(xmlGenericErrorContext,
			"Equal: undefined\n");
#endif
		break;
	    case XPATH_NODESET:
	    case XPATH_XSLT_TREE:
		ret = mlXPathEqualNodeSets(arg1, arg2, 0);
		break;
	    case XPATH_BOOLEAN:
		if ((arg1->nodesetval == NULL) ||
		  (arg1->nodesetval->GetSize() == 0)) ret = 0;
		else 
		    ret = 1;
		ret = (ret == arg2->boolval);
		break;
	    case XPATH_NUMBER:
		ret = EqualNodeSetFloat(arg1, arg2->floatval, 0);
		break;
	    case XPATH_STRING:
		ret = mlXPathEqualNodeSetString(arg1, arg2->stringval, 0);
		break;
	    case XPATH_USERS:
	    case XPATH_POINT:
	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		TODO
		break;
	}
	delete(arg1);
	delete(arg2);
	return(ret);
    }

    return (EqualValuesCommon(arg1, arg2));
}

int mlXPathParserContext::NotEqualValues() {
    mlXPathObjectPtr arg1, arg2, argtmp;
    int ret = 0;

    if ((context == NULL)) return(0);
    arg2 = valuePop(); 
    arg1 = valuePop();
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    delete(arg1);
	else
	    delete(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if (arg1 == arg2) {
#ifdef DEBUG_EXPR
        xmlGenericError(xmlGenericErrorContext,
		"NotEqual: by pointer\n");
#endif
        return(0);
    }

    //
     // If either argument is a nodeset, it's a 'special case'
     
    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	//
	 // Hack it to assure arg1 is the nodeset
	 
	if ((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE)) {
		argtmp = arg2;
		arg2 = arg1;
		arg1 = argtmp;
	}
	switch (arg2->type) {
	    case XPATH_UNDEFINED:
#ifdef DEBUG_EXPR
		xmlGenericError(xmlGenericErrorContext,
			"NotEqual: undefined\n");
#endif
		break;
	    case XPATH_NODESET:
	    case XPATH_XSLT_TREE:
		ret = mlXPathEqualNodeSets(arg1, arg2, 1);
		break;
	    case XPATH_BOOLEAN:
		if ((arg1->nodesetval == NULL) ||
		  (arg1->nodesetval->GetSize() == 0)) ret = 0;
		else 
		    ret = 1;
		ret = (ret != arg2->boolval);
		break;
	    case XPATH_NUMBER:
		ret = EqualNodeSetFloat(arg1, arg2->floatval, 1);
		break;
	    case XPATH_STRING:
		ret = mlXPathEqualNodeSetString(arg1, arg2->stringval,1);
		break;
	    case XPATH_USERS:
	    case XPATH_POINT:
	    case XPATH_RANGE:
	    case XPATH_LOCATIONSET:
		TODO
		break;
	}
	delete(arg1);
	delete(arg2);
	return(ret);
    }

    return (!EqualValuesCommon(arg1, arg2));
}

double mlXPathCastNodeToNumber (mlNodePtr node) {
    xmlChar *strval;
    double ret;

    if (node == NULL)
	return(mlXPathNAN);
    strval = mlXPathCastNodeToString(node);
    if (strval == NULL)
	return(mlXPathNAN);
    ret = mlXPathCastStringToNumber(strval);
    delete(strval);

    return(ret);
}

int mlXPathCompareNodeSets(int inf, int strict,
	                mlXPathObjectPtr arg1, mlXPathObjectPtr arg2) {
    int i, j, init = 0;
    double val1;
    double *values2;
    int ret = 0;
    mlNodeSetPtr ns1;
    mlNodeSetPtr ns2;

    if ((arg1 == NULL) ||
	((arg1->type != XPATH_NODESET) && (arg1->type != XPATH_XSLT_TREE))) {
		delete(arg2);
        return(0);
    }
    if ((arg2 == NULL) ||
	((arg2->type != XPATH_NODESET) && (arg2->type != XPATH_XSLT_TREE))) {
		delete(arg1);
		delete(arg2);
        return(0);
    }

    ns1 = arg1->nodesetval;
    ns2 = arg2->nodesetval;

    if ((ns1 == NULL) || (ns1->GetSize() <= 0)) {
		delete(arg1);
		delete(arg2);
		return(0);
    }
    if ((ns2 == NULL) || (ns2->GetSize() <= 0)) {
		delete(arg1);
		delete(arg2);
		return(0);
    }

    values2 = MP_NEW_ARR( double, ns2->GetSize());
    if (values2 == NULL) {
        xmlXPathErrMemory(NULL, "comparing nodesets\n");
		delete(arg1);
		delete(arg2);
		return(0);
    }
    for (i = 0;i < ns1->GetSize();i++) {
		val1 = mlXPathCastNodeToNumber(ns1->nodeTab[i]);
		if (IsNaN(val1))
			continue;
		for (j = 0;j < ns2->GetSize();j++) {
			if (init == 0) {
			values2[j] = mlXPathCastNodeToNumber(ns2->nodeTab[j]);
			}
			if (IsNaN(values2[j]))
			continue;
			if (inf && strict) 
			ret = (val1 < values2[j]);
			else if (inf && !strict)
			ret = (val1 <= values2[j]);
			else if (!inf && strict)
			ret = (val1 > values2[j]);
			else if (!inf && !strict)
			ret = (val1 >= values2[j]);
			if (ret)
			break;
		}
		if (ret)
			break;
		init = 1;
    }
    MP_DELETE_ARR( values2);
    MP_DELETE(arg1);
    MP_DELETE(arg2);
    return(ret);
}

int mlXPathParserContext::CompareNodeSetFloat(int inf, int strict,
	                    mlXPathObjectPtr arg, mlXPathObjectPtr f) {
    int i, ret = 0;
    mlNodeSetPtr ns;
    xmlChar *str2;

    if ((f == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE))) {
		delete(arg);
		delete(f);
        return(0);
    }
    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i = 0;i < ns->GetSize();i++) {
	     str2 = mlXPathCastNodeToString(ns->nodeTab[i]);
	     if (str2 != NULL) {
		 valuePush(mlXPathNewString(str2));
		  MP_DELETE( str2);
		 NumberFunction(1);
		 valuePush(mlXPathObjectCopy(f));
		 ret = CompareValues(inf, strict);
		 if (ret)
		     break;
	     }
	}
    }
    delete(arg);
    delete(f);
    return(ret);
}

int mlXPathParserContext::CompareNodeSetString(int inf, int strict,
	                    mlXPathObjectPtr arg, mlXPathObjectPtr s) {
    int i, ret = 0;
    mlNodeSetPtr ns;
    xmlChar *str2;

    if ((s == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE))) {
		delete(arg);
		delete(s);
        return(0);
    }
    ns = arg->nodesetval;
    if (ns != NULL) {
	for (i = 0;i < ns->GetSize();i++) {
	     str2 = mlXPathCastNodeToString(ns->nodeTab[i]);
	     if (str2 != NULL) {
		 valuePush(mlXPathNewString(str2));
		 delete(str2);
		 valuePush(mlXPathObjectCopy(s));
		 ret = CompareValues(inf, strict);
		 if (ret)
		     break;
	     }
	}
    }
    delete(arg);
    delete(s);
    return(ret);
}

int mlXPathParserContext::CompareNodeSetValue(int inf, int strict,
	                    mlXPathObjectPtr arg, mlXPathObjectPtr val) {
    if ((val == NULL) || (arg == NULL) ||
	((arg->type != XPATH_NODESET) && (arg->type != XPATH_XSLT_TREE)))
        return(0);

    switch(val->type) {
        case XPATH_NUMBER:
	    return(CompareNodeSetFloat(inf, strict, arg, val));
        case XPATH_NODESET:
        case XPATH_XSLT_TREE:
	    return(mlXPathCompareNodeSets(inf, strict, arg, val));
        case XPATH_STRING:
	    return(CompareNodeSetString(inf, strict, arg, val));
        case XPATH_BOOLEAN:
	    valuePush(arg);
	    BooleanFunction(1);
	    valuePush(val);
	    return(CompareValues(inf, strict));
	default:
	    TODO
    }
    return(0);
}

int mlXPathParserContext::CompareValues(int inf, int strict) {
    int ret = 0, arg1i = 0, arg2i = 0;
    mlXPathObjectPtr arg1, arg2;

    if ((context == NULL)) return(0);
    arg2 = valuePop(); 
    arg1 = valuePop();
    if ((arg1 == NULL) || (arg2 == NULL)) {
	if (arg1 != NULL)
	    delete(arg1);
	else
	    delete(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }

    if ((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE) ||
      (arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
	if (((arg2->type == XPATH_NODESET) || (arg2->type == XPATH_XSLT_TREE)) &&
	  ((arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE))){
	    ret = mlXPathCompareNodeSets(inf, strict, arg1, arg2);
	} else {
	    if ((arg1->type == XPATH_NODESET) || (arg1->type == XPATH_XSLT_TREE)) {
		ret = CompareNodeSetValue(inf, strict,
			                          arg1, arg2);
	    } else {
		ret = CompareNodeSetValue(!inf, strict,
			                          arg2, arg1);
	    }
	}
	return(ret);
    }

    if (arg1->type != XPATH_NUMBER) {
	valuePush(arg1);
	NumberFunction(1);
	arg1 = valuePop();
    }
    if (arg1->type != XPATH_NUMBER) {
	delete(arg1);
	delete(arg2);
	XP_ERROR0(XPATH_INVALID_OPERAND);
    }
    if (arg2->type != XPATH_NUMBER) {
		valuePush(arg2);
		NumberFunction(1);
		arg2 = valuePop();
    }
    if (arg2->type != XPATH_NUMBER) {
		delete(arg1);
		delete(arg2);
		XP_ERROR0(XPATH_INVALID_OPERAND);
    }
    //
     // Add tests for infinity and nan
     // => feedback on 3.4 for Inf and NaN
     
    // Hand check NaN and Infinity comparisons 
    if (IsNaN(arg1->floatval) || IsNaN(arg2->floatval)) {
	ret=0;
    } else {
	arg1i=IsInf(arg1->floatval);
	arg2i=IsInf(arg2->floatval);
	if (inf && strict) {
	    if ((arg1i == -1 && arg2i != -1) ||
		(arg2i == 1 && arg1i != 1)) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval < arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (inf && !strict) {
	    if (arg1i == -1 || arg2i == 1) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval <= arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (!inf && strict) {
	    if ((arg1i == 1 && arg2i != 1) ||
		(arg2i == -1 && arg1i != -1)) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval > arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
	else if (!inf && !strict) {
	    if (arg1i == 1 || arg2i == -1) {
		ret = 1;
	    } else if (arg1i == 0 && arg2i == 0) {
		ret = (arg1->floatval >= arg2->floatval);
	    } else {
		ret = 0;
	    }
	}
    }
    delete(arg1);
    delete(arg2);
    return(ret);
}

void mlXPathParserContext::RunEval() {
    mlXPathCompExpr* lcomp;

    if ((this->comp == NULL))
		return;

    if (valueTab == NULL) {
		// Allocate the value stack 
		this->valueTab = MP_NEW_ARR( mlXPathObjectPtr, 10);
		if (this->valueTab == NULL) {
//			xmlXPathPErrMemory(ctxt, "creating evaluation context\n");
		}
		valueNr = 0;
		valueMax = 10;
		value = NULL;
    }
    lcomp = this->comp;
    if(comp->last < 0) {
//		mlGenericError(xmlGenericErrorContext,
//		    "xmlXPathRunEval: last is less than zero\n");
		return;
    }
    CompOpEval(&comp->steps[comp->last]);
}

bool mlXPathParserContext::GetResult(mlNodeVect& avNodes){
    mlXPathObjectPtr res, tmp, init = NULL;
    if (value == NULL) {
//		mlGenericError(xmlGenericErrorContext,
//			"xmlXPathEval: evaluation failed\n");
		res = NULL;
    } else if ((*cur != 0) && (comp != NULL)) {
//		xmlXPatherror(ctxt, __FILE__, __LINE__, XPATH_EXPR_ERROR);
		res = NULL;
    } else {
		res = valuePop();
    }

    int stack = 0;
    do {
        tmp = valuePop();
		if (tmp != NULL) {
		    if (tmp != init)
				stack++;    
			 MP_DELETE( tmp);
        }
    } while (tmp != NULL);
    if ((stack != 0) && (res != NULL)) {
//		xmlGenericError(xmlGenericErrorContext,
//			"xmlXPathEval: %d object left on the stack\n",
//				stack);
    }
    if (error != XPATH_EXPRESSION_OK) {
		 MP_DELETE( res);
		res = NULL;
    }

	avNodes.clear();
	if(res != NULL && res->nodesetval != NULL){
		mlNodeSetPtr nodeset = res->nodesetval;
		mlNodeVect::const_iterator cvi;
		for(cvi = nodeset->nodeTab.begin(); cvi != nodeset->nodeTab.end(); cvi++){
			avNodes.push_back(*cvi);
		}

	}
    return (res != NULL);
}

bool mlIXMLNode::select(const wchar_t* apwcXPathExpr, mlNodeVect& avNodes){
	// создаем контекст
	JSObject* jsoDoc = GetDocument();
	mlXMLDocument* pXMLDoc=(mlXMLDocument*)JS_GetPrivate(GetJSContext(), jsoDoc);
	MP_NEW_P( ctx, mlXPathContext, pXMLDoc);
	// парсим выражение
	MP_NEW_P2( pctxt, mlXPathParserContext, apwcXPathExpr, ctx);
	pctxt->CompileExpr(1);
	// вычисляем выражение
	pctxt->RunEval();
	// возвращаем результат
	bool bRes = pctxt->GetResult(avNodes);
	 MP_DELETE( pctxt);
	 MP_DELETE( ctx);
	return bRes;
}

}
