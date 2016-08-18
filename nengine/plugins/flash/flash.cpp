DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )// flash.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "flash.h"
#include <Assert.h>
#include "ifile.h"

#include <malloc.h>

#include "zlib.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


CFlash::CFlash(IFlashCallBack* apCallBack, HWND ahwndParentWindow, IResLibrary* apResLibrary):
m_callBack(apCallBack),
m_resLib(apResLibrary),
m_initialized(false),
m_colorChannelCount(0),
m_frameData(NULL),
m_height(0),
m_width(0),
m_textureWidth(0),
m_textureHeight(0),
m_bitmap(0),
m_hdc(0),
m_updateBitmap(NULL),
m_updateHDC(NULL),
m_cs(NULL),
m_instance(NULL),
m_parentWindow(ahwndParentWindow),
m_needToUpdate(false),
m_isWindowed(true),
m_isTransparent(true)
{
	m_cs = new CRITICAL_SECTION;
	InitializeCriticalSection(m_cs);

	m_hdc = CreateCompatibleDC(0);

	m_instance = &m_instanceStruct;

	if(m_resLib != NULL)
		m_asynchResMan = m_resLib->GetAsynchResMan();

	for (int k = 0; k < 4; k++)
	{
		m_userData[k] = NULL;
	}
}

void* 	CFlash::GetUserData(unsigned int userDataID)
{
	if (userDataID < 4)
	{
		return m_userData[userDataID];
	}
	else
	{
		return NULL;
	}
}

void 	CFlash::SetUserData(unsigned int userDataID, void* data)
{
	if (userDataID < 4)
	{
		m_userData[userDataID] = data;
	}
}

CFlash::~CFlash()
{
	// освободить ресурсы из m_mapAsynchRes
	MapAsynchRes::const_iterator cit = m_mapAsynchRes.begin();
	for(; cit != m_mapAsynchRes.end(); cit++){
		FlashStream fs = cit->second;
		m_asynchResMan->RemoveLoadHandler(this, m_resLib);
		fs.asynchRes->Release();
		gFlashNP.m_pluginFuncs.destroystream(m_instance, fs.stream, NPRES_DONE);
		delete fs.stream;
	}
	

	if(m_oldBitmap != NULL)
		::SelectObject(m_hdc, m_oldBitmap);

	::DeleteObject(m_bitmap);
	m_frameData = NULL;

	::DeleteDC(m_hdc);

	::DeleteCriticalSection(m_cs);
	delete m_cs;
}

// Возвращает динамическую текстуру
IDynamicTexture* CFlash::GetDynamicTexture(){
	return this;
}

// установить размер "окна" браузера
bool CFlash::SetSize(int aiWidth, int aiHeight){
	CGuardByCS oGCS(m_cs);
	m_width = aiWidth;
	m_height = aiHeight;
	SetTextureSize(aiWidth, aiHeight);
	return true;
}

// Обновить состояние объекта (не текстуру)
void CFlash::Update(){
	// сохранить текущие значения свойств, чтобы использовать их в UpdateFrame()
	m_updateWidth = m_width;
	m_updateHeight = m_height;
	m_updateTextureWidth = m_textureWidth;
	m_updateTextureHeight = m_textureHeight;
	//
	m_updateBitmap = m_bitmap;
	m_updateHDC = m_hdc;
	// ??
	if(!m_initialized){
		CGuardByCS oGCS(m_cs);

		// первый раз отрисовываем в главном потоке, 
		// чтобы m_webView инициализировался (а надо чтобы он инициализировался именно в главном потоке)
		UpdateFrame();

		// ??

		m_initialized = true;
	}else{
	}
}

