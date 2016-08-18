#pragma once

#include "FileNameStorage.h"

namespace rmml {

/**
 * Элемент RMML-скрипта (Java-скрипта)
 * Используется mlRMMLBuilder-ом при инициализации RMML-структур. 
 * а также для задания скриптов, выполняемых при возникновении определенного события
 */
class mlRMMLScript: public mlRMMLElement
{
friend class mlRMMLBuilder;
	//bool mbRef; // mjss является ссылкой на скрипт класса
	// mjss является ссылкой на скрипт класса
	#define SCRF_REF (1 << 0)
	// скрипт еще не скомпилирован
	#define SCRF_NOT_COMPILED (1 << 1)
	unsigned char mucFlags; // флаги: mbRef
	void* mpData; // зависит от типа скрипта (event или watch или event expression)

	void Compile(const wchar_t* apwcData, const char* apszSrcName, int aiLine);
	void Compile(const char* apszData, const char* apszSrcName, int aiLine);
	mlresult Load(const wchar_t* apwcSrc);
	// сохранить скрипт в виде текста, чтобы скомпилировать его позже
	void SaveToCompileLater(const wchar_t* apwcData, const char* apszSrcName, int aiLine);
//	mlresult SetEventListener(wchar_t* apwcValue);
	// скомпилировать скрипт, если он еще не скомпилирован
	void Compile();
public:
	mlRMMLScript(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLScript(void);

	// реализация mlRMMLElement
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
		// задание параметров скрипта
	mlresult SetProperty(const wchar_t* apwcName,const wchar_t* apwcValue);
	mlresult SetValue(wchar_t* apwcValue);
//	bool NeedToBeDeleted(){ return true; }
	mlresult SetMeAsEventListener();
	void YouAreSetAsEventListener(char aidEvent, mlRMMLElement* apSender);
	mlresult RemoveMeFromEventListeners();
	bool EventNotify(char aidEvent, mlRMMLElement* apSender);
	void EventSenderDestroyed(mlRMMLElement* apSender);
	void Activate();
	mlString GetEventOrWatchName();
	mlString GetScriptName();
	mlString GetTagName(){ return L"script"; }
//	bool NeedToSave(int aiPropGroupType,const wchar_t* apwcName,
//		const wchar_t* apwcVal,mlRMMLElement* apTempElem);
	mlString GetValue(int aiIndent);
	SynchEventSender* GetSynchEventSenderStruct(){
		if(mType==MLST_SYNCHMS) return (SynchEventSender*)mpData;
		return NULL;
	}
	void SynchEventNotify(){
		Execute();
	}
	//
//	void Execute(JSContext* cx, JSObject* obj);
	mlresult Execute(jsval* apjsvResult=NULL);
	void GetWaitingEvents();
	struct EventExpressionStruct;
	bool CheckEventExpr(EventExpressionStruct* apEvExprStruct);

	std::string GetName();
	void SetName(std::string fileName);

	bool IsCompiled(){ return !(mucFlags & SCRF_NOT_COMPILED); }

	// получить размер буфера в байтах, в котором хранится нескомпилированный скрипт
	int GetNonCompiledScriptSize();

	// Когда объект исключается из иерархии сцены (parent=null), то надо, чтобы он перестал обрабатывать и события
	virtual void UnlinkFromEventSenders(bool abWithChildren = true);

private:
	unsigned short m_srcID;	
	unsigned short m_line;
};

}