
#include "StdAfx.h"
#include "FontManager.h"
#include "HelperFunctions.h"
#include "GlobalSingletonStorage.h"
#include <ft2build.h>  
#include FT_FREETYPE_H

#include "FTFace.h"
#include "freetype/freetype.h"
#include "PlatformDependent.h"

_SFontDesc::_SFontDesc():
	MP_STRING_INIT(family),
	MP_STRING_INIT(normalFileName),
	MP_STRING_INIT(boldFileName),
	MP_STRING_INIT(italicFileName),
	MP_STRING_INIT(boldItalicFileName)
{
}

_SFontDesc::_SFontDesc(const _SFontDesc& desc):
	MP_STRING_INIT(family),
	MP_STRING_INIT(normalFileName),
	MP_STRING_INIT(boldFileName),
	MP_STRING_INIT(italicFileName),
	MP_STRING_INIT(boldItalicFileName)
{
	family = desc.family;
	normalFileName = desc.normalFileName;
	boldFileName = desc.boldFileName;
	italicFileName = desc.italicFileName;
	boldItalicFileName = desc.boldItalicFileName;
	renderType = desc.renderType;
}

CFontManager::CFontManager():
	MP_MAP_INIT(m_refsByFont),
	MP_MAP_INIT(m_fonts)
{
}

void CFontManager::LoadAll()
{
	// to do
}

void CFontManager::SaveAllFontsTextures()
{
	int cnt = 0;

	const std::wstring fontsDirectory = L"c:\\fonts\\";

	MP_MAP<MP_STRING, int>::iterator it = m_refsByFont.begin();
	MP_MAP<MP_STRING, int>::iterator itEnd = m_refsByFont.end();

	::CreateDirectory( (LPCSTR)fontsDirectory.c_str(), NULL);

	for ( ; it != itEnd; it++, cnt++)
	{
		int fontID = GetObjectNumber((*it).first);
		assert(fontID != -1);
		CFont* font = GetObjectPointer(fontID);

		std::wstring texturePath = fontsDirectory;
		texturePath += IntToWStr(cnt);
		texturePath += L".png";

		font->SaveFontTexture(texturePath);
	}
}

void CFontManager::OnFontFound(std::string filename)
{
	filename = StringToLower(filename);
	if ((filename.find("arial") == -1) && (filename.find("verdana") == -1) && (filename.find("times") == -1) && (filename.find("tahoma") == -1) && (filename.find("micros") == -1))
	{
		return;
	}

	bool bold, italic;
	bold = false; italic = false;

	FTFace face(filename.c_str());	
	FT_Face* ftface = face.Face();
	if ((ftface == NULL) || ((*ftface) == NULL))
	{
		g->lw.WriteLn("[ERROR] Failed to open font: ", filename);
		return;
	}

	std::string family = (*ftface)->family_name;
	long style_flags = (*ftface)->style_flags;
	switch(style_flags)
	{
	case 0: //regular
		break;
	case 1:
		italic = true;
		break;
	case 2:
		bold = true;
	    break;
	case 3:
		italic = true;
		bold = true;
	    break;
	default:
	    break;
	}

	MP_MAP<MP_STRING, SFontDesc*>::iterator iter = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (iter != m_fonts.end())
	{
		if ((bold) && (italic))
		{
			if ((*iter).second->boldItalicFileName.empty())
			{
				(*iter).second->boldItalicFileName = filename;
			}
		}
		else
		if (bold)
		{
			if ((*iter).second->boldFileName.empty())
			{
				(*iter).second->boldFileName = filename;
			}
		}
		else
		if (italic)
		{
			if ((*iter).second->italicFileName.empty())
			{
				(*iter).second->italicFileName = filename;
			}
		}
		else
		{
			if ((*iter).second->normalFileName.empty())
			{
				(*iter).second->normalFileName = filename;
			}
		}

		return;
	}
	else	
	{
		SFontDesc* tmp = MP_NEW(SFontDesc);
		tmp->family = family;
		tmp->normalFileName = "";
		tmp->italicFileName = "";
		tmp->boldFileName = "";
		tmp->boldItalicFileName = "";

		if((bold) && (italic))
		{
			tmp->boldItalicFileName = filename;
		}
		else if (bold)
		{
			tmp->boldFileName = filename;
		}
		else if (italic)
		{
			tmp->italicFileName = filename;
		}
		else 
		{
			tmp->normalFileName = filename;
		}
		m_fonts.insert(MP_MAP<MP_STRING, SFontDesc*>::value_type(MAKE_MP_STRING(StringToUpper(tmp->family)), tmp));
	}
}

void CFontManager::ScanDirectoryForFonts(std::string dir)
{
	if (!dir.empty())
	if ((dir[dir.size() - 1] != '\\') && (dir[dir.size() - 1] != '/'))
	{
		dir += "\\";
	}

	WIN32_FIND_DATA findData;
	HANDLE hSearch = FindFirstFile((dir + "*.ttf").c_str(), &findData);
	if (hSearch != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((strcmp(findData.cFileName, ".") != 0) && (strcmp(findData.cFileName, "..") != 0))
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	
			{
			}
			else
			{
				OnFontFound(dir + findData.cFileName);

			}
		} while (FindNextFile(hSearch, &findData));
		FindClose(hSearch);		
	}
}

int CFontManager::GetFontsListSize()
{
	return m_fonts.size();
}

