
#include "StdAfx.h"
#include <Assert.h>
#include <malloc.h>
#include "HelperFunctions.h"
#include "FormattedTextSprite.h"
#include "GlobalSingletonStorage.h"
#include "cal3d/memory_leak.h"

#define LESS_TAG		1
#define GREATER_TAG		2

#define FOR_ALL_PARTS std::vector<SPart *>::iterator it = m_parts.begin(); std::vector<SPart *>::iterator itEnd = m_parts.end(); for ( ; it != itEnd; it++)

_SPart::_SPart():
	MP_WSTRING_INIT(text)
{
	isBold = false;
	isItalic = false;
	isUnderline = false;
	isShadowEnabled = false;
	lineWidth = 0;
	textSprite = MP_NEW(CTextSprite);
}

_SPart::~_SPart()
{
	MP_DELETE(textSprite);
}

CFormattedTextSprite::CFormattedTextSprite(): 	
	m_realWidth(0),
	m_lastWidth(0),
	m_lastHeight(0),
	m_sliced(false),
	m_wordBreaks(false),
	m_isTagsEnabled(true),
	m_isTagsAllowed(true),	
	MP_VECTOR_INIT(m_parts),
	MP_WSTRING_INIT(m_text),
	m_horizontalAlign(ALIGN_LEFT)	
{
	SetType(SPRITE_TYPE_FORMATTED_TEXT);	
}

void CFormattedTextSprite::PermitTags(const bool isEnabled)
{
	m_isTagsEnabled = isEnabled;
}

bool CFormattedTextSprite::AreWordBreaksExists()
{
	return m_wordBreaks;
}

bool CFormattedTextSprite::IsTagsPermitted()const
{
	if (m_isTagsEnabled && m_isTagsAllowed)
		return true;

	return false;
}

void CFormattedTextSprite::SetHorizontalAlign(const int align)
{
	m_horizontalAlign = align;
}

int CFormattedTextSprite::GetHorizontalAlign()const
{
	return m_horizontalAlign;
}

int CFormattedTextSprite::GetSymbolIDByXY(int x, int y)
{
	std::vector<SPart*>::iterator it = m_parts.begin();
	std::vector<SPart*>::iterator itEnd = m_parts.end();

	int offset = 0;

	bool isFound = false;

	for (; it != itEnd; it++)
	{
		SPart* part = *it;		
		if ((x >= part->x + m_x) && (x <= part->x + m_x + part->textSprite->GetRealWidth()) &&
			(y >= part->y + m_y) && (y <= part->y + m_y + part->textSprite->GetRealHeight()))
		{		
			unsigned int i = 1;		
			int height = part->textSprite->GetRealHeight();
			while (i < part->text.size())
			{		
#pragma warning(push)
#pragma warning(disable : 4239)
				int width = part->textSprite->GetRealWidth(part->text.substr(0, i));		
#pragma warning(pop)				
				i++;
				offset++;

				if ((x >= part->x + m_x) && (x <= part->x + m_x + width) &&
					(y >= part->y + m_y) && (y <= part->y + m_y + height))
				{
					break;
				}
			}
			isFound = true;
			break;
		}
		else
		{
			offset += part->text.size();
		}
	}

	if (isFound)
	{
		return offset;
	}
	else
	{
		SPart* foundPart = NULL;
		offset = 0;
		it = m_parts.begin();
		for (; it != itEnd; it++)
		{
			SPart* part = *it;		
			if ((y >= part->y + m_y) && (y <= part->y + m_y + part->textSprite->GetRealHeight()))
			{
				offset += part->text.size();
				foundPart = part;
			}
			else if (!foundPart)
			{
				offset += part->text.size();
			}
		}
		
		if (foundPart)
		{
			return offset;
		}
		else
		{
			return -1;
		}
	}
}