// Обновить текстуру
void CFlash::UpdateFrame(){
//	::SelectObject(m_updateHDC, m_updateBitmap);
static bool gbOnceCalled = false;
	if(!m_needToUpdate)
		return;
	m_needToUpdate = false;

//static int giCntr = 0;
//if(giCntr > 0){
//giCntr--;
//return;
//}
//giCntr = 20;
	// ??

	// отрисовка

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = m_width;
	rc.bottom = m_height;
	RECTL rect = {rc.left, rc.top, rc.right, rc.bottom};
	//if(!gbOnceCalled){
	//m_paintAll = true;
//::FillRect(m_updateHDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
	::SelectClipRgn(m_updateHDC, NULL);

	NPEvent npEvent;

	WINDOWPOS windowpos;
	memset(&windowpos, 0, sizeof(windowpos));

	windowpos.x = 0;
	windowpos.y = 0;
	windowpos.cx = m_width;
	windowpos.cy = m_height;

	npEvent.event = WM_WINDOWPOSCHANGED;
	npEvent.lParam = reinterpret_cast<uint32>(&windowpos);
	npEvent.wParam = 0;

	int16 iRes1 = gFlashNP.m_pluginFuncs.event(m_instance, &npEvent);

	m_npWindow.type = NPWindowTypeDrawable;
	m_npWindow.window = m_updateHDC;

	m_npWindow.x = 0;
	m_npWindow.y = 0;

	m_npWindow.width = m_width;
	m_npWindow.height = m_height;

	m_npWindow.clipRect.left = 0;
	m_npWindow.clipRect.top = 0;
	m_npWindow.clipRect.right = m_width;
	m_npWindow.clipRect.bottom = m_height;

	if (gFlashNP.m_pluginFuncs.setwindow) {
		NPError npErr = gFlashNP.m_pluginFuncs.setwindow(m_instance, &m_npWindow);
		int hh=0;
	}

	npEvent.event = WM_PAINT;
	npEvent.wParam = reinterpret_cast<uint32>(m_hdc);
	npEvent.lParam = 0;

	int16 iRes = gFlashNP.m_pluginFuncs.event(m_instance, &npEvent);

//CreateBMPFile(NULL, "c:\\bmp.bmp", m_updateBitmap, m_updateHDC);

/*
	HPEN pen = ::CreatePen(PS_SOLID, 2, RGB(255,128,128));
	HPEN penOld = (HPEN)::SelectObject(m_updateHDC, pen);
	::MoveToEx(m_updateHDC, 10, 10,NULL);
	::LineTo(m_updateHDC, 100, 100);
	::SelectObject(m_updateHDC, penOld);
	::DeleteObject(pen);
*/
	// ??
}

// Вернуть ссылку на массив с построчным содержимым кадра	
void* CFlash::GetFrameData(){
	return m_frameData;
}

// Вернуть ширину исходной картинки в пикселях
unsigned short CFlash::GetSourceWidth(){
	return m_width;
}

// Вернуть высоту исходной картинки в пикселях
unsigned short CFlash::GetSourceHeight(){
	return m_height;
}

// Вернуть ширину результирующей текстуры в пикселях
unsigned short CFlash::GetTextureWidth(){
	return m_textureWidth;
}

// Вернуть высоту результирующей текстуры в пикселях
unsigned short CFlash::GetTextureHeight(){
	return m_textureHeight;
}

// Установить размер результирующей текстуры
void CFlash::SetTextureSize(int width, int height){
//	assert(width > 0);
//	assert(height > 0);

	if ((width <= 0) || (height <= 0))
	{
		return;
	}

	if ((m_textureWidth != width) || (m_textureHeight != height))
	{
		if (m_bitmap)
		{
			if(m_oldBitmap != NULL)
				::SelectObject(m_hdc, m_oldBitmap);
			DeleteObject(m_bitmap);
		}

		BITMAPINFOHEADER bmih;
		bmih.biSize = sizeof (BITMAPINFOHEADER); 
		bmih.biPlanes = 1;
		bmih.biBitCount = 24;
		if(m_isTransparent)
			bmih.biBitCount = 32;
		bmih.biWidth = width;     
		bmih.biHeight = height;     
		bmih.biCompression = BI_RGB;  
		m_bitmap = CreateDIBSection(m_hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&m_frameData), NULL, NULL);
		::SelectClipRgn(m_hdc, NULL);
		m_oldBitmap = ::SelectObject(m_hdc, m_bitmap);
		m_updateBitmap = m_bitmap;
		m_updateHDC = m_hdc;
		m_paintAll = true;
		m_needToUpdate = true;
		UpdateFrame();
	}

	m_textureWidth = width;
	m_textureHeight = height;
}

// Вернуть количество цветовых каналов, 3 для RGB, 4 для RGBA
unsigned char CFlash::GetColorChannelCount(){
	if(m_isTransparent)
		return 4;
	return m_colorChannelCount;
}

// Порядок цветовых каналов прямой (RGB) или обратный (BGR)
bool CFlash::IsDirectChannelOrder(){
	// ??
	return false;
}

// Нужно ли обновлять текстуру (или сейчас её содержимое не изменяется?)
bool CFlash::IsNeedUpdate(){
	// ??
	return true;
}

void CFlash::Destroy(){
	gFlashNP.DestroyInstance(this);
//	delete this;
}

bool CFlash::SetFocus(){
/*
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->SetActiveState(true);
	return SUCCEEDED(hr);
*/
	return false;
}

