#pragma once

#include <istream>
#include "mlBuildEvalCtxt.h"

namespace rmml {

/**
 * Заглушка XML-буфера через MFC CFile (надо через потоки сделать)
 * Используется mlRMMLBuilder-ом
 */
/*
struct mlXMLBuffer : CFile{
   void Write( const char* pszBuffer, UINT uCount, UINT* puCount){
	   if(uCount==0) return;
	   CFile::Write(pszBuffer, uCount);
//      gd::CUTF8String stringUTF8;
//      stringUTF8.Add( pszBuffer, uCount );
//      CFile::Write( stringUTF8.GetUTF8Text(), stringUTF8.GetLength() );
   } // if //_DEBUG// Cxml1Doc commandsclass CFileBuffer : CFile{void Write( const TCHAR* pszBuffer, UINT uCount, UINT* puCount ){

	UINT Read( char* pszBuffer, UINT uCount, UINT* puCount ){
//		char* pBuff=new char[uCount+1];
		UINT uiRead=CFile::Read(pszBuffer, uCount);
//		::MultiByteToWideChar(CP_UTF8,0,pBuff,uiRead,pszBuffer,uCount);
//		 MP_DELETE( pBuff;
		if(puCount!=NULL) *puCount=uiRead;
		return uiRead;
	}

};
*/

/**
 * Построитель всех RMML-ных структур (сцен, композиций, элементов, ...).
 * Используется SceneLoader-ом
 */
class mlRMMLBuilder
{
friend class mlSceneManager;
friend class mlScriptPreloader::mlIncludeInfoInternal;
	JSContext *cx;
	mlSceneManager* mpSceneManager;
	bool mbIn3DScene;
	bool mbInViewport;
	mlXMLArchive* mpCurAr;
	mlSceneManager::mlLibXml2Funcs* mpLibXml2Funcs;
	mlBuildEvalCtxt* mpBuildEvalCtxt;
	int miElemCount; // счетчик количества созданных RMML-элементов для периодического вызова GC
	//
#ifdef RMML_USE_LIBXML2
    MP_WSTRING msIncludeSrc;
    xmlParserCtxtPtr mpParserCtxt;
    mlRMMLComposition* mpLastScene;
    MP_VECTOR<mlRMMLElement*> mvParents;
    MP_WSTRING msValue;
	MP_WSTRING msUnkonownTagName;
	MP_VECTOR<MP_WSTRING> mvIncludes;	// список подключенных в загружаемый rmml-файлов

	mlRMMLElement* mpRootElement;	// если != NULL, то это элемент, к которому подцеплять в качестве дочерних корневые композиции загружаемого RMML-файла
	//
//	unsigned hmLibXml2;
	//
public:
    // SAX1
    static void characters(void* ctx ATTRIBUTE_UNUSED, const xmlChar* ch, int len);
    static void warningDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...);
    static void errorDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...);
    static void fatalErrorDebug(void* ctx ATTRIBUTE_UNUSED, const char* msg, ...);
    // SAX2
    static void startElementNs(void* ctx ATTRIBUTE_UNUSED,
            const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
            int nb_namespaces, const xmlChar** namespaces, int nb_attributes,
            int nb_defaulted, const xmlChar** attributes);
    static void endElementNs(void* ctx ATTRIBUTE_UNUSED,
            const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI);
    // XML Input Buffer
    static int InputReadCallback(void * context, char * buffer, int len);
    static int InputCloseCallback(void * context);
    xmlParserInputPtr CreateInputStream(xmlParserCtxtPtr ctxt, mlXMLArchive* pAr, xmlCharEncoding encoding=XML_CHAR_ENCODING_NONE);
    xmlParserCtxtPtr CreateStreamParserCtxt(mlXMLArchive* pAr);
    int SAXUserParseStream(xmlSAXHandlerPtr sax, mlXMLArchive* apAr);
	void FormatErrorString(char* pszErr, unsigned int bufferSize);
public:
	mlresult Include(const wchar_t* apwcSrc);
#endif

	void SetScene(mlRMMLComposition* apNewScene, bool abUI, const wchar_t* apwcSrc);
public:
	mlRMMLBuilder(JSContext* acx, bool abUI=false);
	~mlRMMLBuilder(void);

	mlresult CreateElementByTag(JSContext* cx, const wchar_t* apwcTagName, mlRMMLElement* &apElem);
	mlresult Build(mlXMLArchive* apAr, mlSceneManager* apSceneManager, bool abUI);
	mlresult Build(const wchar_t* apwcSrc, mlSceneManager* apSceneManager, bool abUI, mlRMMLElement* apRootElem);
	mlresult Save(mlXMLArchive* apAr, mlSceneManager* apSceneManager);
	void SetIn3DSceneFlag(bool abIn3DScene){ mbIn3DScene = abIn3DScene; }
private:
	void SetSceneManager(mlSceneManager* apSceneManager){ mpSceneManager = apSceneManager; }
	// специально для mlScriptPreloader::mlScriptInfoInternal
	void SetParserContext(xmlParserCtxtPtr apParserCtxt){ mpParserCtxt = apParserCtxt; }
};

}