void CFormattedTextSprite::SetSelection(int from, int to)
{	
	if (m_parts.empty())
	{
		return;
	}
	
	if (to < from)
	{
		int tmp = to;
		to = from;
		from = tmp;
	}
	
	{
	int lastY = -1;
	std::vector<SPart *>::iterator it = m_parts.begin();
	for ( ; it != m_parts.end(); )
	{
		if ((*it)->y < lastY)
		{
			it--;
			MP_DELETE(*it);
			it = m_parts.erase(it);
			lastY = (*it)->y;
			continue;
		}

		lastY = (*it)->y;
		it++;
	}
	}

	//получаем длину строки
	int textLength = 0;
	std::vector<SPart*>::iterator it = m_parts.begin();
	std::vector<SPart*>::iterator itEnd = m_parts.end();

	for (; it != itEnd; it++)
	{
		textLength += (*it)->text.size();
	}

	//высчитываем координаты и размеры выделения текста
	it = m_parts.begin();
	for (; it != itEnd; it++)
	{
		(*it)->textSprite->ClearSelection();
	}

	//если не за пределами строки
	if (((to <= 0) || (from < 0)) || ((from >= textLength) || (to > textLength)) || (to == from))
	{
		return;
	}
	
	int partStart = 0;
	int currentPos = 0;
	bool foundFrom = false;
	bool foundTo = false;
	std::wstring bufString = L"";
	std::wstring text = L"";
	it = m_parts.begin();
	
	for (; it != itEnd; it++)
	{
		int fromPos = 0;
		text = (*it)->text;
		currentPos += text.size();
		partStart = currentPos - text.size();
		SPart* newPart = MP_NEW(SPart);
		newPart->isBold = (*it)->isBold;
			newPart->isItalic = (*it)->isItalic;
			newPart->isUnderline = (*it)->isUnderline;
			newPart->r = (*it)->r;
			newPart->_g = (*it)->_g;
			newPart->b = (*it)->b;
			newPart->x = (*it)->x;
			newPart->y = (*it)->y;
			newPart->textSprite->SetBoldStyle((*it)->textSprite->IsBoldEnabled());
			newPart->textSprite->SetItalicStyle((*it)->textSprite->IsItalicEnabled());
			newPart->textSprite->SetUnderlineStyle((*it)->textSprite->IsUnderlineEnabled());
			newPart->textSprite->SetFont((*it)->textSprite->GetFontName());
			newPart->textSprite->SetFontSize((*it)->textSprite->GetFontSize());
			newPart->height = (*it)->height;
			newPart->lineID = (*it)->lineID;
		
		if ((currentPos >= from) && (!foundFrom))
		{
			bufString = L"";
			for( ; (unsigned int)fromPos < text.size(); fromPos++)
			{
				if (fromPos == from - partStart)
					break;
				bufString += text[fromPos];
			}
			/*int pos = bufString.rfind(" ");
			if (pos != -1)
			{
				bufString.replace(pos, 1, "r");
			}*/
			newPart->text = bufString;
			newPart->textSprite->SetText(newPart->text);
			int selX = newPart->textSprite->GetRealWidth();
			int selY = 1;
			(*it)->textSprite->SetSelectionXY(selX, selY);

			foundFrom = true;
		}
		if ((currentPos < to) && (foundFrom))
		{
			if (fromPos == 0)
			{
				//start select for this part
				int selX = 0;
				int selY = 1;
				(*it)->textSprite->SetSelectionXY(selX, selY);
			}
			bufString.assign(text, fromPos, text.size() - fromPos);
			/*int pos = bufString.rfind(" ");
			if (pos != -1)
			{
				bufString.replace(pos, 1, "r");
			}*/
			newPart->text = bufString;
			newPart->textSprite->SetText(newPart->text);
			int selWidth = newPart->textSprite->GetRealWidth();
			int selHeight = newPart->textSprite->GetAlphabetHeight();//(int)(newPart->height * 1.25f);
			(*it)->textSprite->SetSelectionWH(selWidth, selHeight);
			(*it)->textSprite->SetSelectionCoords(fromPos, text.size());
			
			//it->getrealwidth, it->getrealheight
			//end select for this part with bufString
		}
		if (currentPos >= to)
		{
			if (!fromPos)
			{
				int selX = 0;
				int selY = 1;
				(*it)->textSprite->SetSelectionXY(selX, selY);
			}
			
			bufString.assign(text, fromPos, to - partStart - fromPos);
			/*int pos = bufString.rfind(" ");
			if (pos != -1)
			{
				bufString.replace(pos, 1, "r");
			}*/
			newPart->text = bufString;
			newPart->textSprite->SetText(newPart->text);
			int selWidth = newPart->textSprite->GetRealWidth();
			int selHeight = newPart->textSprite->GetAlphabetHeight();//(int)(newPart->height * 1.25f);
			(*it)->textSprite->SetSelectionWH(selWidth, selHeight);
			(*it)->textSprite->SetSelectionCoords(fromPos, to - partStart);
			foundTo = true;
		}
		MP_DELETE(newPart);
		if (foundTo)
		{
			break;
		}
	}
}

int CFormattedTextSprite::GetCursorX()const 
{
	return m_cursorX;
}

int CFormattedTextSprite::GetCursorY()const 
{
	return m_cursorY;
}

int CFormattedTextSprite::GetCursorPos()const 
{
	return m_cursorPos;
}

int CFormattedTextSprite::GetCursorHeight() 
{
	if (m_cursorHeight == 0)
	{
		if (m_parts.size() > 0)
		{
			std::vector<SPart*>::iterator it = m_parts.begin();
			m_cursorHeight = (unsigned char)((*it)->textSprite->GetRealHeight() * 1.25f);
		}
		else
		{
			return m_size;
		}
	}
	
	return m_cursorHeight;
}

void CFormattedTextSprite::SetCursorPos(const unsigned int pos)
{
	if (m_cursorPos > pos)
	{
		DecreaseCursor(m_cursorPos - pos);
	}
	else if (m_cursorPos < pos)
	{
		IncreaseCursor(pos - m_cursorPos);
	}
	return;
}

void CFormattedTextSprite::IncreaseCursor(unsigned int delta)
{
	//получаем длину строки
	unsigned int textLength = 0;
	std::vector<SPart*>::iterator it = m_parts.begin();
	std::vector<SPart*>::iterator itEnd = m_parts.end();

	for (; it != itEnd; it++)
	{
		textLength += (*it)->text.size();
	}

	//если не длиннее строки
	if (m_cursorPos + delta < textLength)
	{
		m_cursorPos += delta;
	}
	else
	{
		m_cursorPos = textLength;
	}

	//сдвигаем курсор по х и по у в зависимости от положения курсора
	unsigned int currentPos = 0;
	int curY = 0;

	it = m_parts.begin();

	for ( ; it != itEnd; it++)
	{		
		currentPos += (*it)->text.size();		
		if (currentPos >= m_cursorPos)
		{
			m_cursorX = (*it)->x;
			m_cursorY = (*it)->y;
			m_cursorHeight = (unsigned char)(*it)->height;
			if (m_cursorY != curY)
			{
				curY = (*it)->y;
				m_cursorY = curY;
			}

			currentPos -= (*it)->text.size();
			std::wstring bufString = L"";
			for (unsigned int i = 0; i < (*it)->text.size(); i++)
			{
				bufString += (*it)->text[i];
				currentPos += 1;
				if (currentPos == m_cursorPos)
				{					
					m_cursorX = (*it)->x + (*it)->textSprite->GetRealWidth(bufString);
					m_cursorY = (*it)->y;
					break;
				}
			}
			break;
		}		
	}
}

void CFormattedTextSprite::DecreaseCursor(unsigned int delta)
{
	if (m_cursorPos > delta)
	{
		m_cursorPos -= delta;
	}
	else
	{
		m_cursorPos = 0;
	}

	//сдвигаем курсор по х и по у в зависимости от положения курсора
	unsigned int currentPos = 0;
	int curY = 0;	

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{		
		currentPos += (*it)->text.size();		
		if (currentPos >= m_cursorPos)
		{			
			m_cursorX = (*it)->x;
			m_cursorY = (*it)->y;
			m_cursorHeight = (unsigned char)(*it)->height;
			if (m_cursorY != curY)
			{
				curY = (*it)->y;
				m_cursorY = curY;
			}

			currentPos -= (*it)->text.size();
			std::wstring bufString = L"";
			for (unsigned int i = 0; i < (*it)->text.size(); i++)
			{
				bufString += (*it)->text[i];
				currentPos++;
				if (currentPos == m_cursorPos)
				{					
					m_cursorX = (*it)->x + (*it)->textSprite->GetRealWidth(bufString);
					m_cursorY = (*it)->y;
					break;
				}
			}			
			break;
		}		
	}
}

