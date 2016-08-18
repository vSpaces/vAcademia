#pragma once

#include "2D\mlRectangleJSO.h"
#include "mlColorJSO.h"

namespace rmml {

/**
 *  ласс изображени€ RMML
 */

class mlRMMLImage :	
					public mlRMMLElement,
					public mlJSClass,
					public mlRMMLLoadable,
					public mlRMMLVisible,
					public mlRMMLPButton,
					public mlIColorCL,
					public mlIImage,
					public moIImage
{
	Filter antialiasing;
protected:
	bool mbEditable;
	bool mbEditableNow;
	bool mbRemoteControlNow;
	mlColor color;
	JSObject* mjsoColor;
	bool tile;
	bool smooth;

	mlBounds* crop;
	bool compressed;
	bool sysmem;
	int angle;
	unsigned char maxDecreaseKoef;

public:
	bool GetOcclusionQueryResult();
	mlRMMLImage(void);
	void destroy() { MP_DELETE_THIS }
	~mlRMMLImage(void);
MLJSCLASS_DECL
private:
	enum{
		JSPROP_editable = 1,
		JSPROP_editableNow,
		JSPROP_color,	// цвет, примешиваемый к изображению
		JSPROP_tile,	// множить картинку, а не раст€гивать
		JSPROP_crop,		// декларативное задание crop-а в формате "x, y, w, h" или "x y w h"
		JSPROP_smooth,

		JSPROP_compressed,	// изображение может хранитс€ в видеопам€ти в сжатом виде (со снижением качества)
		JSPROP_angle, 
		JSPROP_maxDecreaseKoef,
		JSPROP_sysmem,	// если картинка невидима€, то не распаковывать в видео пам€ть, а оставить в сжатом виде в системной пам€ти
		JSPROP_remoteControlNow,
		JSPROP_editToolUsed,
		JSPROP_firstSharingFrameDrawed
	};
	JSFUNC_DECL(makeSnapshot)
	JSFUNC_DECL(drawLine)
	JSFUNC_DECL(drawEllipse)
	JSFUNC_DECL(drawRectangle)
	JSFUNC_DECL(fillRectangle)
	JSFUNC_DECL(setCanvasSize)
	JSFUNC_DECL(getPixel)
	JSFUNC_DECL(setActiveTool)
	JSFUNC_DECL(setLineWidth)
	JSFUNC_DECL(setLineColor)
	JSFUNC_DECL(setBgColor)
	JSFUNC_DECL(setToolOpacity)
	JSFUNC_DECL(setTextFontName)
	JSFUNC_DECL(setTextBold)
	JSFUNC_DECL(setTextItalic)
	JSFUNC_DECL(setTextUnderline)
	JSFUNC_DECL(setTextAntialiasing)
	JSFUNC_DECL(setToolMode)
	JSFUNC_DECL(setLineDirection)
	JSFUNC_DECL(resample)
	JSFUNC_DECL(undoAction)
	JSFUNC_DECL(pasteText)
	JSFUNC_DECL(clearOldScreen)
	JSFUNC_DECL(copyToClipboard)
	JSFUNC_DECL(isBinaryStateReceived)
	JSFUNC_DECL(save)

	void CreateColorJSO(JSObject* &jso, mlColor* apColor);

// реализаци€ mlRMMLElement
MLRMMLELEMENT_IMPL
	mlresult CreateMedia(omsContext* amcx);
	mlresult Load();
	mlRMMLElement* Duplicate(mlRMMLElement* apNewParent);
	void DuplicateMedia(mlRMMLElement* apSource);

// реализаци€  mlILoadable
MLILOADABLE_IMPL

// реализаци€ mlIColorCL
public:
	void ColorChanged(mlColor* apColor){
		ColorChanged();
	}

// реализаци€  mlIVisible
MLIVISIBLE_IMPL

// реализаци€  mlIButton
MLIBUTTON_IMPL

// реализаци€ mlIImage
public:
	mlIImage* GetIImage(){ return this; }
	Filter GetAntialiasing(){ return antialiasing; }
	bool GetEditable(){ return this->mbEditable; }
	bool GetEditableNow(){ return this->mbEditableNow; }
	bool GetRemoteControlNow(){return this->mbRemoteControlNow; }
	mlColor GetColor(){ return color; }
	bool GetCompressed(){ return compressed; }
	int GetAngle(){ return angle; }
	unsigned char GetMaxDecreaseKoef(){ return maxDecreaseKoef; }
	bool GetSysmem(){ return sysmem; }
	bool GetTile(){ return tile; }
	bool GetSmooth(){ return smooth; }

	mlBounds GetCrop(){
		if(crop == NULL){
			mlBounds bnd; ML_INIT_BOUNDS(bnd);
			return bnd;
		}else
			return *crop;
	}

// реализаци€ moIImage
public:
	void EditableChanged();
	void EditableNowChanged();
	void ColorChanged();
	void TileChanged();
	void SmoothChanged();

	void CropChanged();
	void CompressedChanged();
	void AngleChanged();
	void MaxDecreaseKoefChanged();
	void SysmemChanged();

	bool MakeSnapshot(mlMedia** appVisibles, const mlRect aCropRect){
		if(PMO_IS_NULL(mpMO)) return false;
		 return mpMO->GetIImage()->MakeSnapshot(appVisibles, aCropRect);
	}
	void DrawLine(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight){
		if(PMO_IS_NULL(mpMO)) return;
		mpMO->GetIImage()->DrawLine(aiX1, aiY1, aiX2, aiY2, color, aiWeight);
	}
	void FillRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color){
		if(PMO_IS_NULL(mpMO)) return;
		mpMO->GetIImage()->FillRectangle(aiX1, aiY1, aiX2, aiY2, color);
	}
	void SetCanvasSize(int aiWidth, int aiHeight){
		if(PMO_IS_NULL(mpMO)) return;
		mpMO->GetIImage()->SetCanvasSize(aiWidth, aiHeight);
	}
	void DrawEllipse(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight, const mlColor aFillColor){
		if(PMO_IS_NULL(mpMO)) return;
		mpMO->GetIImage()->DrawEllipse(aiX1, aiY1, aiX2, aiY2, color, aiWeight, aFillColor);
	}
	