bool CFlash::KillFocus(){
/*
	if(m_webView == NULL || !m_initialized)
		return false;
	CGuardByCS oGCS(m_cs);
	HRESULT hr = m_webView->SetActiveState(false);
	return SUCCEEDED(hr);
*/
	return false;
}

// получает прогресс загрузки объекта
void CFlash::OnAsynchResourceLoadedPersent(IAsynchResource* asynch, unsigned char percent){
/*
	MapAsynchRes::const_iterator cit = m_mapAsynchRes.find(asynch);
	if(cit == m_mapAsynchRes.end()){
		assert(false);
		return; 
	}
	// ?? отправить загруженную часть потока во flash
	// ??
*/
}

// объект загружен
void CFlash::OnAsynchResourceLoaded(IAsynchResource* asynch){
	MapAsynchRes::iterator it = m_mapAsynchRes.find(asynch);
	if(it == m_mapAsynchRes.end()){
		assert(false);
		return; 
	}
	// отправить загруженную часть потока во flash
	FlashStream& flashStream = it->second;
	NPStream* stream = flashStream.stream;
	//typedef int32 (*NPP_WriteProcPtr)(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
//	unsigned int uiSize = asynch->GetSize();
	ifile* file = asynch->GetIFile();
	if(file != NULL){
		loadAndNotify(file, stream, flashStream.sendNotification, flashStream.notifyData);
		file->close();
	}
	// и сказать flash-у, что поток закончился
	gFlashNP.m_pluginFuncs.destroystream(m_instance, stream, NPRES_DONE);
	delete stream;
	// отрелизить ресурс
	asynch->Release();
	// удалить запись из m_mapAsynchRes
	m_mapAsynchRes.erase(it);
}

// ошибка загрузки
void CFlash::OnAsynchResourceError(IAsynchResource* asynch){
	MapAsynchRes::iterator it = m_mapAsynchRes.find(asynch);
	if(it == m_mapAsynchRes.end()){
//		assert(false);
		return; 
	}
	NPStream* stream = it->second.stream;
	// отрелизить ресурс
	asynch->Release();
	// и сказать flash-у, что поток умер
	gFlashNP.m_pluginFuncs.destroystream(m_instance, stream, NPRES_NETWORK_ERR);
	delete stream;
	// удалить запись из m_mapAsynchRes
	m_mapAsynchRes.erase(it);
}

NPError CFlash::loadAndNotify(ifile* apFile, NPStream* pStream, bool sendNotification, void* notifyData){
	NPReason npRes = NPRES_NETWORK_ERR;
	NPError npErrRet = NPERR_NO_DATA;
	unsigned long uiSize = 0;
	const void* buf = apFile->get_file_data( &uiSize);
	// прочитать заголовок SWF-ки и взять у него размер если это главная SWF-ка
	CFlashHeader flashHeader(buf, uiSize);
	long player_version = gFlashNP.m_pluginFuncs.version;
	player_version >>= 16;
	if (player_version < flashHeader._version)
	{   // Проинформировать пользователя о не правильной версии
		// установленного флеш-плеера
		// ??
	}
	int width = m_width;
	int height = m_height;
	int iNewWidth = flashHeader._x_max/20;
	int iNewHeight = flashHeader._y_max/20;
	m_callBack->OnAdjustSize(iNewWidth, iNewHeight);
	// для Flash-мувиков применять родные размеры
	if(m_width != iNewWidth || m_height != iNewHeight){
		m_width = iNewWidth;
		m_height = iNewHeight;
		SetTextureSize(m_width, m_height);
	}

	if(buf != NULL)
	{
		void*	tempBuffer = malloc( uiSize);
		if( tempBuffer)
		{
			memcpy( tempBuffer, buf, uiSize);
		int32 iReadySize = gFlashNP.m_pluginFuncs.writeready(m_instance, pStream);
			int32 iRet = gFlashNP.m_pluginFuncs.write(m_instance, pStream, 0, uiSize, tempBuffer);
			free( tempBuffer);
		}
		apFile->free_file_data();
		npRes = NPRES_DONE;
		npErrRet = NPERR_NO_ERROR;
	}
	if(sendNotification){
		gFlashNP.m_pluginFuncs.urlnotify(m_instance, pStream->url, npRes, notifyData);
	}
	return npErrRet;
}

