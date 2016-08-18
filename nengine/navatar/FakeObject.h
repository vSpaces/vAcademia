
#if !defined(AFX_FAKEOBJECT_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_)
#define AFX_FAKEOBJECT_H__DBEA1B1B_43F0_4FE3_9EDA_311BCC236FB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "CommonAvatarHeader.h"

#include "Base.h"
#include "IPlugin.h"
#include "cal3d.h"
#include "AvatarObjPart.h"
#include "CommonDefines.h"
#include "CompositeTextureManager.h"

#include "3DObject.h"
#include "GlobalSingletonStorage.h"
#include "FrameBuffer.h"
#include "SkeletonAnimationObject.h"

#include "MessageObject.h"
#include "Commands.h"

class CLoadingCal3DObjectsTask;

typedef struct _SAsynchModel
{
	MP_STRING partName;
	MP_STRING partSrc;
	IAsynchResource* asynch;

	_SAsynchModel();
	_SAsynchModel(const _SAsynchModel& other);
} SAsynchModel;

typedef struct _SCommandsParam
{
	MP_STRING commandID;
	MP_STRING commandPath;
	MP_STRING commandParam;
	MP_STRING commandValue;

	_SCommandsParam(std::string _commandID, std::string _commandPath, std::string _commandParam, std::string _commandValue);
	_SCommandsParam(const _SCommandsParam& other);
} SCommandsParam;


