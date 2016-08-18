#pragma once

namespace rmml {

// имена свойств
#define MLELPN_SRC "src"
//#define MLELPN_WIDTH "_width"
//#define MLELPN_HEIGHT "_height"

// имена событий
#define EVNM_onLoad L"onLoad"
#define EVNM_onLoading L"onLoading"
#define EVNM_onLoadAll L"onLoadAll"
#define EVNM_onUnload L"onUnload"
#define EVNM_onLoadError L"onLoadError"

/**
 * RMML-подкласс для RMML-классов, которые могут быть загружены из файла
 * (Image, Animation, ...)
 * (не путать с прототипами JavaScript!)
 */

class mlRMMLLoadable: 
									//public mlJSClassProto,
									public mlILoadable,
									moILoadable
{
friend class mlRMMLElement;
public:
	mlRMMLLoadable(void);
	~mlRMMLLoadable(void);
MLJSPROTO_DECL
protected:
	enum { JSPROP0_mlRMMLLoadable=mlRMMLElement::TEVIDE_mlRMMLElement-1,
		JSPROP_src,
		//JSPROP_bytesLoaded,
		//JSPROP_bytesTotal,
		JSPROP_streaming,
		TIDE_mlRMMLLoadable
	};
	enum { EVID_mlRMMLLoadable=TIDE_mlRMMLLoadable-1,
		EVID_onLoad,	// загружен основной ресурс (указанный в src) (RMML_LOAD_STATE_SRC_LOADED)
		EVID_onLoading,	// начался процесс загружки основного ресурса (указанный в src) (RMML_LOAD_STATE_SRC_LOADING)
		EVID_onLoadAll,	// загружены все необходимые ресурсы, включая ресурсы дочерних элементов (RMML_LOAD_STATE_ALL_RESOURCES_LOADED)
		EVID_onUnload,	// выгружаются все ресурсы (перед загрузкой нового ресурса или при уничтожении объекта)
		EVID_onLoadError,	// при загрузке одного из ресурсов произошла ошибка (RMML_LOAD_STATE_LOADERROR)
		TEVIDE_mlRMMLLoadable
	};

//	#define	LST_NOT_LOADED							6
	#define	LST_SRC_NOT_SET							5
	#define	LST_NOT_LOADED							4
	#define	LST_LOADING								3
	#define	LST_LOAD_ERROR							2
	#define	LST_RES_LOADED							1
	#define	LST_LOADED								0 // ресурс загрузился и onLoad выз-ся в скрипте

protected:
	JSString* src;
	MP_WSTRING msSrcFull;

	unsigned char miSrcLoaded;	// ресурс, указанный в src был загружен
	unsigned long miLoadedBytes; // кол-во загружанных байтов файла
	unsigned long miTotalBytes; // общее число байтов (размер) файла
	bool mbStreaming; // указывает, что процесс работы с ресурсом начнется до полной загрузки его с интернета

	// 1 - "UI" - приоритет загрузки текстур интерфейса. К ним же относятся заметки в развернутом окне.
	// 2 - "ForShowNow" - для ближайшего показа на объектах. Это то, что мы хотим показывать на доске. К этим текстурам надо применять привязку к объекту.
	// 3 - "ForShowLater" - для загрузки не в приоритете. Это то, что мы возможно будем скоро показывать. Это ближайшие слайды. К этим текстурам надо применять привязку к объекту.
	// 4 - "ForShowSomeDay" - для фоновой загрузки. Это то, что в ближайшее время нам не надо. Например быстро перемотанное начало презентации.
	ELoadingPriority	mLoadingPriority;	// приоритет загрузки ресурса

	JSFUNC_DECL(getBytesLoaded)
	JSFUNC_DECL(getBytesTotal)
	JSFUNC_DECL(reload)
	JSFUNC_DECL(getLoadingPriority)
	JSFUNC_DECL(setLoadingPriority)

protected:
	virtual mlRMMLElement* GetElem_mlRMMLLoadable()=0;
	bool Lod_SetEventData(char aidEvent, mlSynchData &Data);
	void Set_Event_state(mlRMMLElement* apMLEl, int aiState, wchar_t *aUserErrorString, wchar_t *aWarningText);
	void Set_Event_errorCode(mlRMMLElement* apMLEl, int aiErrorCode, int aiUserErrorCode, wchar_t *aUserErrorString);
public:
	virtual void ResourceLoadedAsynchronously(mlSynchData &Data){}

// реализация  mlILoadable
public:
	// Свойства
	const wchar_t* GetRawSrc() const;
	const wchar_t* GetSrc();
	ELoadingPriority GetLoadingPriority() const;
	bool IsStreaming();
	bool IsLoading();
	// События
	void onLoad(int aiState);
	void onLoading(int amiLoadedBytes, int amiTotalBytes);
	void onUnload();
	void SetLoaded();
// реализация moILoadable
protected:
	moILoadable* GetmoILoadable();
	// Свойства
	virtual bool SrcChanged();
	// методы
	virtual bool Reload();
	// смена приоритета загрузки ресурса
	void LoadingPriorityChanged( ELoadingPriority aPriority);
};

#define MLILOADABLE_IMPL \
public: \
	mlRMMLElement* GetElem_mlRMMLLoadable(){ return this; } \
	mlILoadable* GetILoadable(){ return this; } \
	mlRMMLLoadable* GetLoadable(){ return this; }

//	mlRMMLElement* GetElem(){ return this; } \
//	void OnLoad(){ mlRMMLLoadable::onLoad(this); } \
//	void OnUnload(){ mlRMMLLoadable::onUnload(this); }

}
