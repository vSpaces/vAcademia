
#pragma once

#include "CommonEngineHeader.h"

#include "UpdateDynamicTexturesTask.h"
#include "ReadingCacheResourceTask.h"
#include "ILoadInfoListener.h"
#include "GlobalInterfaceStorage.h"
#include "LoadingTexturesTask.h"
#include "RenderingCallback.h"
#include "CachingManager.h"
#include "CommonDefines.h"
#include "TextureLoader.h"
#include "TextureSaver.h"
#include "Manager.h"
#include "Texture.h"
#include <string>
#include "PBOInfo.h"

#define ASYNCH_MODE_NORMAL			0
#define ASYNCH_MODE_ONLY			1
#define ASYNCH_MODE_UNPACK_ONLY		2

typedef struct _SSaveCompressionTextureInfo
{
	SPBOInfo* pboInfo;
	int size;
	MP_STRING path;
	int framePassCount;

	_SSaveCompressionTextureInfo();	
} SSaveCompressionTextureInfo;

#define EMPTY_TEXTURE_LINEAR_SIZE	2

class CGlobalSingletonStorage;

class CTextureManager : public CManager<CTexture>, public CCachingManager
{
friend class CGlobalSingletonStorage;

public:
	void Initialize();

	void Update();

	void LoadAll();
	void LoadAllExternal();

	void RemoveDynamicTexture(IDynamicTexture* texture);

	int GetObjectNumber(std::string name);
	int GetObjectNumber(std::wstring name);
	
	void BindTexture(int textureID);
		
	void RefreshTextureOptimization();
	void RestoreDefaultSettings();
	
	int ManageGLTexture(std::string dest, unsigned int oglTextureID, int width, int height);

	int LoadTextureFromMemory(void* buf, int type, std::string name);
	int LoadAsynch(std::string &name, std::string &path, std::vector<void*>* owners = NULL, bool onlyAsynch = false, CNTextureLoadingInfo* loadInfo = NULL, unsigned char maxDecreaseKoef = 0);
	int LoadAsynch(std::wstring &name, std::wstring &path, std::vector<void*>* owners = NULL, bool onlyAsynch = false, CNTextureLoadingInfo* loadInfo = NULL, unsigned char maxDecreaseKoef = 0);
	bool LoadFromIFile(CTexture* texture, ifile* file, bool isMipmap = true, bool isMaskNeeded  = false);
	int LoadTextureFromFile(std::string &name, std::string &path);
	int LoadTextureFromFile(std::string &name, std::wstring &path);
	
	bool SaveTexture(CTexture* texture, std::wstring& fileName , SCropRect* const rect = NULL);
	bool SaveTexture(int textureID, std::wstring& fileName, SCropRect*  const rect = NULL);
	bool SaveTexture(int openGLTextureID, unsigned int width, unsigned int height, bool isTransparent, std::wstring fileName);
	bool CompressImageData(int pictureWidth, int pictureHeight, int channelsCount,  void* pictureData
							, void** pictureDataOut, unsigned long* dataSize, int quality = 50);
	bool StretchCompressedImageData(int srcX, int srcY, int srcWidth, int srcHeight, int destWidth
		, int destHeight, void* pictureData, void** pictureDataOut, unsigned long* dataSize, int quality);

	void DisableMultitexturing2TL();
	void DisableMultitexturing3TL();
	void DisableTexturing();
	void EnableTexturing();

	void SetTextureForLevel(int level, int mode, int textureID);
	void SetTextureForLevel(int level, int mode, std::string fileName);

	void SetLoadInfoListener(ILoadInfoListener* loadInfoListener);

	int AddColorTexture(std::string textureName);
	int AddColorTexture(std::string textureName, unsigned int color);
	void ReplaceColorTexture(int textureID, unsigned int color);
	int AddDynamicTexture(IDynamicTexture* dynamicTexture);
	void UpdateDynamicTextureSize(IDynamicTexture* dynamicTexture);
	int AddResampledTexture(int textureID, int width, int height);
	void ResizeTexture(int textureID, int width, int height);
	void ResetResizeTexture(int textureID, int width, int height);
	
	void RemoveDynamicTexture(CTexture* texture);

	void BeforeNewFrame();
	void AfterNewFrame();

	int GetCurrentTextureID()const;
	int GetErrorTextureID()const;
	int GetGreyTextureID()const;	
	int GetWhiteTextureID()const;

	void RegisterTexture(CTexture* texture);

	int GetTextureIDByOGLID_DEBUG(int oglTextureID);

	int GetAvatarTextureID();

	void SetCurrentMipMapBias(float mipmapBias);

private:
	CTextureManager();
	CTextureManager(const CTextureManager&);
	CTextureManager& operator=(const CTextureManager&);
	~CTextureManager();

	void InitTexture(CTextureLoader* textureLoader, CTexture* texture, bool isMipmap = true, bool isMaskNeeded = false, std::string* cacheFileName = NULL);
	bool InitCompressedTexture(CTexture* texture, void* data, int size);
	bool LoadCompressed(CTexture* texture, std::string& cacheFileName);
	bool LoadCompressed(CTexture* texture, unsigned char* data, unsigned int size);	
	
	void SetCubeMapTexture(int target, int format, bool isMipmap, void* ptr);
	
	// loading values
	int m_new;
	int m_greyTextureID;
	int m_whiteTextureID;
	int m_lastWidth, m_lastHeight;
	bool m_canBeCompressed;

	int m_lastTextureID[8];
	int m_errorTextureID;
	int m_skipLoadingFrames;
	unsigned int m_cubeMapNum;

	bool m_isTexturingEnable;
	bool m_isCheckingVisibleDynamicTextures;
	__int64 m_lastUpdateTime;
	__int64 m_startTexturesChanged; 

	unsigned int m_deltaTime;
	unsigned int m_commonTime;

	float m_currentMipmapBias;

	CTextureLoader m_textureLoader;
	CTextureLoader m_bgTextureLoader;

	CTextureSaver m_textureSaver;

	CTexture* m_occlusionTexture;

	MP_VECTOR<CTexture *> m_dynamicTextures;
	MP_VECTOR<CTexture *> m_updatingTextures;

	MP_VECTOR<SLoadingTexture *> m_loadingTextures;

	ILoadInfoListener* m_loadInfoListener;

	CUpdateDynamicTexturesTask m_updateDynamicTexturesTask;

	MP_VECTOR<CLoadingTexturesTask *> m_loadingTexturesTasks;
	MP_VECTOR<int> m_avatarTextureID;

	MP_VECTOR<CReadingCacheResourceTask *> m_readCompressedTexturesTasks;

	MP_VECTOR<SSaveCompressionTextureInfo *> m_texturesForSave;

	int m_textureWidthBeforeResize;
	int m_textureHeightBeforeResize;
};