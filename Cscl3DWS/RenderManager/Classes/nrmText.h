// nrmText.h: interface for the nrmText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_nrmText_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_nrmText_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "TextSprite.h"
#include "FormattedTextSprite.h"

class nrmText :		public nrmObject,
					public nrmMOIVisibleStub,
					public moIButton,
					public moIText
{
public:
	nrmText(mlMedia* apMLMedia);
	virtual ~nrmText();

protected:
	void ReformatText();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
	moIButton* GetIButton(){ return this; }
	moIText* GetIText(){ return this; }

// реализация  moILoadable
public:
	bool SrcChanged(); // загрузка нового ресурса
	bool Reload(){return false;} // перегрузка ресурса по текущему src

// реализация  moIVisible
public:
	// Свойства
	void AbsXYChanged(); // изменилось абсолютное положение 
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютная глубина
	void AbsVisibleChanged(); // изменилась абсолютная видимость
	void AbsOpacityChanged(); // изменилась абсолютная видимость
	mlSize GetMediaSize(); 
	void SetMask(int aiX, int aiY, int aiW, int aiH);
	int IsPointIn(const mlPoint /*aPnt*/);

	bool AreWordBreaksExists();
	
// реализация  moIButton
public:
	void onKeyDown(){}
	void onKeyUp(){}

	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged();

// реализация  moIText
public:
	// Свойства`
	void ValueChanged();
	void StylesChanged();
	void AdjustChanged(){}
	void HtmlChanged();
	void BkgColorChanged();
	unsigned int GetScrollTop(){ return 0; }
	void SetScrollTop(unsigned int /*auValue*/){}
	unsigned int GetScrollLeft(){ return 0; }
	void SetScrollLeft(unsigned int /*auValue*/){};
	// Методы
	void ScrollToLine(unsigned int /*auLine*/){}
	void AddText(wchar_t* str);
	int GetCursorX();
	int GetCursorY();
	int GetCursorPos();
	int GetCursorHeight();
	void SetCursorPos(int pos);
	void IncreaseCursor(unsigned int delta);
	void DecreaseCursor(unsigned int delta);
	void GetVisibleBounds(int& _begin, int& _end);
	void ToClipboard(wchar_t* str);
	std::wstring FromClipboard();
	void SetSelection(int from, int to);
	bool GetSliced();
	// Прочие
	bool GetTextFormattingInfo(mlSynchData &aData);  // для Authoring-а
	virtual	void	OnSetRenderManager();

	int GetSymbolIDByXY(int x, int y);

protected:
	virtual void	UpdateMask( nrmImage* apMask);

public:
	CFormattedTextSprite*	m_text;
};

#endif // !defined(AFX_nrmText_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
