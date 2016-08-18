
#include "StdAfx.h"
#include "TextTool.h"
#include "GlobalVars.h"
#include "GlobalParams.h"
#include "ToolsParameters.h"
#include "CommonToolRoutines.h"
#include "GlobalSingletonStorage.h"

CFont* glFont;
std::wstring glText;
int glX;
int glY;
bool glIsFinal;
unsigned char glOpacity;
unsigned char glInvScale;

#define TEXT_OPTIMIZATION	1

void TextToolDraw()
{
	if (!glFont)
	{
		return;
	}
	g->stp.PushMatrix();
	GLFUNC(glScalef)(1.0f, -1.0f, 1.0f);
	g->stp.SetState(ST_CULL_FACE, false);
	g->stp.SetState(ST_ZTEST, false);
	g->stp.SetColor(255, 255, 255);
	if (!glIsFinal)
	{
		glText += L"|";
	}	
	glFont->SetColor(glColorR, glColorG, glColorB, glOpacity);
	glFont->SetTextScale(1.0f / glInvScale);
	glFont->OutTextXY(glX, glY, glText, FONT_ALIGN_LEFT, FONT_VALIGN_TOP);
	glFont->SetTextScale(1.0f);	
	g->stp.SetState(ST_CULL_FACE, true);
	g->stp.SetState(ST_ZTEST, true);
	g->stp.PopMatrix();
}


#define ANTIALIAS_SETTING		false
    
CTextTool::CTextTool():
	m_isNeedToCorrectCursor(false),
	m_workMode(WORK_MODE_FREE),
	m_isAntialiasing(false),
	m_isFontChanged(false),
	m_color(0, 0, 0, 255),
	m_isUnderline(false),
	MP_STRING_INIT(m_fontName),
	m_isWorking(false),
	m_textOpacity(128),
	m_isItalic(false),
	m_isBold(true),
	m_fontSize(48),
	MP_WSTRING_INIT(m_text),
	MP_WSTRING_INIT(m_bufferText),
	m_x(0), 
	m_y(0),
	m_pasteX(0),
	m_pasteY(0)
{	
	m_fontName = "Arial";
	m_font = g->fm.GetFont(m_fontName, m_fontSize, m_isBold, m_isItalic, m_isUnderline, ANTIALIAS_SETTING);
	assert(m_font);	
}

void CTextTool::UpdateFont()
{
	if (m_isFontChanged)
	{
		if (m_font)
		{
			g->fm.DeleteFont(m_font);
		}		

		m_font = g->fm.GetFont(m_fontName, m_fontSize, m_isBold, m_isItalic, m_isUnderline, ANTIALIAS_SETTING);
		assert(m_font);

		m_isFontChanged = false;
	}
}

void CTextTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CTextTool::SetParameter(int paramID, void* param)
{	
	switch (paramID)
	{
	case TEXT_FONT_NAME:
		if (m_fontName != *(std::string *)param)
		{
			m_fontName = *(std::string *)param;
			m_isFontChanged = true;
		}
		break;

	case TEXT_COLOR:
		{
			unsigned int tmp = (unsigned int)param;
			m_color = *(CColor4 *)&tmp;
		}
		break;

	case TEXT_SIZE:
		if (m_fontSize != (unsigned char)param)
		{
			m_fontSize = (unsigned char)param;
			m_isFontChanged = true;
		}
		break;

	case TEXT_BOLD:
		if (m_isBold != (param != NULL))
		{
			m_isBold = (param != NULL);
			m_isFontChanged = true;
		}
		break;

	case TEXT_ITALIC:
		if (m_isItalic != (param != NULL))
		{
			m_isItalic = (param != NULL);
			m_isFontChanged = true;
		}
		break;

	case TEXT_UNDERLINE:
		if (m_isUnderline != (param != NULL))
		{
			m_isUnderline = (param != NULL);
			m_isFontChanged = true;
		}
		break;

	case TEXT_ANTIALIASING:
		//m_isAntialiasing = (param != NULL);
		break;

	case TEXT_OPACITY:
		m_textOpacity = (unsigned char)param;
		break;

	case TEXT_NEW_CURSOR_POSITION:
		m_isNeedToCorrectCursor = (param != NULL);
		break;
	}

#ifdef TEXT_OPTIMIZATION
	if (!g_isPlaying)
#endif
	if (WORK_MODE_FREE != m_workMode)
	{
		glIsFinal = false;
		Draw(m_x, m_y);
	}
}