	void DrawRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight, const mlColor aFillColor){
		if(PMO_IS_NULL(mpMO)) return;
		mpMO->GetIImage()->DrawRectangle(aiX1, aiY1, aiX2, aiY2, color, aiWeight, aFillColor);
	}

	mlColor GetPixel(int aiX, int aiY){
		if(PMO_IS_NULL(mpMO)) return mlColor();
		return mpMO->GetIImage()->GetPixel(aiX, aiY);
	}
	// сделать себе копию изображени€
	void DuplicateFrom(mlMedia* apSourceImage){
		if(PMO_IS_NULL(mpMO)) return;
		return mpMO->GetIImage()->DuplicateFrom(apSourceImage);
	}

	void SetActiveTool(int aToolNumber);
	void UndoAction();
	void PasteText(bool isPasteOnIconClick = false);
	void SetLineWidth(int aLineWidth);
	void SetLineColor(const mlColor aColor);
	void SetBgColor(const mlColor aColor);
	void SetToolOpacity(int aOpacity);
	void SetTextFontName(wchar_t* aFontName);
	void SetTextBold(bool isBold);
	void SetTextItalic(bool isItalic);
	void SetTextUnderline(bool isUnderline);
	void SetTextAntialiasing(bool isAntialiasing);
	void SetToolMode(int aMode);
	void SetLineDirection(int aDirection);//1 - up, 2 - down, 3 - no direction
	void Resample(int aiWidth, int aiHeight);
	void ClearOldScreen();
	void CopyToClipboard();
	void Save();
	bool isPaintToolUsed();
	bool isFirstSharingFrameDrawed();
	bool ChangeState( unsigned char type, unsigned char *aData, unsigned int aDataSize);
	bool GetBinState( BYTE*& aBinState, int& aBinStateSize);
	bool SetIPadDrawing( bool abIPadDrawing);
	bool ApplyIPadState();
	bool IsBinaryStateReceived();
};

}