class CAvatarObject :	public CAnyPluginBase,
						public IPlugin,
						public ICommandPerformer,
						public IAsynchResourceHandler
{
public:
	enum Object3DType
	{
		OBJ3DT_NULL,
		OBJ3DT_ATTACH
	};

public:
	CAvatarObject();
	virtual ~CAvatarObject();

	void FreeObject();

private:
	void ClearTasks();
	void ClearTask(CLoadingCal3DObjectsTask* task);

	bool CanTakeBillboardFromCache();
	std::string GetBillboardCacheName();
	bool IsBillboardDataCorrect(void* data, unsigned int dataSize);
	bool FixBillboardData(unsigned char* data, unsigned int textureSize);

public:
	IPlugin* GetPluginInterface();

	void HandleChangeBodyTexture(const std::string textureSrc);

public:
	void BeforeUpdate();
	void AfterUpdate();
	void BeforeRender();
	void AfterRender();
	void EndRendering();
	void EndRendering3D();
	void EndRendering2D();
	void SetObject(void* object);

	void OnAsynchResourceLoadedPersent(IAsynchResource* /*asynch*/, unsigned char/* percent*/);
	void OnAsynchResourceLoaded(IAsynchResource* asynch);
	void OnAsynchResourceError(IAsynchResource* asynch);

// реализация IRMPluginNotifications
public:
	void	OnPropsChanged() {};					// Called on Any prop changed
	void	OnPropChanged(char* /*propName*/) {};	// Called when prop <apPropName> is changed

// реализация IRMBasePlugin
public:
	unsigned long	ProcessMessage(int argumentCount, va_list arguments);

public:
	void	ProcessMessage(const wchar_t* /*ames*/) {};

public:
	void	OnSetPropsContainer();
	bool				LoadAndBindCal3DObjects(STransitionalInfo* info, std::string src, void* data, unsigned int size);
	void				LoadAndBindEngineObjects(STransitionalInfo* info, std::string src, C3DObject** ppLoadModel);

private:
	unsigned long	ProcessMessage(IMessageObject* message);

	bool				IsClothesUsed(std::string src);

	// Сохранение параметров аватара
	void				SaveAvatarPartToXMLDoc(TiXmlNode* node, SAvatarObjPart& avatarPart);
	void				SaveAvatarPartParametersToXMLDoc(TiXmlNode* node, SAvatarObjPart& avatarPart);

	void				LoadAvatarFromXML(std::string str);
	void				LoadAvatarPart(TiXmlNode* xml);	
	void				LoadParams(TiXmlNode* xml, const std::string& partTarget);
	void				UndressAvatar();
	void				ExportTuneString(std::string varName);
	void				ImportTuneString(std::string tuneString);
	std::string			GetTuneString();
	void				SetTuneString(const std::string& tuneString);
	void				MakeVisible(bool isVsible);
	
	// Загрузить часть тела и прявязать к скелету
	bool				LoadAndBind(std::string src, C3DObject** pp3dobj);
	bool				LoadAndBindDevided(std::string src, C3DObject** pp3dobj, void* data, unsigned int size);
	void				OnAvatarPartLoaded(C3DObject* obj3d);

	// Загрузить бленд	
	int					GetIndexPart(std::string partName);		
	void				DeleteEditparam(std::string name);
	void				ClearUserResources(SAvatarObjPart& desc);
	bool				CheckTemplateLoad(std::string TemplateName);
	
	void				SetFaceDeformation(std::string animationName, int animationWeight );

	void				setSnapshotColor(std::string strR, std::string strG, std::string strB, std::string strA);
	void				MakeSnapshot(const std::string& asCamera);
	CCamera3D*			CreateRenderCamera(const std::string& asCamera, float upKoef = 1.0);

	void				HandleSetParameterSafely(std::string commandID, std::string commandPath, std::string commandParam, std::string commandValue);
	void				HandleSetParameter(std::string partPath, std::string commandID, std::string commandParam, std::string commandValue);
	void				HandleSetParameter(SAvatarObjPart& part, std::string commandID, std::string partPath, std::string commandParam, std::string commandValue);
	void				HandleLoadMesh(const std::string& partName, const std::string& partSrc);	

	// Обработчики конкретных команд
	void				HandleSetBlendValue(SAvatarObjPart& part, std::string commandParam, const std::string& commandValue);
	void				SetPartBlendWeight(SAvatarObjPart& part, const std::string& asBlendName, float afWeight);
	float				GetPartBlendWeight(SAvatarObjPart& part, const std::string& blendName);
	void				HandleSetAlphaValue(SAvatarObjPart& part, const std::string& commandParam, const std::string& commandValue);
	void				HandleSetRGBVColor(SAvatarObjPart& part, const std::string& partPath, const std::string& commandParam, const std::string& commandValue);
	void				HandleSetBodyHeight(std::string commandValue);	
	void				HandleSetAlpharef(std::string commandValue, std::string commandParam, std::string commandPart);
	void				HandleSetDiffuse(std::string commandValue, std::string commandParam, std::string commandPart);
	void				HandleInterpolateTexture(std::string commandValue, std::string commandParam, std::string commandPart);	

	// Вспомогательные функции
	void				SetSkinColor(int textureID, DWORD color1, DWORD color2, float koef, int maxTextures);
	void				ApplyCommonSettings();
	void				ApplyDefaultSettings();
	void				InitAvatar(bool isMaleAvatar);
	
	void				LoadVisemes(LPCSTR lpFileName);

	void				ApplySkinSettings();

	void				MakeBillboard(CReadingCacheFileTask* task);

private:
	void				SaveChangedTextures(TiXmlNode* node);
	void				DeleteAvatarPart(SAvatarObjPart& part);

	void				ChangeHead(int headID);
	void				MakeUseOfSavingAvatarsObjPartsInfo();
	void				SaveAvatarObjPartsInfo(const std::string& partName, const std::string& partSrc, ifile* asynchFile = NULL);
	void				CheckDressAndSuperDress();
	
	void				ApplyClothesColors();
	void				ApplyBlends();

	bool				IsAvatarLoaded();

	bool				ChangeTexture(std::string meshSrc, std::wstring textureSrc);

	MP_VECTOR<SAvatarObjPart> m_avatarParts;
	MP_VECTOR<SAvatarParam> m_savedParameters;
	MP_VECTOR<SAsynchModel> m_asynchLoadedModels;


	CFrameBuffer* m_fbo;
	
	CCompositeTextureManager* m_compositeTextureManager;

	int m_brightnessLevel[MAX_LEVELS];
	unsigned char m_diffuseColors[MAX_LEVELS][3];

	CalVector		m_baseBodyHeight;
	float			m_bodyHeight;
	
	// Пол аватара
	bool			m_isMale;
	bool			m_isMyAvatar;

	bool			m_isNeedToMakePhoto;
	bool			m_isFinalizeRequired;

	bool			m_isDefineSnapshotMode;
	bool			m_isDefaultSettingsApplyed;	

	SAvatarObjPart	m_commonParameters;

	C3DObject* m_obj3d;	
	byte			m_dwSnaphotBgColorR;
	byte			m_dwSnaphotBgColorG;
	byte			m_dwSnaphotBgColorB;
	byte			m_dwSnaphotBgColorA;
	float			m_fCameraFov;

	__int64			m_lastUpdateTime;

	MP_STRING		m_currentCameraMode;
	int				m_viewportX, m_viewportY;
	int				m_viewportWidth, m_viewportHeight;
	bool			m_isEditorVisible;
	bool			m_isFinalizeTimed;

	MP_MAP<MP_STRING, int> m_clothesBrightness;

	CCommandCache* m_commandCache;

	int				m_headID;

	__int64			m_finalizeTime;

	bool m_hasNoParent;

	MP_VECTOR<SAvatarsObjNameAndSrc*> m_avatarsObjPartsInfo;
	MP_VECTOR<SCommandsParam*> m_rgbvColorCommands;
	MP_VECTOR<SCommandsParam*> m_blendCommands;

	MP_MAP<MP_STRING, MP_WSTRING> m_oldTexturesMap;
	MP_MAP<MP_STRING, MP_WSTRING> m_newTexturesMap;

	MP_MAP<MP_STRING, MP_WSTRING> m_changeTextureCommands;

	bool m_isComeToEndSetRGBVColor;
	bool m_isClothesLoadAsynch;
	bool m_isSpecialColor;

	MP_VECTOR<CLoadingCal3DObjectsTask*> m_loadingCal3DObjectsTasks;

	CReadingCacheFileTask* m_billboardTask;

	MP_STRING m_bodyTexture;
};

#endif 