NPError CFlash::load(const char* url, const char* target, bool sendNotification, void* notifyData){
	std::string sSRC = url; // путь к ресурсу

	if(sSRC.find("javascript:") == 0)
		return NPERR_NO_ERROR;

//	std::string::size_type posComma = sSRC.find("|");
//	if(posComma != std::string::npos){
//		sSRC.replace(posComma, 1, ":");
//	}
	
//	std::string::size_type posPrefix = sSRC.find("file:///a:\\");
//	if(posPrefix == 0){
//		sSRC.erase(0, strlen("file:///a:\\"));
//	}

	NPStream* pStream = new NPStream;
	memset(pStream, 0, sizeof(NPStream));
	pStream->url = url;

	uint16 stype = NP_NORMAL;
	NPMIMEType pluginType = "application/x-shockwave-flash";
	NPError npErr = gFlashNP.m_pluginFuncs.newstream(m_instance, pluginType, pStream, false, &stype);

	IAsynchResource* pAsynchRes = m_asynchResMan->GetAsynchResource(sSRC.c_str(), RES_TYPE_TEXTURE, this, m_resLib);
	if(pAsynchRes != NULL){
		typedef std::pair <IAsynchResource*, FlashStream> ARMapPair;
		m_mapAsynchRes.insert(ARMapPair(pAsynchRes, FlashStream(pStream, pAsynchRes, sendNotification, notifyData)));
		return NPERR_NO_ERROR;
	}
	// возможно ресурс где-то локально размещен
	ifile* file =  m_resLib->GetResMemFile(sSRC.c_str());
	if(file != NULL){
		NPError npErr = loadAndNotify(file, pStream, sendNotification, notifyData);
		m_resLib->DeleteResFile(file);
		delete pStream;
		return npErr;
	}
	delete pStream;
	return NPERR_FILE_NOT_FOUND;
}

bool CFlash::LoadMovie(const wchar_t* apwcSrc){
//return false;
	CGuardByCS oGCS(m_cs);
	if(apwcSrc == NULL)
		return false;
	//typedef NPError (*NPP_SetValueProcPtr)(NPP instance, NPNVariable variable, void *value);
	//bool bWindow = false;
	//NPError npErr = gFlashNP.m_pluginFuncs.setvalue(m_instance, NPPVpluginWindowBool, (void*)&bWindow);
//	gFlashNP.m_pluginFuncs
//	int iFlashVersion = LoadFlashPlugin();
	NPObject* object = NULL;
	NPError npErr;
	npErr = gFlashNP.m_pluginFuncs.getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
	if(npErr != NPERR_NO_ERROR || object == NULL)
		return false;

	bool bRet = false;
	if(object->_class->invoke != NULL){
		NPIdentifier ident = _NPN_GetStringIdentifier("LoadMovie");
		if (object->_class->hasMethod(object, ident)){
			NPVariant args[2];
			args[0].type = NPVariantType_Int32;
			args[0].value.intValue = 0;
			args[1].type = NPVariantType_String;
			std::string sSRC = cLPCSTR(apwcSrc); // путь к ресурсу
			std::string sURL = sSRC; // URL как идентификатор стрима
			if(sURL[0] == ':')
				sURL = sURL.substr(1);
			if(sURL.find("http://") == std::string::npos){
//				std::string::size_type pos = sURL.find(":");
//				if(pos != std::string::npos)
//					sURL.replace(pos, 1, "|");
//				sURL.insert(0, "a:\\");
			}
			NPString npsURL = { (const NPUTF8*)sURL.c_str(), static_cast<uint32_t>(sURL.size()) };
			NPN_InitializeVariantWithStringCopy(&(args[1]), &npsURL);
			NPVariant result;
			bRet = object->_class->invoke(object, ident, args, 2, &result);
			_NPN_ReleaseVariantValue(&args[1]);
/*
			NPStream* pStream = new NPStream;
			memset(pStream, 0, sizeof(NPStream));
			pStream->url = sURL.c_str();

			uint16 stype = NP_NORMAL;
			NPMIMEType pluginType = "application/x-shockwave-flash";
			npErr = gFlashNP.m_pluginFuncs.newstream(m_instance, pluginType, pStream, false, &stype);

			IAsynchResource* pAsynchRes = m_asynchResMan->GetAsynchResource(sSRC.c_str(), RES_TYPE_TEXTURE, this, m_resLib);

			typedef std::pair <IAsynchResource*, FlashStream> ARMapPair;
			m_mapAsynchRes.insert(ARMapPair(pAsynchRes, FlashStream(pStream, pAsynchRes)));
int hh=0;
*/

/*
std::string sSrcFile = "C:/vu2008/ui/k01_2.swf";
std::string sSrc = "file:///C:/vu2008/ui/k01_2.swf"; // "C:/vu2008/ui/k01_2.swf";

NPStream stream;
memset(&stream, 0, sizeof(stream));
stream.url = sSrc.c_str();
FILE* f = fopen(sSrcFile.c_str(), "r");
fseek(f, 0L, SEEK_END);
fpos_t iFPos = 0;
fgetpos(f, &iFPos);
size_t iFSize = (size_t)iFPos;
fseek(f, 0L, SEEK_SET);
stream.end = (int32)iFSize;
char* buf = new char[iFSize + 1];
fread(buf, 1, iFSize, f);
//	typedef NPError (*NPP_NewStreamProcPtr)(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
uint16 stype = NP_NORMAL;
NPMIMEType pluginType = "application/x-shockwave-flash";
npErr = gFlashNP.m_pluginFuncs.newstream(m_instance, pluginType, &stream, false, &stype);
//	NP_NORMAL
//	m_pluginFuncs.asfile(pFlash->m_instance, &stream, sSrc.c_str());
int32 iReadySize = gFlashNP.m_pluginFuncs.writeready(m_instance, &stream);
//typedef int32 (*NPP_WriteProcPtr)(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
int32 iRet = gFlashNP.m_pluginFuncs.write(m_instance, &stream, 0, iFSize, buf);
gFlashNP.m_pluginFuncs.destroystream(m_instance, &stream, NPRES_DONE);
delete[] buf;
fclose(f);
*/

		}
	}
	
	_NPN_ReleaseObject(object);
	return bRet;
}