void CFormattedTextSprite::SetDotsText(const std::wstring& text)
{
	if (text.empty())
	{
		return;
	}

	m_dotsText = text;
}

void CFormattedTextSprite::SetLineHeight(const short height)
{
	if (height > 0)
	{
		m_lineHeight = height;
	}	
}

void CFormattedTextSprite::SetPartSettings(SPart* const part, const unsigned char style, const short r, const short g, 
										   const short b, const int x, const int y, 
										   const short shadowR, const short shadowG, const short shadowB,
										   const short shadowOffsetX, const short shadowOffsetY, const int lineID,
											const char fontSize)
{
	part->isBold = (((style & STYLE_BOLD) > 0) || (m_isBold));
	part->isItalic = (((style & STYLE_ITALIC) > 0) || (m_isItalic));
	part->isUnderline = (((style & STYLE_UNDERLINE) > 0) || (m_isUnderline));
	part->r = r;
	part->_g = g;
	part->b = b;
	part->x = x;
	part->y = y;
	part->textSprite->SetScene2D(GetScene2D());
	part->textSprite->SetBoldStyle(part->isBold);

	part->textSprite->SetItalicStyle(part->isItalic);
	part->textSprite->SetUnderlineStyle(part->isUnderline);
	part->textSprite->SetFont(m_fontName);
	part->textSprite->SetFontSize((fontSize == -1) ? m_size : fontSize);
	part->isShadowEnabled = ((shadowR != -1) && (shadowG != -1) && (shadowB != -1));
	part->shadowR = shadowR;
	part->shadowG = shadowG;
	part->shadowB = shadowB;
	part->shadowOffsetX = shadowOffsetX; 
	part->shadowOffsetY = shadowOffsetY;
	part->height = 0;
	part->lineID = lineID;
}

void CFormattedTextSprite::UpdateLineHeight(const int lineID)
{
	int maxHeight = 0;
	int lineWidth = 0;

	std::vector<SPart *>::reverse_iterator it = m_parts.rbegin();
	std::vector<SPart *>::reverse_iterator itEnd = m_parts.rend();

	for ( ; it != itEnd; it++)
	if ((*it)->lineID == lineID)
	{
		if ((*it)->textSprite->GetAlphabetHeight() > maxHeight)
		{			
			maxHeight = (*it)->textSprite->GetAlphabetHeight();						
		}

		lineWidth += (*it)->textSprite->GetRealWidth();
	}
	else
	{
		break;
	}

	if (m_lineHeight != 0xFFFF) 
	{
		maxHeight = m_lineHeight;
	}

	it = m_parts.rbegin();

	for ( ; it != itEnd; it++)
    if ((*it)->lineID == lineID)
	{
		(*it)->height = maxHeight;
		(*it)->lineWidth = lineWidth;
	}
	else
	{
		break;
	}
}

unsigned char* CFormattedTextSprite::GetMemoryBuffer(const unsigned int bufferID, const unsigned int size, int defaultValue)
{	
#define MEMORY_BUFFER_COUNT	6
	static unsigned char* memoryBuffers[MEMORY_BUFFER_COUNT] = {NULL};
	static unsigned int memoryBuffersSize[MEMORY_BUFFER_COUNT] = {0};

	if (bufferID >= MEMORY_BUFFER_COUNT)
	{
		return NULL;
	}	

	if (size > memoryBuffersSize[bufferID])	
	{
		if (memoryBuffers[bufferID])
		{
			MP_DELETE_ARR(memoryBuffers[bufferID]);
		}		
		memoryBuffers[bufferID] = MP_NEW_ARR(unsigned char, size * 2);
		memoryBuffersSize[bufferID] = size * 2;		
	}

	memset(memoryBuffers[bufferID], defaultValue, size);

	return memoryBuffers[bufferID];
}

