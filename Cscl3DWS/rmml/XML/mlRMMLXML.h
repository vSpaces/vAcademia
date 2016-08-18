#pragma once

#include "mlRMMLXMLNode.h"
#include "mlRMMLXMLNodeList.h"
#include "mlRMMLXMLAttr.h"
#include "mlRMMLXMLText.h"
#include "mlRMMLXMLCDATA.h"
#include "mlRMMLXMLComment.h"
#include "mlRMMLXMLNamedNodeMap.h"
#include "mlRMMLXMLElement.h"
#include "mlRMMLXMLDocument.h"
#include "../../Comman/ComManRes.h"

using namespace res;
namespace rmml {

struct mlXMLBuf{
	virtual void Write( const wchar_t* pszBuffer, unsigned uCount, unsigned* puCount)=0;
	virtual unsigned Read( wchar_t* pszBuffer, unsigned uCount, unsigned* puCount )=0;
};

struct mlXMLBuffer:public mlXMLBuf{
	enum Encoding{
		UNKNOWN,
		DEFAULT,
		UTF8,
		WIN1251
	} enc;
	bool mbHeader;
	std::istream* is;
	std::iostream* ios;
	mlXMLBuffer(std::istream* ais, Encoding aenc=DEFAULT){ is=ais; ios=NULL; enc=aenc; mbHeader=true; }
	mlXMLBuffer(std::iostream* aios, Encoding aenc=DEFAULT){ ios=aios; is=NULL; enc=aenc; mbHeader=true; }
	void Write( const wchar_t* pszBuffer, unsigned uCount, unsigned* puCount);
	unsigned Read( wchar_t* pszBuffer, unsigned uCount, unsigned* puCount );
private:
	void DefineEncoding(wchar_t* pwszStart, wchar_t* pwszEnd);
};

// for LibXML2 parsing
struct mlXMLBufC{ 
	virtual void Write(const char* pszBuffer, unsigned uCount, unsigned* puCount)=0;
	virtual unsigned Read(char* pszBuffer, unsigned uCount, unsigned* puCount )=0;
	virtual JSContext* GetJSContext()=0;
};

struct mlXMLBufferRaw: mlXMLBufC{
	std::istream* is;
	std::iostream* ios;
	JSContext* mcx;
	mlXMLBufferRaw(JSContext* cx, std::istream* ais){ mcx = cx; is=ais; ios=NULL; }
	void Write( const char* pszBuffer, unsigned uCount, unsigned* puCount);
	unsigned Read( char* pszBuffer, unsigned uCount, unsigned* puCount );
	JSContext* GetJSContext(){ return mcx; }
};

/**
 * Класс XML-элемента RMML
 */

class mlRMMLXML :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public IResourceNotify
{
	JSObject* jsoXMLDocument;
	res::resIResource* mpRes;
public:
	mlRMMLXML(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLXML(void);
	static mlresult InitJSClasses(JSContext* cx , JSObject* obj);
	static JSObject* CreateElementByClassName(JSContext* cx, const char* apszClassName);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_XMLDocument = 1,
		JSPROP_value,
		JSPROP_asynchronous	// XML-файл должен загружаться асинхронно?
	};

	bool asynchronous;

	CRITICAL_SECTION m_loadAsyncCallback;
	void CreateNewXMLDocument();
	mlresult ParseXML(mlXMLBuf* apXMLBuf);

#ifdef RMML_USE_LIBXML2
	mlresult ParseXML(mlXMLBufC* apXMLBuf);
#endif	

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult SetValue(wchar_t* apwcValue);
	virtual void ResourceLoadedAsynchronously(mlSynchData &Data);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация moILoadable
protected:
	// Свойства
	bool SrcChanged();
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация IResourceNotify
	long onResourceNotify( IResource* aResource, DWORD aNotifyCode, DWORD aNotifyServerCode, wchar_t *aErrorText);
	bool isNeedDestroyByComman(){return false;};
	void setAsynchResource(void* aAsynch){};	

};

}