void CTextTool::OnMouseMove(int x, int y)
{
	if (m_isWorking)
	{
		m_x = x;
		m_y = y;
				
		glIsFinal = false;
		MiddleMoving();
	}
}

void CTextTool::OnLMouseDown(int x, int y)
{
	m_x = x;
	m_y = y;
	m_isWorking = true;
	if (WORK_MODE_FREE == m_workMode)
	{
		m_workMode = WORK_MODE_START;
	}

	glIsFinal = false;
	MiddleMoving();
}

void CTextTool::OnLMouseUp(int x, int y)
{
	if (m_isWorking)
	{
		m_isWorking = false;
		m_x = x;
		m_y = y;
		m_pasteX = x;
		m_pasteY = y;
		
		glIsFinal = false;
		MiddleMoving();
	}
}

void CTextTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CTextTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CTextTool::OnChar(int keyCode, int scancode)
{
	if (WORK_MODE_FREE == m_workMode)
	{
		return;
	}

	if ((keyCode  == 0 && scancode == 17) || (keyCode == 22 ))
	{
		return;
	}

	static int cnt = 0;
	cnt++;

	if (8 == keyCode)
	{
		if (m_text.length() > 0)
		{
			m_text = m_text.substr(0, m_text.size() - 1);

			glIsFinal = false;
			Draw(m_x, m_y);
		}
	}
	else if (13 == keyCode)
	{
		glIsFinal = true;
		if (m_workMode == WORK_MODE_PASTE)
		{
			Draw(m_pasteX, m_pasteY);
			m_y += m_lastTextHeight;
		}
		else
		{
			Draw(m_x, m_y);			
			if (m_font)
			{
				m_y += m_font->GetAlphabetHeight();		
			}
		}

		m_workMode = WORK_MODE_TYPING;
		m_drawingTarget->SaveBackBuffer();
		m_text = L"";
		glIsFinal = false;
		m_pasteX = m_x;
		m_pasteY = m_y;
		Draw(m_x, m_y);
	}
	else if (37 == scancode)
	{
		m_x--; 
		Draw(m_x, m_y);
	}
	else if (38 == scancode)
	{
		m_y--; 
		Draw(m_x, m_y);
	}
	else if (39 == scancode)
	{
		m_x++; 
		Draw(m_x, m_y);
	}
	else if (40 == scancode)
	{
		m_y++; 
		Draw(m_x, m_y);
	}
	else
	{
		wchar_t tmp[2];
		tmp[0] = (wchar_t)keyCode;
		tmp[1] = 0;

		m_text += tmp;
		glIsFinal = false;

#ifdef TEXT_OPTIMIZATION
		if (!g_isPlaying)
		{
			if 	(m_workMode == WORK_MODE_PASTE)
			{
				Draw(m_pasteX, m_pasteY);
			}
			else
			{
#endif
				Draw(m_x, m_y);
#ifdef TEXT_OPTIMIZATION
			}
		}
#endif
	}
}
void CTextTool::RedoAction(void* /*data*/)
{
}

void CTextTool::PushWord(bool isDrawAnything)
{
	if (!isDrawAnything)
	if (m_bufferPasteY == m_pasteY)
	{
		m_bufferText += m_text;
	}

	if (isDrawAnything)
	{
		m_pasteY++;
	}

	if (m_bufferPasteY != m_pasteY)
	{
		if (m_bufferPasteY < (int)m_drawingTarget->GetHeight())
		{
			int tmpX = m_x;
			int tmpY = m_y;
			m_x = m_bufferPasteX;
			m_y = m_bufferPasteY;
			std::wstring tempText = m_text;
			m_text = m_bufferText;		
			FastDraw(m_x, m_y);
			m_text = tempText;
			m_x = tmpX;
			m_y = tmpY;
		}
		m_bufferPasteY = m_pasteY;
		m_bufferText = m_text;
	}
}