void CFormattedTextSprite::AddText(const int _currentY, const int _currentLineID, const std::wstring& text)
{
	int currentY = _currentY;
	int currentLineID = _currentLineID;
	m_text += text;

	m_sliced = false;
	int minLineID = currentLineID;

	if (text.empty())
	{
		return;
	}

	int charSize = sizeof( wchar_t);

	short* parseColor = (short *)GetMemoryBuffer(0, (text.size() + 1) * 12 * sizeof(short) * 2, -1);
	char* parseShadow = (char *)GetMemoryBuffer(1, (text.size() + 1) * 20 * sizeof(char) * 2, -1);
	unsigned char* parseAttribs = (unsigned char*)GetMemoryBuffer(2, (text.size() + 1) * sizeof(unsigned char) * 2, 0);	
	wchar_t* parseText = (wchar_t*)GetMemoryBuffer(3, (text.size() + 1) * charSize * 2, 0);		
	unsigned char* parseYOffset = (unsigned char*)GetMemoryBuffer(4, (text.size() + 1)  * sizeof(unsigned char) * 2, 0);	
	char* parseSize = (char *)GetMemoryBuffer(5, (text.size() + 1) * sizeof(char) * 2, -1);	

	unsigned int i;
	unsigned int currentChar = 0;
	for (i = 0; i < text.size(); i++)
	{
		bool isNewColorSet = false;

		std::wstring tag = L"";
	
		if (text[i] == L'<')
		{
			int pos = text.find_first_of(L'>', i);
			if (-1 != pos)
			{
				tag = text.substr(i, pos - i + 1);
			}
		}
		else if (text[i] == L'&')
		{
			tag = text.substr(i, 4);

			if (L"&lt;" == tag)
			{
				parseAttribs[currentChar + 1] = parseAttribs[currentChar];
				parseText[currentChar++] = LESS_TAG;
				i += 3;
				continue;
			}
			else if (L"&gt;" == tag)
			{
				parseAttribs[currentChar + 1] = parseAttribs[currentChar];
				parseText[currentChar++] = GREATER_TAG;
				i += 3;
				continue;
			}
		}

		if  (m_isTagsEnabled && m_isTagsAllowed) 
		{
			if (L"<nohtml>" == tag)
			{
				m_isTagsAllowed = false;
				i += 7;
				continue;
			}
		}
		else
		{
			if (m_isTagsEnabled)
			{
				if (L"</nohtml>" == tag)
				{
					m_isTagsAllowed = true;
					i += 8;
					continue;
				}
			}
			tag = L"";
		}
			

		if (L"<b>" == tag)
		{
			if (!(parseAttribs[currentChar] & STYLE_BOLD))
			{
				parseAttribs[currentChar] += STYLE_BOLD;
			}
			i += 2;
			continue;
		}
		else if (L"<i>" == tag)
		{
			if (!(parseAttribs[currentChar] & STYLE_ITALIC))
			{
				parseAttribs[currentChar] += STYLE_ITALIC;
			}
			i += 2;
			continue;
		}
		else if (L"<u>" == tag)
		{
			if (!(parseAttribs[currentChar] & STYLE_UNDERLINE))
			{
				parseAttribs[currentChar] += STYLE_UNDERLINE;
			}
			i += 2;
			continue;
		}
		else if (L"</b>" == tag)
		{
			if (parseAttribs[currentChar] & STYLE_BOLD)
			{
				parseAttribs[currentChar] -= STYLE_BOLD;
			}
			i += 3;
			continue;
		}
		else if (L"</i>" == tag)
		{
			if (parseAttribs[currentChar] & STYLE_ITALIC)
			{
				parseAttribs[currentChar] -= STYLE_ITALIC;
			}
			i += 3;
			continue;
		}
		else if (L"</u>" == tag)
		{
			if (parseAttribs[currentChar] & STYLE_UNDERLINE)
			{
				parseAttribs[currentChar] -= STYLE_UNDERLINE;
			}
			i += 3;
			continue;
		}
		else if (L"<br>" == tag)
		{
			parseAttribs[currentChar + 1] = parseAttribs[currentChar];
			parseText[currentChar] = 13;
			parseColor[(currentChar + 1) * 3 + 0] = parseColor[currentChar * 3 + 0];
			parseColor[(currentChar + 1) * 3 + 1] = parseColor[currentChar * 3 + 1];
			parseColor[(currentChar + 1) * 3 + 2] = parseColor[currentChar * 3 + 2];
			parseShadow[(currentChar + 1) * 5 + 0] = parseShadow[currentChar * 5 + 0];
			parseShadow[(currentChar + 1) * 5 + 1] = parseShadow[currentChar * 5 + 1];
			parseShadow[(currentChar + 1) * 5 + 2] = parseShadow[currentChar * 5 + 2];
			parseShadow[(currentChar + 1) * 5 + 3] = parseShadow[currentChar * 5 + 3];
			parseShadow[(currentChar + 1) * 5 + 4] = parseShadow[currentChar * 5 + 4];
			parseYOffset[currentChar] = 0;			
			currentChar++;			
			i += 3;
			continue;
		}
		else if (L"<br" == tag.substr(0, 3))
		{
			parseAttribs[currentChar + 1] = parseAttribs[currentChar];
			parseText[currentChar] = 13;
			currentChar++;

			int pos = tag.find(L"size=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 6);
				
				if (pos2 != -1)
				{
					std::wstring sizeStr = tag.substr(pos + 6, pos2 - pos - 6);

					unsigned char size = (unsigned char)_wtoi(sizeStr.c_str());
					if (currentChar > 0)
					{
						parseYOffset[currentChar - 1] = size;
					}
				}
			}

			i += tag.size() - 1;
			continue;
		}
		else if (L"<font" == tag.substr(0, 5))
		{
			int pos = tag.find(L"color=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 7);
				
				if (pos2 != -1)
				{
					std::wstring color = tag.substr(pos + 7, pos2 - pos - 7);
					if ((color.size() >= 6) && (color.size() <= 7))
					{
						if (L'#' == color[0])
						{
							color = color.substr(1, 6);
						}

						parseColor[currentChar * 3 + 0] = (short)HexStringToDec(color.substr(0, 2));
						parseColor[currentChar * 3 + 1] = (short)HexStringToDec(color.substr(2, 2));
						parseColor[currentChar * 3 + 2] = (short)HexStringToDec(color.substr(4, 2));
						
						isNewColorSet = true;
					}
				}
			}
			else
			{
				int pos = tag.find(L"color='");

				if (pos != -1)
				{
					int pos2 = tag.find_first_of(L"'", pos + 7);
				
					if (pos2 != -1)
					{
						std::wstring color = tag.substr(pos + 7, pos2 - pos - 7);
						if ((color.size() >= 6) && (color.size() <= 7))
						{
							if (L'#' == color[0])
							{
								color = color.substr(1, 6);
							}

							parseColor[currentChar * 3 + 0] = (short)HexStringToDec(color.substr(0, 2));
							parseColor[currentChar * 3 + 1] = (short)HexStringToDec(color.substr(2, 2));
							parseColor[currentChar * 3 + 2] = (short)HexStringToDec(color.substr(4, 2));
						}
					}
				}
			}

			pos = tag.find(L"size=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 6);
				
				if (pos2 != -1)
				{
					std::wstring size = tag.substr(pos + 6, pos2 - pos - 6);
					parseSize[currentChar] = (char)_wtoi(size.c_str());										
				}
			}
			else
			{
				int pos = tag.find(L"size='");

				if (pos != -1)
				{
					int pos2 = tag.find_first_of(L"'", pos + 6);
				
					if (pos2 != -1)
					{
						std::wstring size = tag.substr(pos + 6, pos2 - pos - 6);
						
						parseSize[currentChar] = (char)_wtoi(size.c_str());
					}
				}
			}

			i += tag.size() - 1;
			continue;
		}
		else if (L"<shadow" == tag.substr(0, 7))
		{
			// offset (-2, -2) and black color by default
			int x = -2;
			int y = -2;
			unsigned char shadowColor[3];
			memset(&shadowColor[0], 0, 3);

			int pos = tag.find(L"color=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 7);
				
				if (pos2 != -1)
				{
					std::wstring color = tag.substr(pos + 7, pos2 - pos);

					if (L'#' == color[0])
					{
						color = color.substr(1, 6);
					}

					shadowColor[0] = (unsigned char)HexStringToDec(color.substr(0, 2));
					shadowColor[1] = (unsigned char)HexStringToDec(color.substr(2, 2));
					shadowColor[2] = (unsigned char)HexStringToDec(color.substr(4, 2));
					
					isNewColorSet = true;
				}
			}
			else
			{
				int pos = tag.find(L"color='");

				if (pos != -1)
				{
					int pos2 = tag.find_first_of(L"'", pos + 7);
				
					if (pos2 != -1)
					{
						std::wstring color = tag.substr(pos + 7, pos2 - pos - 7);
						if (L'#' == color[0])
						{
							color = color.substr(1, 6);
						}

						shadowColor[0] = (unsigned char)HexStringToDec(color.substr(0, 2));
						shadowColor[1] = (unsigned char)HexStringToDec(color.substr(2, 2));
						shadowColor[2] = (unsigned char)HexStringToDec(color.substr(4, 2));
					}
				}
			}

			pos = tag.find(L"x=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 3);
				
				if (pos2 != -1)
				{
					std::wstring strX = tag.substr(pos + 3, pos2 - pos);					
					x = _wtoi(strX.c_str());
				}
			}
			else
			{
				pos = tag.find(L"x='");

				if (pos != -1)
				{
					int pos2 = tag.find_first_of(L"'", pos + 3);
				
					if (pos2 != -1)
					{
						std::wstring strX = tag.substr(pos + 3, pos2 - pos);					
						x = _wtoi(strX.c_str());
					}
				}
			}

			pos = tag.find(L"y=\"");

			if (pos != -1)
			{
				int pos2 = tag.find_first_of(L'"', pos + 3);
				
				if (pos2 != -1)
				{
					std::wstring strY = tag.substr(pos + 3, pos2 - pos);					
					y = _wtoi(strY.c_str());
				}
			}
			else
			{
				pos = tag.find(L"y='");

				if (pos != -1)
				{
					int pos2 = tag.find_first_of(L"'", pos + 3);
				
					if (pos2 != -1)
					{
						std::wstring strY = tag.substr(pos + 3, pos2 - pos);					
						y = _wtoi(strY.c_str());
					}
				}
			}

			parseShadow[currentChar * 5 + 0] = shadowColor[0];
			parseShadow[currentChar * 5 + 1] = shadowColor[1];
			parseShadow[currentChar * 5 + 2] = shadowColor[2];
			parseShadow[currentChar * 5 + 3] = (char)x;
			parseShadow[currentChar * 5 + 4] = (char)y;

			i += tag.size() - 1;
			continue;
		}
		else if (L"</font>" == tag)
		{
			parseColor[currentChar * 3 + 0] = -1;
			parseColor[currentChar * 3 + 1] = -1;
			parseColor[currentChar * 3 + 2] = -1;
			parseSize[currentChar] = -1;

			i += tag.size() - 1;
			continue;
		}
		else if (L"</shadow>" == tag)
		{
			parseShadow[currentChar * 5 + 0] = -1;
			parseShadow[currentChar * 5 + 1] = -1;
			parseShadow[currentChar * 5 + 2] = -1;
			parseShadow[currentChar * 5 + 3] = -1;
			parseShadow[currentChar * 5 + 4] = -1;

			i += tag.size() - 1;
			continue;
		}
		
		{
			parseColor[(currentChar + 1) * 3 + 0] = parseColor[currentChar * 3 + 0];
			parseColor[(currentChar + 1) * 3 + 1] = parseColor[currentChar * 3 + 1];
			parseColor[(currentChar + 1) * 3 + 2] = parseColor[currentChar * 3 + 2];
			parseShadow[(currentChar + 1) * 5 + 0] = parseShadow[currentChar * 5 + 0];
			parseShadow[(currentChar + 1) * 5 + 1] = parseShadow[currentChar * 5 + 1];
			parseShadow[(currentChar + 1) * 5 + 2] = parseShadow[currentChar * 5 + 2];
			parseShadow[(currentChar + 1) * 5 + 3] = parseShadow[currentChar * 5 + 3];
			parseShadow[(currentChar + 1) * 5 + 4] = parseShadow[currentChar * 5 + 4];
		
			parseText[currentChar] = text[i];
			parseAttribs[currentChar + 1] = parseAttribs[currentChar];
			parseSize[currentChar + 1] = parseSize[currentChar];
			currentChar++;
		}

		if (text[i] == 13)
		{			
			parseAttribs[currentChar + 1] = parseAttribs[currentChar];
			parseText[currentChar] = ' ';
			parseColor[(currentChar + 1) * 3 + 0] = parseColor[currentChar * 3 + 0];
			parseColor[(currentChar + 1) * 3 + 1] = parseColor[currentChar * 3 + 1];
			parseColor[(currentChar + 1) * 3 + 2] = parseColor[currentChar * 3 + 2];
			parseShadow[(currentChar + 1) * 5 + 0] = parseShadow[currentChar * 5 + 0];
			parseShadow[(currentChar + 1) * 5 + 1] = parseShadow[currentChar * 5 + 1];
			parseShadow[(currentChar + 1) * 5 + 2] = parseShadow[currentChar * 5 + 2];
			parseShadow[(currentChar + 1) * 5 + 3] = parseShadow[currentChar * 5 + 3];
			parseShadow[(currentChar + 1) * 5 + 4] = parseShadow[currentChar * 5 + 4];
			parseYOffset[currentChar] = 0;			
			currentChar++;
		}
	}

	wchar_t tmp[2];
	tmp[1] = 0;

	int currentX = 0;

	SPart* newPart = MP_NEW(SPart);
	SetPartSettings(newPart, parseAttribs[0], parseColor[0], parseColor[1], parseColor[2], currentX, currentY,
		parseShadow[0], parseShadow[1], parseShadow[2], parseShadow[3], parseShadow[4], currentLineID, parseSize[0]);
	m_parts.push_back(newPart);

	for (i = 0; i < currentChar; i++)
	switch (parseText[i])
	{
	case LESS_TAG:
		parseText[i] = L'<';
		break;

	case GREATER_TAG:
		parseText[i] = L'>';
		break;
	}

	for (i = 0; i < currentChar; i++)
	{
		if (((0 == i) || ((parseAttribs[i - 1] == parseAttribs[i]) && 
			(parseColor[(i - 1) * 3] == parseColor[i * 3]) && 
			(parseColor[(i - 1) * 3 + 1] == parseColor[i * 3 + 1]) && 
			(parseColor[(i - 1) * 3 + 2] == parseColor[i * 3 + 2]) &&
			(parseShadow[(i - 1) * 5] == parseShadow[i * 5]) && 
			(parseShadow[(i - 1) * 5 + 1] == parseShadow[i * 5 + 1]) && 
			(parseShadow[(i - 1) * 5 + 2] == parseShadow[i * 5 + 2]) &&
			(parseShadow[(i - 1) * 5 + 3] == parseShadow[i * 5 + 3]) &&
			(parseShadow[(i - 1) * 5 + 4] == parseShadow[i * 5 + 4]) &&
			(parseSize[i - 1] == parseSize[i])
			)) && (parseText[i] != 13))
		{
			tmp[0] = parseText[i];
			
			newPart->text += tmp;
			newPart->textSprite->SetText(newPart->text);

			if ((m_width > 0) && (m_width - currentX < newPart->textSprite->GetRealWidth()))
			{
				std::wstring newText;

				if (newPart->text.find(L" ") != -1)
				{
					int pos = newPart->text.find_last_of(L" ");

					newText = newPart->text.substr(pos + 1, newPart->text.size() - pos - 1);
					newPart->text = newPart->text.substr(0, pos + 1);
				}
				else
				{
					newPart->text = newPart->text.substr(0, newPart->text.size() - 1);
					newText = tmp;
					m_wordBreaks = true;
				}

				newPart->textSprite->SetText(newPart->text);
				//newPart->height = newPart->textSprite->GetRealHeight();
				newPart->height = newPart->textSprite->GetAlphabetHeight();
				UpdateLineHeight(newPart->lineID);

				currentX = 0;
				currentY += newPart->height;
				currentLineID++;

				newPart = MP_NEW(SPart);
				SetPartSettings(newPart, parseAttribs[i], parseColor[i * 3], parseColor[i * 3 + 1], parseColor[i * 3 + 2], currentX, currentY, 
					parseShadow[i * 5 + 0], parseShadow[i * 5 + 1], parseShadow[i * 5 + 2], 
					parseShadow[i * 5 + 3], parseShadow[i * 5 + 4],	currentLineID, parseSize[i]);
				m_parts.push_back(newPart);		

				newPart->text = newText;
				newPart->textSprite->SetText(newPart->text);
			}
		}
		else
		{
			tmp[0] = parseText[i];
			newPart->textSprite->SetText(newPart->text);
			if (newPart->text == L"")
			{
				SetPartSettings(newPart, parseAttribs[i], parseColor[i * 3], parseColor[i * 3 + 1], parseColor[i * 3 + 2], currentX, currentY, 
					parseShadow[i * 5 + 0], parseShadow[i * 5 + 1], parseShadow[i * 5 + 2], 
					parseShadow[i * 5 + 3], parseShadow[i * 5 + 4], currentLineID, parseSize[i]);				
				if (tmp[0] != 13)
				{
					newPart->text = tmp;				
				}				
			}
						
			newPart->height = newPart->textSprite->GetAlphabetHeight();
			UpdateLineHeight(newPart->lineID);
			
			currentX += newPart->textSprite->GetRealWidth();

			if (13 == parseText[i])
			{
				if (parseYOffset[i] != 0)
				{
					if (0 == currentX)
					{
						if (parseYOffset[i] > 0)
						{
							currentY += parseYOffset[i];
						}						
					}
					else
					{
						currentY += ((parseYOffset[i] > newPart->height) ? parseYOffset[i] : newPart->height);
					}
				}
				else
				{
					if (newPart->height != 0)
					{
						currentY += newPart->height;
					}
					else
					{
						std::vector<SPart *>::reverse_iterator it = m_parts.rbegin();
						std::vector<SPart *>::reverse_iterator itEnd = m_parts.rend();

						for ( ; it != itEnd; it++)
						if ((*it)->height > 0)
						{
							currentY += (*it)->height;
							break;
						}
					}
				}
				currentX = 0;				
				currentLineID++;
				tmp[0] = 0;
			}

			newPart = MP_NEW(SPart);
			SetPartSettings(newPart, parseAttribs[i], parseColor[i * 3], parseColor[i * 3 + 1], parseColor[i * 3 + 2], currentX, currentY, 
				parseShadow[i * 5 + 0], parseShadow[i * 5 + 1], parseShadow[i * 5 + 2], 
				parseShadow[i * 5 + 3], parseShadow[i * 5 + 4], currentLineID, parseSize[i]);
			m_parts.push_back(newPart);		

			if ((tmp[0] != 13) && (tmp[0] != 0))
			{
				newPart->text = tmp;
			}
			else
			{
				newPart->text = L"";
			}
			newPart->textSprite->SetText(newPart->text);
		}

		//added 11.10.2008 nikitin
		/*
		*	waste lines deleted
		*	last line gets slice text at it's back
		*/

		bool isPartAdded = false;

		if (!m_dotsText.empty())
		if ((m_height > 0) && (m_height - currentY <= newPart->textSprite->GetAlphabetHeight()))
		{
			if (m_parts.size() == 1)
			{
				if (m_parts[0]->text.size() == 1)
				{
					m_parts[0]->text = L"";
						m_sliced = true;
				}
			}

			if (!m_parts.empty())
			{
				if (m_parts.size() > 1)
				{
					if (!isPartAdded)
					{
						SPart* part1 = m_parts[m_parts.size() - 2];
						if ((m_width == 0) || (part1->textSprite->GetRealWidth(m_dotsText) + part1->textSprite->GetRealWidth() + part1->x < m_width))
						{
							SPart* part2 = m_parts[m_parts.size() - 1];
							part2->x = part1->x + part1->textSprite->GetRealWidth();
							part2->y = part1->y;						
							part2->lineID = part1->lineID;
						}
						else
						{
							isPartAdded = true;
						}
					}

					if (isPartAdded)
					{
						SPart* part = m_parts.back();
						MP_DELETE(part);
						m_parts.pop_back();			
					}	

					isPartAdded = true;
				}

				newPart = m_parts.back();				
			}

			if (m_parts.empty())
			{
				break;
			}

			if (m_parts.size() > 0)
			{
				std::vector<SPart *>::reverse_iterator it = m_parts.rbegin();
				std::wstring changeText;
				changeText = (*it)->text;
				if (changeText.size() >= m_dotsText.size())
				{
					changeText.replace(changeText.size() - m_dotsText.size(), m_dotsText.size(), m_dotsText);
					// весь текст не поместился в отведенное под него поле
					m_sliced = true;
				}
				else
				{
					changeText.replace(0, changeText.size(), m_dotsText);
					m_sliced = true;
				}
				(*it)->text = changeText;
				(*it)->textSprite->SetText((*it)->text);
			}

			break;
		}
		////added 11.10.2008 nikitin
	}

	newPart->height = newPart->textSprite->GetAlphabetHeight();
	UpdateLineHeight(currentLineID - 1);
	UpdateLineHeight(currentLineID);

	int lastY = -1;
	std::vector<SPart *>::iterator it = m_parts.begin();
	for ( ; it != m_parts.end(); )
	{
		if ((*it)->y < lastY)
		{
			it--;
			MP_DELETE(*it);
			it = m_parts.erase(it);
			lastY = (*it)->y;
			continue;
		}

		lastY = (*it)->y;
		it++;
	}

	it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	if ((*it)->lineID >= minLineID)
	{
		(*it)->y += ((*it)->height - (*it)->textSprite->GetAlphabetHeight());
	}	
}