void CFontManager::GetFontsList(std::vector<std::string>& fonts)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.begin();	
	MP_MAP<MP_STRING, SFontDesc*>::iterator itEnd = m_fonts.end();

	for ( ; it != itEnd; it++)
	{
		fonts.push_back((*it).second->family);
	}
}

CFont* CFontManager::GetFont(std::string family, int size, bool isBold, bool isItalic, bool isUnderline, bool isAntialised)
{
	std::string fontName = GetFontNameForSearch(family, size, isBold, isItalic, isAntialised);
	
	MP_MAP<MP_STRING, int>::iterator it = m_refsByFont.find(MAKE_MP_STRING(fontName));

	if (it != m_refsByFont.end())
	{
		(*it).second++;

		CFont* font = GetObjectPointer(GetObjectNumber(fontName));
		font->SetUnderliness(isUnderline);
		return font;
	}
	else
	{
		if (GetAvailableFontStyles(family) & GetFontStyle(isBold, isItalic, isUnderline))
		{
			int fontID = AddObject(fontName);			
			CFont* font = GetObjectPointer(fontID);
			
			font->SetAntialiasing(isAntialised);
						
			font->LoadFromFile(GetFontFile(family, isBold, isItalic), family, size);
			font->SetStyle(isBold, isItalic);
			font->SetUnderliness(isUnderline);			

			m_refsByFont.insert(MP_MAP<MP_STRING, int>::value_type(MAKE_MP_STRING(fontName), 1));

			return font;
		}
		else
		{
			return NULL;
		}
	}
}

std::string CFontManager::GetFontFile(std::string family, bool isBold, bool isItalic)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator iter = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (iter != m_fonts.end())
	{
		if ((isBold) && (isItalic))
		{
			return (*iter).second->boldItalicFileName;
		}
		
		if (isBold)
		{
			return (*iter).second->boldFileName;
		}

		if (isItalic)
		{
			return (*iter).second->italicFileName;
		}

		return (*iter).second->normalFileName;
	}
	else
	{
		assert(false);
		return "";
	}
}

void CFontManager::DeleteFont(CFont* font)
{    
	USES_CONVERSION;
	std::string fontName = W2A(font->GetName());

	MP_MAP<MP_STRING, int>::iterator it = m_refsByFont.find(MAKE_MP_STRING(fontName));

	if (it != m_refsByFont.end())
	{
		(*it).second--;

		if ((*it).second == 0)
		{			
			DeleteObject(font->GetID());
			m_refsByFont.erase(it);
		}
	}
}

bool CFontManager::HasNormalStyle(std::string family)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (it != m_fonts.end())
	{
		if (!(*it).second->normalFileName.empty())
		{
			return true;
		}
	}

	return false;
}

bool CFontManager::HasBoldStyle(std::string family)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (it != m_fonts.end())
	{
		if (!(*it).second->boldFileName.empty())
		{
			return true;
		}
	}

	return false;
}

bool CFontManager::HasItalicStyle(std::string family)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (it != m_fonts.end())
	{
		if (!(*it).second->italicFileName.empty())
		{
			return true;
		}
	}

	return false;
}

bool CFontManager::HasBoldItalicStyle(std::string family)
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (it != m_fonts.end())
	{
		if (!(*it).second->boldItalicFileName.empty())
		{
			return true;
		}
	}

	return false;
}


unsigned char CFontManager::GetAvailableFontStyles(std::string family)
{
	unsigned char res;
	res = 0;
	
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.find(MAKE_MP_STRING(StringToUpper(family)));
	if (it != m_fonts.end())
	{
		if (!(*it).second->normalFileName.empty())
		{
			res += STYLE_NORMAL;
			res += STYLE_UNDERLINE;
		}

		if (!(*it).second->boldFileName.empty())
		{
			res += STYLE_BOLD;
			res += STYLE_BOLD_UNDERLINE;
		}

		if (!(*it).second->italicFileName.empty())
		{
			res += STYLE_ITALIC;
			res += STYLE_ITALIC_UNDERLINE;
		}

		if (!(*it).second->boldItalicFileName.empty())
		{
			res += STYLE_BOLD_ITALIC;
			res += STYLE_BOLD_ITALIC_UNDERLINE;
		}
	}

	return res;
}

unsigned char CFontManager::GetFontStyle(bool isBold, bool isItalic, bool isUnderline)
{
	int res = isBold + isItalic * 2 + isUnderline * 4;

	switch (res)
	{
	case 0:	return STYLE_NORMAL;
	case 1:	return STYLE_BOLD;
	case 2:	return STYLE_ITALIC;
	case 3:	return STYLE_BOLD_ITALIC;
	case 4:	return STYLE_UNDERLINE;
	case 5:	return STYLE_BOLD_UNDERLINE;
	case 6:	return STYLE_ITALIC_UNDERLINE;
	case 7:	return STYLE_BOLD_ITALIC_UNDERLINE;
	}

	return 0;
}

std::string CFontManager::GetFontNameForSearch(std::string& family, int size, bool isBold, bool isItalic, unsigned char renderType)
{
	std::string fontName = family + "_";
	fontName += IntToStr(size);
	fontName += "_";
	fontName += IntToStr(isBold);
	fontName += "_";
	fontName += IntToStr(isItalic);
	fontName += "_";
	fontName += IntToStr(renderType);

	return fontName;
}

CFontManager::~CFontManager()
{
	MP_MAP<MP_STRING, SFontDesc*>::iterator it = m_fonts.begin();
	MP_MAP<MP_STRING, SFontDesc*>::iterator itEnd = m_fonts.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE((*it).second);
	}
}