NPError CFlash::getValue(NPNVariable variable, void* value){
	switch (variable) {
	case NPNVWindowNPObject: {

		NPObject* windowScriptObject = &m_window; // m_parentFrame->script()->windowScriptNPObject();

		// Return value is expected to be retained, as described here: <http://www.mozilla.org/projects/plugin/npruntime.html>
		if (windowScriptObject)
			_NPN_RetainObject(windowScriptObject);

		void** v = (void**)value;
		*v = windowScriptObject;
//		*v = NULL;

		return NPERR_NO_ERROR;
		}
	case NPNVnetscapeWindow: {
		HWND* w = reinterpret_cast<HWND*>(value);
		//*w = m_parentWindow;
		*w = NULL;
		return NPERR_NO_ERROR;
		}
	case NPNVSupportsWindowless: {
		NPBool *result = reinterpret_cast<NPBool*>(value);
		*result = TRUE;
		return NPERR_NO_ERROR;
		}
	// ??
	}
	return NPERR_GENERIC_ERROR;
}

NPError CFlash::setValue(NPPVariable variable, void* value){
	switch(variable){
	case NPPVpluginWindowBool:
		m_isWindowed = (value!=NULL)?true:false;
		return NPERR_NO_ERROR;
	case NPPVpluginTransparentBool:
		m_isTransparent = (value!=NULL)?true:false;
		return NPERR_NO_ERROR;
	}
	return NPERR_GENERIC_ERROR;
}

//static const char* MozillaUserAgent = "Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.8.1) VUClient";
static const char* MozillaUserAgent = "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.0.8) Gecko/2009032609 Firefox/3.0.8";

const char* CFlash::userAgentStatic(){
	return MozillaUserAgent;
}

const char* CFlash::userAgent(){
	return MozillaUserAgent; // ?
}

void CFlash::invalidateRect(NPRect* invalidRect){
	CGuardByCS oGCS(m_cs);
	m_needToUpdate = true;
}

void CFlash::invalidateRegion(NPRegion invalidRegion){
	CGuardByCS oGCS(m_cs);
	m_needToUpdate = true;
}

void CFlash::forceRedraw(){
	CGuardByCS oGCS(m_cs);
	m_needToUpdate = true;
}

CFlash::CFlashHeader::CFlashHeader(FILE *in)
: _version(0), _is_compresed(false),
_x_min(0), _x_max(0),
_y_min(0), _y_max(0),
_frame_rate(0), _frame_count(0)
{
	memset(_rowSwfHeader, 0, sizeof(_rowSwfHeader));

	if (in)
	{
		fread(_rowSwfHeader, sizeof(unsigned char), 9, in);
		// проверякм маркировку 0, 1 и 2 байта
		if (_rowSwfHeader[1] == 'W' && _rowSwfHeader[2] == 'S')
		{
			if (_rowSwfHeader[0] == 'C')
			{ // пожатый zlib-ом файл
				if (decompressSwf(in))
					_is_compresed = true;
			}
			else if (_rowSwfHeader[0] == 'F')
			{ // не пожатый  файл
				fread(&_rowSwfHeader[9], sizeof(_rowSwfHeader) - 9, 1, in);
			}
			else
				// такого формата не знаю
				return;

			parseSwfHeader(_rowSwfHeader);
		}
	}
}

