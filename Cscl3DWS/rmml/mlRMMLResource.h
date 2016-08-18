#pragma once

namespace rmml {

/**
 * Класс объектов для загрузки/распаковки/проверки ресурсов
 */

class mlRMMLResource :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public res::resIAsynchResource
{
	res::resIResource* mpResource;
	char mchUnpackState; // 0 - не надо распаковывать, 1 - выкачивается еще, 2 - распаковывается, 3 - распакован
	bool mbUploading;
	res::IAsyncCheckExist *pAsynchCheckExist;
	MP_WSTRING mwsWarning;
	
	void Reset();
	unsigned int GetState();
public:
	mlRMMLResource(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLResource(void);
MLJSCLASS_DECL
protected:
	enum {
		JSPROP_size = 1,	// RO размер файла
		JSPROP_downloaded,	// RO сколько байт выкачано
		JSPROP_state, 		// RO состояние (существует вообще|есть в ЛХ|есть в ЛХ и распакован)
		JSPROP_downloading, // RW флаг закачивается/не закачивается
		JSPROP_uploaded,	// RO сколько байт закачано в модуль
		JSPROP_uploading,   // RW флаг закачивается/не закачивается
		JSPROP_warningText,	// RO 
		JSPROP_RS_NOT_FOUND,	// 0 - ресурс не найден
		JSPROP_RS_REMOTE,		// 1 - ресурс найден на сервере, но не закачан и не закачивается
		JSPROP_RS_DOWNLOADING,	// 2 - ресурс закачивается с сервера
		JSPROP_RS_LOCAL,		// 3 - ресурс найден на локальном компе или на CD
		JSPROP_RS_UNPACKING,	// 4 - ресурс на локальном компе и сейчас распаковывается
		JSPROP_RS_READY,		// 5 - ресурс на локальном компе, распакован и готов к использованию
		JSPROP_RS_UPLOADING,	// 10 - ресурс закачивается в модуль (либо в локальный, либо на севере)
		JSPROP_RS_UPLOADING_PAUSE,	// 11 - закачка ресурса приостановлена
		JSPROP_RS_UPLOADED,		// 12 - ресурс закачан
		JSPROP_RS_ERROR,		// 13 - ресурс не закачан
		JSPROP_RS_REGISTERING,  // 14 - ресурс регистрируется
		JSPROP_RS_REGISTERED,   // 15 - ресурс зарегистрирован
    };
//?	bool downloading;

	JSFUNC_DECL(isExist)	// проверить, существует ли ресурс
	JSFUNC_DECL(download)	// начать закачивание ресурса (но не распаковывать)
	JSFUNC_DECL(downloadAndUnpack)	// начать закачивание и распаковку
	JSFUNC_DECL(getProgress)	// получить состояние закачки/распаковки в процентах
	JSFUNC_DECL(copyTo)		// скопировать закачанный и распакованный ресурс в заданную дирректорию
	JSFUNC_DECL(registerFileAtResServer)	// зарегистрировать ресурс на сервере ресурсов

	bool IsExist( const wchar_t* apwcPath, bool abLocal = false, bool abSync = false);
	bool StartDownloading( char aUnpackState, const wchar_t* apwcLocalPath=NULL);
	void StateChanged();
	bool RegisterFileAtResServer(const wchar_t* apwcPath);

public:
	bool StartUploading(const wchar_t* apwcDestPath, const wchar_t* apwcSrcPath, bool abPackSource = false);

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult Update(const __int64 alTime);
	bool Freeze();
	bool Unfreeze();

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация moILoadable
	virtual bool SrcChanged();

// реализация res::resIAsynchResource
	virtual omsresult OnLoaded();
	virtual omsresult OnDownloaded();
	virtual omsresult OnCached();
	virtual omsresult OnUnpacked();
	virtual omsresult OnError( unsigned int auiCode, unsigned int auiServerCode = 0, wchar_t* aNotifyServerText = NULL);
	virtual omsresult OnUploaded(wchar_t* aNotifyServerText);
	virtual omsresult OnExisted( bool auiCode);
	virtual omsresult OnRegistered(wchar_t* aNotifyServerText);
};

}