void CFormattedTextSprite::AddText(const std::wstring& text)
{
	if (m_parts.size() == 0)
	{
		SetText(text);
	}
	else
	{
		SPart* part = m_parts[m_parts.size() - 1];

		int y = part->y + part->height;

		AddText(y, part->lineID + 1, text);
	}

	m_realWidth = GetRealWidth();
}

void CFormattedTextSprite::SetText(const std::wstring& text)
{
	if (m_text.length() == text.length())
	if ((m_text == text) && (m_lastWidth == GetWidth()) && (m_lastHeight == GetHeight()))
	{
		return;
	}

	m_wordBreaks = false;

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);
	}

	m_parts.clear();	
	m_text = L"";
	m_lastWidth = GetWidth();
	m_lastHeight = GetHeight();

	AddText(0, 0, text);

	m_realWidth = GetRealWidth();
}

int CFormattedTextSprite::GetRealWidth()
{
	if (0 == m_parts.size())
	{
		return 0;
	}

	UpdateIfNeeded();

	int width = 0;

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{
		int fragmentWidth = (*it)->x + (*it)->textSprite->GetRealWidth();
		width = (fragmentWidth > width) ? fragmentWidth : width;
	}

	return width;
}

int CFormattedTextSprite::GetRealHeight()
{
	if (0 == m_parts.size())
	{
		return 0;
	}

	UpdateIfNeeded();

	std::vector<SPart *>::reverse_iterator it = m_parts.rbegin();
	std::vector<SPart *>::reverse_iterator itEnd = m_parts.rend();

	for ( ; it != itEnd; it++)
	if ((!(*it)->text.empty()) && ((*it)->text.size() > 0))
	{
		return (*it)->y + (*it)->textSprite->GetAlphabetHeight();
	}

	return 0;
}