void CTextTool::PasteText(wchar_t* result, bool isPasteOnIconClick)
{
	UpdateFont();
	if (!m_font)
	{
		return;
	}

	int width = 0;
	glIsFinal = true;
	wchar_t ch[2];
	ch[1] = '\0';

	//m_drawingTarget->SaveBackBuffer();
	m_drawingTarget->RestoreBackBuffer();
	m_drawingTarget->BindToApplyGeometry();


	// по нажатию на иконку текст всегда вставляем с верхнего левого угла 
	// очищая перед этим доску
	if ( (isPasteOnIconClick) || ((m_x == 0 ) && (m_y == 0)))
	{
		m_x = 20;
		m_y	= 20;

		if (m_isNeedToCorrectCursor)
		{
			m_y += m_font->GetAlphabetHeight();
		}

		m_pasteX = m_x;
		m_pasteY = m_y;		
	}

	m_bufferText = L"";
	m_bufferPasteY = m_y;
	m_bufferPasteX = m_x;

	unsigned int textLength = wcslen( result);
	
	for ( unsigned int i = 0; i < textLength; i++)
	{		
		if (m_pasteY > (int)m_drawingTarget->GetHeight())
		{
			m_drawingTarget->UnbindToApplyGeometry();
			m_drawingTarget->SaveBackBuffer();
			glIsFinal = false;
			m_text = L"";
			return;
		}

		if (result[i] == 9 || result[i]==10)
		{				
			ch[0]  = ' ';
			m_text +=ch;
		}
		else if ( (result[i]== 32) || (result[i]== 13) || (i == textLength - 1) )
		{			
     		if (i == textLength - 1)
			{				
				ch[0] = result[i];
				m_text += ch;
			}			

			width = m_pasteX + m_font->GetStringWidth(m_text);			

			if (width > (int)m_drawingTarget->GetWidth()) 
			{				
				// переходим на новую строчку
				if (m_x != m_pasteX)				
				{
					m_y += m_font->GetAlphabetHeight();
					m_pasteY = m_y;
					m_pasteX = m_x;
				}

				// рисуем текст на новой позиции				
				//Draw(m_pasteX, m_pasteY);
				if (result[i]!= 32)
				{
					PushWord();
				}
				//m_drawingTarget->SaveBackBuffer();
				m_workMode = WORK_MODE_TYPING;

				// начинаем новый абзац
				if (result[i]== 13)
				{					
					m_y += m_font->GetAlphabetHeight();	
					m_text = L"";
					//g->lw.WriteLn("Draw");
					//Draw(m_x, m_y);
					m_pasteY = m_y;
					m_pasteX = m_x;
				}
				else if (result[i]== 32)
				{					
					ch[0] = ' ';
					m_text+= ch;
					PushWord();
					m_pasteX = m_x + m_font->GetStringWidth(m_text);					
					m_text = L"";	
					//g->lw.WriteLn("Draw");
					//Draw(m_pasteX, m_pasteY);
				}
			}
			else if(result[i]== 13)
			{				
				//Draw(m_pasteX, m_pasteY);
				PushWord();
				//m_drawingTarget->SaveBackBuffer();
				m_workMode = WORK_MODE_TYPING;

				m_y += m_font->GetAlphabetHeight();	
				m_text = L"";
				//g->lw.WriteLn("Draw");
				//Draw(m_x, m_y);
				m_pasteY = m_y;
				m_pasteX = m_x;
			}
			else if ((result[i]== 32) || (i == textLength-1))
			{				
				if (result[i]== 32)
				{					
					ch[0] = ' ';
					m_text+= ch;
				}
				
				PushWord();
				//Draw(m_pasteX, m_pasteY);
				//m_drawingTarget->SaveBackBuffer();
				m_workMode = WORK_MODE_TYPING;
				
				m_pasteX = m_pasteX + m_font->GetStringWidth(m_text);

				m_text=L"";

				//g->lw.WriteLn("Draw");
				//Draw(m_pasteX, m_pasteY);
				//m_drawingTarget->SaveBackBuffer();

			}	
		}
		else
		{			
			// если слово не вмещается в строку (m_x + (длина слова) ), то переносим по буквам
			ch[0] = result[i];

			width = m_x + m_font->GetStringWidth(m_text+ch);

			if (width > (int)m_drawingTarget->GetWidth()) 
			{ 				
				if (m_x != m_pasteX)
				{					
					m_y += m_font->GetAlphabetHeight();	
					m_pasteY = m_y;
					m_pasteX = m_x;
				}
				
				//Draw(m_pasteX, m_pasteY);
				PushWord();
				//m_drawingTarget->SaveBackBuffer();
				m_workMode = WORK_MODE_TYPING;

				m_y += m_font->GetAlphabetHeight();	
				m_text = L"";
				//g->lw.WriteLn("Draw");
				//Draw(m_x, m_y);
				//m_drawingTarget->SaveBackBuffer();
				m_pasteY = m_y;
				m_pasteX = m_x;
			}

			ch[0] = result[i];
			m_text+= ch;
		}
	}	

	glIsFinal = true;

	//Draw(m_pasteX, m_pasteY);
	PushWord(true);

	m_drawingTarget->UnbindToApplyGeometry();

	m_drawingTarget->SaveBackBuffer();
	m_workMode = WORK_MODE_TYPING;

	// после вставки надо сдвинуть m_x
	width = m_pasteX + m_font->GetStringWidth(m_text.c_str());

	//высота текста, если после вставки сразу придет OnChar(13)
	m_lastTextHeight = m_font->GetAlphabetHeight();	

	if (width < (int)m_drawingTarget->GetWidth())
	{
		m_pasteX = width;
	}
	else
	{
		m_pasteX = m_x;
		m_y += m_lastTextHeight;	
		m_pasteY = m_y;
	}

	glIsFinal = false;
	m_text = L"";

#ifdef TEXT_OPTIMIZATION
	if (!g_isPlaying)
	{
#endif      
		m_workMode = WORK_MODE_START;
		Draw(m_pasteX, m_pasteY);	
		m_workMode = WORK_MODE_PASTE;
#ifdef TEXT_OPTIMIZATION
	}
#endif
}