CFlash::CFlashHeader::CFlashHeader(const void* apData, unsigned int auSize)
	: _version(0), _is_compresed(false),
	_x_min(0), _x_max(0),
	_y_min(0), _y_max(0),
	_frame_rate(0), _frame_count(0)
{
	if(auSize > 32){
		memcpy(_rowSwfHeader, apData, 9);
		if (_rowSwfHeader[1] == 'W' && _rowSwfHeader[2] == 'S')
		{
			if (_rowSwfHeader[0] == 'C')
			{ // пожатый zlib-ом файл
				if (decompressSwf((const unsigned char*)apData, auSize))
					_is_compresed = true;
			}
			else if (_rowSwfHeader[0] == 'F')
			{ // не пожатый  файл
				memcpy(_rowSwfHeader, apData, sizeof(_rowSwfHeader));
			}
			else
				// такого формата не знаю
				return;

			parseSwfHeader(_rowSwfHeader);
		}
	}
}

bool CFlash::CFlashHeader::decompressSwf(FILE *in)
{
	unsigned char *compressedSwf   = (unsigned char *)alloca(sizeof(_rowSwfHeader));

	fseek(in, 0, SEEK_SET);
	fread(_rowSwfHeader, sizeof(unsigned char), 8, in);
	_rowSwfHeader[0] = 'F';

	z_stream zcpr;
	memset((void*)&zcpr, 0, sizeof(zcpr));
	inflateInit(&zcpr);

	zcpr.next_out  = _rowSwfHeader + 8;
	zcpr.avail_out = sizeof(_rowSwfHeader) - 8;

	int ret = Z_STREAM_END;
	do
	{
		if (zcpr.avail_in == 0)
		{
			fread(compressedSwf, sizeof(unsigned char), sizeof(_rowSwfHeader), in);
			zcpr.next_in  = compressedSwf;
			zcpr.avail_in = sizeof(_rowSwfHeader);
		}

		ret = inflate(&zcpr, Z_SYNC_FLUSH);

		if (zcpr.total_out >= sizeof(_rowSwfHeader) - 8
			|| ret == Z_STREAM_END)
			break;

	} while (ret == Z_OK);

	inflateEnd(&zcpr);

	return ret == Z_OK || ret == Z_STREAM_END ? true : false;
}

bool CFlash::CFlashHeader::decompressSwf(const unsigned char* apData, unsigned int auSize)
{
	unsigned char *compressedSwf = (unsigned char *)alloca(sizeof(_rowSwfHeader));
	memcpy(_rowSwfHeader, apData, 8);
	unsigned int pos = 8;
	_rowSwfHeader[0] = 'F';

	z_stream zcpr;
	memset((void*)&zcpr, 0, sizeof(zcpr));
	inflateInit(&zcpr);

	zcpr.next_out  = _rowSwfHeader + 8;
	zcpr.avail_out = sizeof(_rowSwfHeader) - 8;

	int ret = Z_STREAM_END;
	do
	{
		if (zcpr.avail_in == 0)
		{
			memcpy(compressedSwf, (unsigned char*)apData + pos, sizeof(_rowSwfHeader));
			pos += sizeof(_rowSwfHeader);
			zcpr.next_in  = compressedSwf;
			zcpr.avail_in = sizeof(_rowSwfHeader);
		}

		ret = inflate(&zcpr, Z_SYNC_FLUSH);

		if (zcpr.total_out >= sizeof(_rowSwfHeader) - 8
			|| ret == Z_STREAM_END)
			break;

	} while (ret == Z_OK);

	inflateEnd(&zcpr);

	return ret == Z_OK || ret == Z_STREAM_END ? true : false;
}

namespace
{
	unsigned inline long
		get_number(const void *in,  // входной буфер (достаточной длины)
		int begin,       // начало буфера от начало в битах
		int length       // длина в битах
		)
	{
		unsigned long result = 0;

		const unsigned char *in_buf = (const unsigned char *)in;
		while (begin - 8 >= 0)
		{
			++in_buf;
			begin -= 8;
		}

		int readen_bits = 0;

		// читаем побайтно
		while (readen_bits < length)
		{
			unsigned char cur_char = *in_buf;
			++in_buf;

			for (register int i = 0; i < 8; ++i)
			{
				if (begin != 0)
					--begin;
				else
				{
					result   = ((result << 1) | (unsigned char(cur_char & 0x80) >> 7));
					if (++readen_bits >= length)
						break;
				}

				cur_char = cur_char << 1;
			}
		}

		// если прочитали лишнее - скоректировать
		if (readen_bits > length)
			result = (result >> (readen_bits - length));

		return result;
	}
}

