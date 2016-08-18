
#pragma once

#include "ILoadInfoListener.h"
#include "Color3.h"
#include "Font.h"
#include <vector>
#include <string>
#include "LogValue.h"
#include "ILogWriter.h"

class INEngine
{
public:
	virtual void SetSettings() = 0;
	virtual void LoadSettings() = 0;
	virtual void Initialize(int resModelsFileID, int resTexturesFileID, ILoadInfoListener* loadInfoListener) = 0;
	virtual void Resize(int width, int height) = 0;
	virtual void BeginFrame(bool isClearNeeded) = 0;
	virtual void EndFrame() = 0;
	virtual void SetSize(int x, int y, int width, int height) = 0;
	virtual int GetWidth()const = 0;
	virtual int GetHeight()const = 0;
	virtual void SetViewport(int width, int height) = 0;
	virtual void SetViewport(int x, int y, int width, int height) = 0;
	virtual void RestoreViewport() = 0;
	virtual int GetCurrentWidth() = 0;
	virtual int GetCurrentHeight() = 0;
	virtual bool IsMultiTexturing()const = 0;
	virtual bool IsTransparent()const = 0;
	virtual bool IsWorking()const = 0;
	virtual bool IsDrawing()const = 0;
	virtual bool IsLoaded()const = 0;
	virtual void SetMultiTexturingState(bool state) = 0;
	virtual void SetTransparentState(bool state) = 0;
	virtual void SetWorkingState(bool state) = 0;
	virtual void SetDrawingState(bool state) = 0;
	virtual void SetLoadedState(bool state) = 0;
	virtual void SetBackgroundColor(float backgroundR, float backgroundG, float backgroundB, float backgroundA) = 0;
	virtual int GetModelsResourceFileID() = 0;
	virtual int GetTexturesResourceFileID() = 0;
	virtual void EnableCaveSupport() = 0;
};

class IFontManager
{
public:
	virtual void LoadAll() = 0;
	virtual void ScanDirectoryForFonts(std::string dir) = 0;
	virtual int GetFontsListSize() = 0;
	virtual void GetFontsList(std::vector<std::string>& fonts) = 0;
	virtual void OnFontFound(std::string filename) = 0;
	virtual CFont* GetFont(std::string family, int size, bool isBold, bool isItalic, bool isUnderline, bool isAntialiased) = 0;
	virtual void DeleteFont(CFont* font) = 0;
	virtual unsigned char GetAvailableFontStyles(std::string family) = 0;
	virtual bool HasNormalStyle(std::string family) = 0;
	virtual bool HasBoldStyle(std::string family) = 0;
	virtual bool HasItalicStyle(std::string family) = 0;
	virtual bool HasBoldItalicStyle(std::string family) = 0;
};

class ISpriteProcessor
{
public:
	virtual void PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, CColor3& color) = 0;
	virtual void PutNormalSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, CColor3& color) = 0;
	virtual void PutNormalSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, int angle) = 0;
	virtual void PutNormalSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, int angle) = 0;
	virtual void PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char sprAlpha, CColor3& color) = 0;
	virtual void PutAlphaSprite(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, unsigned char sprAlpha, CColor3& color) = 0;
	virtual void PutAlphaSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, int fl, unsigned char sprAlpha, float angle) = 0;
	virtual void PutAlphaSpriteWithRotation(int x, int y, int texX1, int texY1, int texX2, int texY2, std::string fileName, unsigned char sprAlpha, float angle) = 0;
	virtual void PutOneColorQuad(float x1, float y1, float x2, float y2, unsigned char r, unsigned char g, unsigned char b) = 0;
	
	virtual float GetOpenGLX(int x)const = 0;
	virtual float GetOpenGLY(int y)const = 0;
};


class IShaderManager
{
public:
	virtual bool Init() = 0;
	virtual int GetObjectNumber(std::string name) = 0;
	virtual bool BindShader(int shaderID, int lastUsedByMaterialID = -1) = 0;
	virtual void UnbindShader() = 0;
	//virtual bool SetUniformVector(int i, const char * name, const CVector4D& value) = 0;
	virtual int SetUniformTexture(int i, const char *name, int texUnit) = 0;
	virtual int SetUniformFloat(int i, const char * name, float value) = 0;
	virtual int SetUniformMatrix(int i, const char* name, float* value) = 0;
	virtual int SetUniformPredefined(int shaderID, std::string name, int valueID) = 0;
	virtual int GetPredefinedParamID(std::string param) = 0;
	virtual int GetParamTypeID(std::string paramType) = 0;
};

class ITextureManager
{
public:
	virtual void Initialize() = 0;
	virtual void LoadAll() = 0;
	virtual void LoadAllExternal() = 0;
	virtual void BindTexture(int number) = 0;
	virtual void RefreshTextureOptimization() = 0;
	virtual int GetObjectNumber(std::string name) = 0;
	virtual void RestoreDefaultSettings() = 0;
	virtual void SetEmptyTextureRGB(unsigned int oglTextureID, int width, int height) = 0;
	virtual void SetEmptyTextureRGBA(unsigned int oglTextureID, int width, int height) = 0;
	virtual int ManageGLTexture(unsigned int oglTextureID, int width, int height) = 0;
	virtual int LoadTextureFromMemory(void* buf, int type, std::string name) = 0;
	virtual void DisableMultitexturing2TL() = 0;
	virtual void DisableMultitexturing3TL() = 0;
	virtual void DisableTexturing() = 0;
	virtual void EnableTexturing() = 0;
	virtual void SetTextureForLevel(int level, int mode, int textureID) = 0;
	virtual void SetTextureForLevel(int level, int mode, std::string fileName) = 0;
	virtual void SetLoadInfoListener(ILoadInfoListener* loadInfoListener) = 0;
};

class ICamManager
{
public:
	virtual void SetCamera2d() = 0;
};

#define SET(x, y) void Set##x##(I##x##* _##y##) { ##y## = _##y##; }

class CGlobalInterfaceStorage
{
public:
	static CGlobalInterfaceStorage* GetInstance()
	{
		static CGlobalInterfaceStorage* obj = NULL;

		if (!obj)
		{
			obj = new CGlobalInterfaceStorage();
		}

		return obj;
	}

	SET(NEngine, ne)
	SET(LogWriter, lw)
	SET(FontManager, fm)
	SET(ShaderManager, sm)
	SET(CamManager, cm)
	SET(TextureManager, tm)
	SET(SpriteProcessor, sp)

	INEngine* ne;
	ILogWriter* lw;
	ICamManager* cm;
	IFontManager* fm;
	IShaderManager* sm;
	ITextureManager* tm;
	ISpriteProcessor* sp;
};

extern CGlobalInterfaceStorage* gI;
