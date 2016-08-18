
#pragma once

#include "CommonEngineHeader.h"
#include "Manager.h"
#include "Font.h"
#include <map>

#pragma  warning (disable: 4786)

typedef struct _SFontDesc
{
	MP_STRING family;
	MP_STRING normalFileName;
	MP_STRING boldFileName;
	MP_STRING italicFileName;
	MP_STRING boldItalicFileName;
	unsigned char renderType;

	_SFontDesc();
	_SFontDesc(const _SFontDesc& desc);
} SFontDesc;

class CFontManager : public CManager<CFont>/*, public IFontManager*/
{
public:
	CFontManager();
	~CFontManager();

	void LoadAll();

	void SaveAllFontsTextures();
	
	void ScanDirectoryForFonts(std::string dir);
	int GetFontsListSize();
	void GetFontsList(std::vector<std::string>& fonts);
	void OnFontFound(std::string filename);

	CFont* GetFont(std::string family, int size, bool isBold, bool isItalic, bool isUnderline, bool isAntialiased);
	void DeleteFont(CFont* font);

	unsigned char GetAvailableFontStyles(std::string family);
	bool HasNormalStyle(std::string family);
	bool HasBoldStyle(std::string family);
	bool HasItalicStyle(std::string family);
	bool HasBoldItalicStyle(std::string family);

	static unsigned char GetFontStyle(bool isBold, bool isItalic, bool isUnderline);

private:
	std::string GetFontFile(std::string family, bool isBold, bool isItalic);
	std::string GetFontNameForSearch(std::string& family, int size, bool isBold, bool isItalic, unsigned char renderType);

	MP_MAP<MP_STRING, int> m_refsByFont;
	MP_MAP<MP_STRING, SFontDesc*> m_fonts;
};