/* SWF File Header
*      Field       Type    Comment
*  0   Signature   UI8     Signature byte:
*                            "F" indicated uncompressed
*                            "C" indicates compressed (SWF 6 or later only)
*  1   Signature   UI8     Signature byte always "W"
*  2   Signature   UI8     Signature byte always "S"
*  3   Version     UI8     Single byte file version (for example, 0x06 for SWF 6)
*  4   FileLength  UI32    Length of entire file in bytes
*  8   FrameSize   RECT    Frame size in twips
*      FrameRate   UI16    Frame delay in 8.8 fixed number of frames per second
*      FrameCount  UI16    Total number of frames in movie
*/
void CFlash::CFlashHeader::parseSwfHeader(const unsigned char *buff)
{
	unsigned short pos = 0;
	if (buff[pos] == 'F' && buff[pos+1] == 'W' && buff[pos+2] == 'S')
	{
		// получить версию флеша
		_version = buff[pos = 3];

		// размер файла, за ненадобностью пропускаем
		//buff[4,5,6,7];

		// получаем размеры флеша
		// количество бит на каждую координату в RECT
		unsigned char rect_fild_size = (buff[pos = 8] >> 3);
		// считываем весь RECT, начало c 5-го бита
		_x_min = get_number(&buff[pos], 5, rect_fild_size);
		_x_max = get_number(&buff[pos], 5 + rect_fild_size, rect_fild_size);
		_y_min = get_number(&buff[pos], 5 + (2*rect_fild_size), rect_fild_size);
		_y_max = get_number(&buff[pos], 5 + (3*rect_fild_size), rect_fild_size);

		pos += (((5 + rect_fild_size*4) + 7) &~7)/8;

		// получаем кол-во кадров в секунду
		_frame_rate = buff[++pos];

		// получаем кол-во кадров
		_frame_count =  long(buff[pos+1]) | (long(buff[pos+2]) << 8);
	}
}

int CFlash::GetScaleMode(){
	// ??
	return 0;
}
void CFlash::SetScaleMode(int){
	// ??
}

int CFlash::GetWMode(){
	// ??
	return 0;
}
void CFlash::SetWMode(int){
	// ??
}

bool UTF82String(const char* autf8Str, int anLen, std::wstring &asStr);

const wchar_t* CFlash::GetVariable(const wchar_t *var){
	NPObject* object = NULL;
	NPError npErr;
	npErr = gFlashNP.m_pluginFuncs.getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
	if(npErr != NPERR_NO_ERROR || object == NULL)
		return NULL;
	m_wsLastGetVarRetVal = L"";
	const wchar_t* pwcRetVal = NULL;
	bool bRet = false;
	if(object->_class->invoke != NULL){
		NPIdentifier ident = _NPN_GetStringIdentifier("GetVariable");
		if (object->_class->hasMethod(object, ident)){
			NPVariant args[2];
			std::string sRequest = cLPCSTR(var);
			NPString npsRequest = { (const NPUTF8*)sRequest.c_str(), static_cast<uint32_t>(sRequest.size()) };
			NPN_InitializeVariantWithStringCopy(&(args[0]), &npsRequest);
			NPVariant result;
			result.type = NPVariantType_Void;
			bRet = object->_class->invoke(object, ident, args, 1, &result);
			_NPN_ReleaseVariantValue(&args[0]);
			if(result.type == NPVariantType_String){
				if(!UTF82String(result.value.stringValue.UTF8Characters, result.value.stringValue.UTF8Length, m_wsLastGetVarRetVal))
					m_wsLastGetVarRetVal = L"";
				pwcRetVal = m_wsLastGetVarRetVal.c_str();
			}
			_NPN_ReleaseVariantValue(&result);
		}
	}
	_NPN_ReleaseObject(object);
	return pwcRetVal;
}


struct MaskItem
{
	unsigned chMask;
	unsigned uShift;
	unsigned uSize;
	unsigned wcMax;
};
static const MaskItem _utf8string_pMaskItem[] = 
{
	{ 0x80,  0*6, 1, 0x7F,   },   // 1 byte sequence
	{ 0xC0,  1*6, 2, 0x7FF,  },   // 2 byte sequence
	{ 0xE0,  2*6, 3, 0xFFFF, },   // 3 byte sequence
	{ 0xF0,  3*6, 4, 0x1FFFFF, }, // 4 byte sequence
};