void CTextTool::FastDraw(int x, int y)
{
	UpdateFont();
	if (!m_font)
	{
		return;
	}

	glInvScale = (unsigned char)m_drawingTarget->GetInvScale();
	glX = x / glInvScale;
	glY = (y - (m_isNeedToCorrectCursor ? 1 : 0) * m_font->GetAlphabetHeight()) / glInvScale;
	glText = m_text;
	glFont = m_font;	
	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;
	glOpacity = m_textOpacity;

	m_drawingTarget->ApplyGeometry(TextToolDraw);
}

void CTextTool::Draw(int x, int y)
{
	UpdateFont();
	if (!m_font)
	{
		return;
	}

	glInvScale = (unsigned char)m_drawingTarget->GetInvScale();
	glX = x / glInvScale;
	glY = (y - (m_isNeedToCorrectCursor ? 1 : 0) * m_font->GetAlphabetHeight()) / glInvScale;
	glText = m_text;
	glFont = m_font;	
	glColorR = m_color.r;
	glColorG = m_color.g;
	glColorB = m_color.b;
	glOpacity = m_textOpacity;

	if (m_workMode == WORK_MODE_START)
	{
		m_drawingTarget->SaveBackBuffer();
		m_workMode = WORK_MODE_TYPING;
	}
	else
	{
		m_drawingTarget->RestoreBackBuffer();
	}
	
	m_drawingTarget->ApplyGeometry(TextToolDraw);
}
    
void CTextTool::OnSelected() 
{
	m_text = L"";
	m_workMode = WORK_MODE_FREE;
	m_pasteX = m_x;
	m_pasteY = m_y;
}

void CTextTool::OnDeselected()
{
	if (m_workMode != WORK_MODE_FREE)
	{		
		glIsFinal = true;
		Draw(m_x, m_y);
		m_text = L"";
		m_workMode = WORK_MODE_FREE;
		m_drawingTarget->SaveBackBuffer();
	}
}

void CTextTool::OnEndRestoreState()
{
	glIsFinal = true;
	m_drawingTarget->SaveBackBuffer();
}

int CTextTool::GetSynchRequiredType()
{
	return SYNCH_KEYS;
}

void CTextTool::Destroy()
{
	MP_DELETE_THIS;
}

void CTextTool::MiddleMoving()
{	
	UpdateFont();
	if (!m_font)
	{
		return;
	}

	int y = m_y - m_font->GetStringHeight(m_text) / 2;
	int x = m_x - m_font->GetStringWidth(m_text) / 2;

	Draw(x, y);

	m_x = x;
	m_y = y;
}

CTextTool::~CTextTool()
{
	if (m_font)
	{
		g->fm.DeleteFont(m_font);
	}
}