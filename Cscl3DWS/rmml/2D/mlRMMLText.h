#pragma once


namespace rmml {

/**
 * Класс текстового элемента RMML
 */

class mlRMMLText :
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLPButton,
					public mlIText,
					public moIText
{
	JSString* value;
	bool mbStylesRef; // pStyles is reference to element
	JSObject* mjsoStyles;
	bool mbWidthIsSetInJS;
	bool mbHeightIsSetInJS;
public:
	mlRMMLText(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLText(void);
MLJSCLASS_DECL
private:
	enum {
		JSPROP_value = 1,
		JSPROP_style,
		JSPROP_html,
		JSPROP_scrollTop,
		JSPROP_scrollLeft,
		JSPROP_sliceTail,
		// ширина, занимаемая текстом
		JSPROP_textWidth,
		// высота, занимаемая текстом
		JSPROP_textHeight,
		// если "истина", то высота элемента постраивается автоматически
		// в зависимости от количества текста
		JSPROP_adjustHeight,
		// цвет подложки на которую выводиться текст
		// '#RGB' '#RRGGBB' '#AARRGGBB'
		JSPROP_bkgColor,
		JSPROP_pageSize, // размер 
		JSPROP_textSize, // размер текста в строчках (максимальная позиция скролинга в строчках)
	};

	JSFUNC_DECL(scrollToLine);
	JSFUNC_DECL(addText);
	JSFUNC_DECL(getCursorPos);
	JSFUNC_DECL(increaseCursor);
	JSFUNC_DECL(decreaseCursor);
	JSFUNC_DECL(getVisibleBounds);
	JSFUNC_DECL(setCursorPos);
	JSFUNC_DECL(toClipboard);
	JSFUNC_DECL(fromClipboard);
	JSFUNC_DECL(setSelection);
	JSFUNC_DECL(isSliced);
	JSFUNC_DECL(getSymbolIDByXY);
	JSFUNC_DECL(areWordBreaksExists);

	JSString* style;
	bool html;
	JSString* sliceTail;
	bool adjustHeight;
	unsigned int textWidth, textHeight;
	mlColor bkgColor;
	unsigned int textSize, pageSize;

	mlRMMLElement* FindStyles(JSString* ajssStylesName);

	void CreateOwnStylesObject(const wchar_t* apwcStyle);

	void BeforeTextReformatting();
	void AfterTextReformatting();

// реализация mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	mlresult SetValue(wchar_t* apwcValue);
	mlString GetValue(int aiIndent);
	static void PropIsSet(mlRMMLElement* apThis, char idProp);

// реализация  mlILoadable
MLILOADABLE_IMPL

// реализация  mlIVisible
MLIVISIBLE_IMPL

// реализация  mlIButton
MLIBUTTON_IMPL

// реализация mlIText
public:
	mlIText* GetIText(){ return this; }
	const wchar_t* GetValue(); // получить текст
	//mlMedia* GetParent();
	mlStyle* GetStyle(const char* apszTag=NULL);
	bool IsHtml(){ return html; }
	const wchar_t* GetSliceTail(){ return sliceTail==NULL?NULL:(const wchar_t*)JS_GetStringChars(sliceTail);}
	bool IsAdjustHeight();
	mlSize GetFormatSize();
	// установить размер текстового поля
	// (может не совпадать с размером элемента)
	void SetTextSize(unsigned int auWidth, unsigned int auHeight);
	const mlColor GetBkgColor();
	// установить размер текста в количестве строк
	void SetScrollSizeInLines(unsigned int auPageSize, unsigned int auTextSize);

// реализация moIText
public:
	// Свойства
	unsigned int GetScrollTop();
	void SetScrollTop(unsigned int auValue);
	unsigned int GetScrollLeft();
	void SetScrollLeft(unsigned int auValue);
	void ScrollToLine(unsigned int auLine);
	void AddText(wchar_t* str);
	void IncreaseCursor(unsigned int delta);
	void DecreaseCursor(unsigned int delta);
	void GetVisibleBounds(int& _begin, int& _end);
	void SetSelection(int from, int to);
	int GetCursorX();
	int GetCursorY();
	int GetCursorPos();
	int GetCursorHeight();
	void ToClipboard(wchar_t* str);
	std::wstring FromClipboard();
	void SetCursorPos(int pos);
	int GetSymbolIDByXY(int x, int y);
	bool GetSliced();
	// События
	void ValueChanged();
	void StylesChanged();
	void HtmlChanged();
	void AdjustChanged();
	void BkgColorChanged();
	// Прочие
	bool GetTextFormattingInfo(mlSynchData &aData); // для Authoring-а
	bool AreWordBreaksExists();
};

}