void CFormattedTextSprite::Draw(CPolygons2DAccum* const accum)
{
	Draw(m_x, m_y, accum);
}


void CFormattedTextSprite::GetVisibleBounds(int& _begin, int& _end)
{
	if (m_parts.size() == 0)
	{
		return;
	}

	UpdateIfNeeded();

	int size = 0;

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{
		size += (*it)->text.size();

		if (((*it)->y + (*it)->height <= m_height) || (0 == m_height) || (0 == (*it)->y) || (-1 != m_scissorsWidth))
		{
			int addX = 0;
			int width = (m_width != 0) ? m_width : m_realWidth;
			switch (m_horizontalAlign)
			{
			case ALIGN_CENTER:
				addX = (width - (*it)->lineWidth) / 2;
				break;

			case ALIGN_RIGHT:
				addX = (width - (*it)->lineWidth);
				break;
			}

			if (m_scissorsWidth != -1)
			{
				if ((!((*it)->y + m_y > m_scissorsY + m_scissorsHeight)) && (!((*it)->y + (*it)->height + m_y < m_scissorsY)))
				{
					if (_begin == 0 )
					{
						_begin = size - (*it)->text.size(); 
					}
					_end = size;

					//(*it)->textSprite->Draw((*it)->x + x + addX, (*it)->y + y, accum);
				}
			}
			else
			{
				if (_begin == 0 )
					_begin = size - (*it)->text.size(); 

				_end = size;
				//(*it)->textSprite->Draw((*it)->x + x + addX, (*it)->y + y, accum);
			}
		}
	}
}