void String2UTF8(
				  const wchar_t* aBSTR,  //!< pointer to unicode string that is converterd
				  std::string& aUTF8        //!< pointer to UTF8 buffer string is converterd to
				  )
{

	char* pbszMax;
	char* pbszUTF8;
	unsigned uShift;
	const MaskItem* pMaskItem;
	const wchar_t* bs = aBSTR;
	DWORD dwLength = wcslen( bs);
	const wchar_t* pwszEnd = bs + dwLength;

	aUTF8.reserve(dwLength * 2);

	pbszUTF8 = (char*)aUTF8.c_str();
	pbszMax = pbszUTF8 + dwLength * 2;

	while( (*bs != L'\0') && (bs != pwszEnd) && (pbszUTF8 < pbszMax) )
	{
		if( *bs < 0x80 ) 
		{
			*pbszUTF8 = (char)*bs;
			pbszUTF8++;
		}
		else
		{
			for (pMaskItem = &_utf8string_pMaskItem[1]; pMaskItem->chMask; pMaskItem++ )
			{
				if (*bs < pMaskItem->wcMax ) break;
			}

			// Check size for buffer
			if (pbszUTF8 > (pbszMax - pMaskItem->uSize)) break;

			uShift = pMaskItem->uShift;
			*pbszUTF8 = (char)(pMaskItem->chMask | (*bs >> uShift));
			pbszUTF8++;
			while (uShift > 0)
			{
				uShift -= 6;
				*pbszUTF8 = (char)(0x80 | (*bs >> uShift)) & 0xBF;
				pbszUTF8++;
			}
		}
		bs++;
	}

	*pbszUTF8 = '\0';
}

bool CFlash::SetVariable(const wchar_t *var, const wchar_t *value){
	NPObject* object = NULL;
	NPError npErr;
	npErr = gFlashNP.m_pluginFuncs.getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
	if(npErr != NPERR_NO_ERROR || object == NULL)
		return false;
	bool bRet = false;
	if(object->_class->invoke != NULL){
		NPIdentifier ident = _NPN_GetStringIdentifier("SetVariable");
		if (object->_class->hasMethod(object, ident)){
			NPVariant args[2];
			std::string sRequest = cLPCSTR(var);
			NPString npsRequest = { (const NPUTF8*)sRequest.c_str(), static_cast<uint32_t>(sRequest.size()) };
			NPN_InitializeVariantWithStringCopy(&(args[0]), &npsRequest);
			std::string sValue = cLPCSTR(value);
			String2UTF8(value, sValue);
			NPString npsValue = { (const NPUTF8*)sValue.c_str(), static_cast<uint32_t>(strlen(sValue.c_str())) };
			NPN_InitializeVariantWithStringCopy(&(args[1]), &npsValue);
			NPVariant result;
			result.type = NPVariantType_Bool;
			bRet = object->_class->invoke(object, ident, args, 2, &result);
			_NPN_ReleaseVariantValue(&args[0]);
			_NPN_ReleaseVariantValue(&args[1]);
		}
	}
	_NPN_ReleaseObject(object);
	return bRet;
}

const wchar_t* CFlash::CallFunction(const wchar_t *request){
	NPObject* object = NULL;
	NPError npErr;
	npErr = gFlashNP.m_pluginFuncs.getvalue(m_instance, NPPVpluginScriptableNPObject, &object);
	if(npErr != NPERR_NO_ERROR || object == NULL)
		return NULL;

	bool bRet = false;
	if(object->_class->invoke != NULL){
		NPIdentifier ident = _NPN_GetStringIdentifier("CallFunction");
		if (object->_class->hasMethod(object, ident)){
			NPVariant args[2];
			/*
			args[0].type = NPVariantType_Int32;
			args[0].value.intValue = 0;
			args[1].type = NPVariantType_String;
			std::string sSRC = cLPCSTR(apwcSrc); // путь к ресурсу
			std::string sURL = sSRC; // URL как идентификатор стрима
			if(sURL[0] == ':')
				sURL = sURL.substr(1);
			NPString npsURL = { (const NPUTF8*)sURL.c_str(), static_cast<uint32_t>(sURL.size()) };
			NPN_InitializeVariantWithStringCopy(&(args[1]), &npsURL);
			*/
			std::string sRequest = cLPCSTR(request);
			NPString npsRequest = { (const NPUTF8*)sRequest.c_str(), static_cast<uint32_t>(sRequest.size()) };
			NPN_InitializeVariantWithStringCopy(&(args[0]), &npsRequest);

			args[1].type = NPVariantType_Void;

			NPVariant result;
			result.type = NPVariantType_Void;
			bRet = object->_class->invoke(object, ident, args, 2, &result);
			_NPN_ReleaseVariantValue(&args[0]);
			// ??
		}
	}
	_NPN_ReleaseObject(object);
	return 0;
}
