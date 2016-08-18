
#ifndef rmmld_h___
#define rmmld_h___

namespace rmmld{

using namespace rmml;

/*struct RMML_NO_VTABLE mlIAuthoring 
{
	enum{
		TRID_SCENE,
		TRID_UI,
		TRID_JS
	};
	virtual int AddElement(short type, int parentID)=0;
	virtual bool DeleteElement(int aElemID)=0;
	virtual int GetParent(int aElemID)=0;
	virtual int GetTree(void** appData, int aiTreeID)=0; // TRID_*
	virtual int GetProperties(int aElemID, void** appData)=0;
	virtual int GetUserEvents(int aElemID, void** appData)=0;
	virtual int GetElementTypeProperties(const char* apcTagName, void** appData)=0;
	virtual int GetElementTypeEvents(const char* apcTagName, void** appData)=0;
	virtual void GetProperty(int aElemID, char* apszName, char** appszVal)=0;
	virtual void SetProperty(int aElemID, char* apszName, char* apszVal)=0;
	virtual const char* GetValue(int aElemID)=0;
	virtual bool SetValue(int aElemID, const char* apcValue)=0;
	virtual omsresult GetAbsVisibleProps(int aElemID, int &aiX, int &aiY, oms::omsRect &arcBounds, int &aiDepth, bool &abVisible)=0;
	virtual omsresult SetAbsVisibleProps(int aElemID, int aiX, int aiY, const oms::omsRect arcBounds)=0;
	virtual int Get3DProperties(int aElemID, void** appData)=0;
	virtual void Get3DProperty(int aElemID, char* apszName, char* apszSubName, char** appszVal)=0;
	virtual void Set3DProperty(int aElemID, char* apszName, char* apszSubName, char* apszVal)=0;
	virtual omsresult Save3DProperties(int aElemID)=0;
	virtual void LoadRMML(unsigned short* apszFileName)=0;
	virtual void SaveRMML(unsigned short* apszFileName)=0;
	virtual int Get3DObjectUnderMouse(int aiViewportID, mlPoint aMouseXY)=0;
	virtual bool IsSceneLoaded()=0;
	virtual bool StartScene()=0;
	virtual bool StopScene()=0;
	virtual bool PauseScene()=0;
	virtual int GetVisibleAt(int aiX, int aiY)=0;
	virtual omsresult GetSource(int aElemID, int &aiStr, const wchar_t* &apwcSrcFileName)=0;
	virtual int GetElementType(int aElemID, const wchar_t* &apwcTagName)=0;
	virtual int FindElement(void* apData, int aiSize)=0;
	virtual int GetTextFormattingInfo(int aElemID, void** appData)=0;
	virtual int GetElementID(mlMedia* apmlMedia)=0;
	virtual mlMedia* GetElementByID(int aElemID);
	// уточнить путь к ресурсу, использу€ полномый путь к RMML-файлу, в котором он используетс€
	virtual omsresult RefineResPath(const char* apcXMLSrcPath, mlOutString &sResSrcPath);
};*/

struct RMML_NO_VTABLE mlIDebugger {
	virtual void Reset()=0;
	virtual char* GetSceneName()=0;
	virtual bool Next()=0;
	virtual int GetTreeDepth()=0;
	// информаци€ о текущем элементе
	virtual char* GetName()=0;
	virtual bool IsComposition()=0;
	virtual bool IsVisible()=0;
	virtual bool IsText()=0;
	// информаци€ о видимом элементе
	virtual mlPoint GetXY()=0;
	virtual mlSize GetSize()=0;
	virtual int GetDepth()=0;
	virtual bool GetVisible()=0;
	virtual mlRect GetBounds()=0;
	// информаци€ о тексте
	virtual mlString GetValue()=0;
	// 
	virtual bool GetProperties(mlSynchData &aData, unsigned int aiElemID)=0;
	//virtual mlIAuthoring* GetAuthoring() = 0;
	virtual bool GetTree(mlSynchData &aData)=0;
	virtual bool GetJSTree(mlSynchData &aData)=0;
	virtual bool GetUITree(mlSynchData &aData)=0;
};

RMML_DECL mlIDebugger* CreateDebugger(mlISceneManager* apSM);

// структура дл€ передачи дерева сцены
struct mldElementInfo
{
	int type;
	char typen[30];
	char name[100];
	unsigned int id;
	unsigned short numChild;
};

enum {
	WCDC_GET_TRACE_INFO=0, 
	WCDC_UPDATE_TRACE_INFO, 
	WCDC_GET_TREE_INFO, 
//	WCDC_UPDATE_TREE_INFO, 
	WCDC_GET_PROPERTIES,
	WCDC_GET_JSTREE 
};

}

#endif