void CFormattedTextSprite::UpdateIfNeeded()
{
	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{
		(*it)->textSprite->SetBoldStyle(((m_isBold) | ((*it)->isBold)));
		(*it)->textSprite->SetItalicStyle(((m_isItalic) | ((*it)->isItalic)));
		(*it)->textSprite->SetUnderlineStyle(((m_isUnderline) | ((*it)->isUnderline)));
		if ((*it)->r == -1)
		{
			(*it)->textSprite->SetColor(m_r, m_g, m_b, m_a);
		}
		else
		{
			(*it)->textSprite->SetColor((unsigned char)(*it)->r, (unsigned char)(*it)->_g, (unsigned char)(*it)->b, 255);
		}
		(*it)->textSprite->SetBkgColor(m_bkg_r, m_bkg_g, m_bkg_b, m_bkg_a);
		(*it)->textSprite->SetVerticalAlign(m_vAlign);
		(*it)->textSprite->SetAntialiasing(m_isAntialiasing);
	}

	if (m_parts.size() > 0)
	{
		SetResourceID((*m_parts.begin())->textSprite->GetFontID());
	}
}

void CFormattedTextSprite::Draw(const int x, const int y, CPolygons2DAccum* const accum)
{
	if (m_parts.size() == 0)
	{
		return;
	}

	OnBeforeDraw(accum);

	UpdateIfNeeded();

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();
	for ( ; it != itEnd; it++)
	if (((*it)->y + (*it)->height + y > 0) && ((*it)->y + y < (int)g->stp.GetCurrentHeight()) && ((*it)->x + x < (int)g->stp.GetCurrentWidth()))
	if (((*it)->y + (*it)->height <= m_height) || (0 == m_height) || (0 == (*it)->y) || (-1 != m_scissorsWidth))
	{
		int addX = 0;
		int width = (m_width != 0) ? m_width : m_realWidth;
		switch (m_horizontalAlign)
		{
		case ALIGN_CENTER:
			addX = (width - (*it)->lineWidth) / 2;
			break;

		case ALIGN_RIGHT:
			addX = (width - (*it)->lineWidth);
			break;
		}

		if (m_scissorsWidth != -1)
		{
			if ((!((*it)->y + y > m_scissorsY + m_scissorsHeight)) && (!((*it)->y + (*it)->height + y < m_scissorsY)))
			{
				if ((*it)->isShadowEnabled)
				{
					(*it)->textSprite->SetColor((unsigned char)(*it)->shadowR, (unsigned char)(*it)->shadowG, (unsigned char)(*it)->shadowB, 255);
					(*it)->textSprite->Draw((*it)->x + x + (*it)->shadowOffsetX + addX, (*it)->y + y + (*it)->shadowOffsetY, accum);

					if ((*it)->r == -1)
					{
						(*it)->textSprite->SetColor(m_r, m_g, m_b, m_a);
					}
					else
					{
						(*it)->textSprite->SetColor((unsigned char)(*it)->r, (unsigned char)(*it)->_g, (unsigned char)(*it)->b, 255);
					}
				}

				(*it)->textSprite->Draw((*it)->x + x + addX, (*it)->y + y, accum);
			}
		}
		else
		{
			if ((*it)->isShadowEnabled)
			{
				(*it)->textSprite->SetColor((unsigned char)(*it)->shadowR, (unsigned char)(*it)->shadowG, (unsigned char)(*it)->shadowB, 255);
				(*it)->textSprite->Draw((*it)->x + x + (*it)->shadowOffsetX + addX, (*it)->y + y + (*it)->shadowOffsetY, accum);

				if ((*it)->r == -1)
				{
					(*it)->textSprite->SetColor(m_r, m_g, m_b, m_a);
				}
				else
				{
					(*it)->textSprite->SetColor((unsigned char)(*it)->r, (unsigned char)(*it)->_g, (unsigned char)(*it)->b, 255);
				}
			}

			(*it)->textSprite->Draw((*it)->x + x + addX, (*it)->y + y, accum);
		}
	}

	OnAfterDraw(accum);
}

bool CFormattedTextSprite::IsPixelTransparent(const int tx, const int ty)const
{
	if ((tx < 0) || ((m_width > 0) && (tx >= m_width)))
	{
		return true;
	}

	if ((ty < 0) || ((m_height > 0) && (ty >= m_height)))
	{
		return true;
	}

	int absX = GetX() + tx;
	int absY = GetY() + ty;
	int maskX, maskY, maskWidth, maskHeight;
	GetScissors(maskX, maskY, maskWidth, maskHeight); 
	if (maskWidth != -1)
	{
		if ((absX < maskX) || (absX > maskX + maskWidth) ||
			(absY < maskY) || (absY > maskY + maskHeight))
		{
			return true;
		}
	}

	return false;
}

void CFormattedTextSprite::PrintDebugInfo()
{
	//g->lw.WriteLn("Text element ", GetName(), " has text [", m_text, "] part count [", m_parts.size(), "]");

	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for (int partID = 0; it != itEnd; it++, partID++)
	{
		//g->lw.WriteLn("part [", partID, "] text [", (*it)->textSprite->GetText(), "] x [", (*it)->x, "] y [", (*it)->y, "] ");
	}
}

void CFormattedTextSprite::SetBkgColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	m_bkg_r = r;
	m_bkg_g = g;
	m_bkg_b = b;
	m_bkg_a = a;

	UpdateIfNeeded();
}

void CFormattedTextSprite::SetFont(const std::string& fontName)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetFont(fontName);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetFont(fontName);
}

void CFormattedTextSprite::SetFontSize(const unsigned char size)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetFontSize(size);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetFontSize(size);
}

void CFormattedTextSprite::SetColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetColor(r, g, b, a);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetColor(r, g, b, a);
}

void CFormattedTextSprite::SetItalicStyle(const bool isItalic)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetItalicStyle(isItalic);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetItalicStyle(isItalic);
}

void CFormattedTextSprite::SetBoldStyle(const bool isBold)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetBoldStyle(isBold);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetBoldStyle(isBold);
}

void CFormattedTextSprite::SetUnderlineStyle(const bool isUnderline)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetUnderlineStyle(isUnderline);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetUnderlineStyle(isUnderline);
}

void CFormattedTextSprite::SetAntialiasing(const bool isEnabled)
{
	FOR_ALL_PARTS
	{
		(*it)->textSprite->SetAntialiasing(isEnabled);
		(*it)->textSprite->UpdateIfNeeded();
	}
	CTextSprite::SetAntialiasing(isEnabled);
}

CFormattedTextSprite::~CFormattedTextSprite()
{
	std::vector<SPart *>::iterator it = m_parts.begin();
	std::vector<SPart *>::iterator itEnd = m_parts.end();

	for ( ; it != itEnd; it++)
	{
		MP_DELETE(*it);	
	}
}