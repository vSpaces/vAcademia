
#if !defined(AFX_NRMIMAGE_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
#define AFX_NRMIMAGE_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_

#include "../CommonRenderManagerHeader.h"

#include <string>
#include "nrmanager.h"
#include "nrmObject.h"
#include "nrmMOIVisibleStub.h"
#include "Color4.h"
#include "ToolPalette.h"
#include "ToolsParameters.h"
#include "IChangesListener.h"
#include "../../../DesktopMan/include/IDesktopMan.h"
#include "sharingmousecontroller.h"
#include "OcclusionQuery.h"

#include "Sprite.h"
//#include "DataBuffer.h"
#include "Mutex.h"

class IPadBinState 
{
public:
	unsigned char mode;
	//CDataBuffer data;
	byte *data;
	unsigned int size;
	IPadBinState( unsigned char aMode, byte *aData, unsigned int aSize)
	{
		mode = aMode;
		size = aSize;
		data = NULL;
		if( aSize > 0)
		{
			data = MP_NEW_ARR( byte, size + 1);
			rtl_memcpy( data, size, aData, aSize);
		}
	}

	~IPadBinState()
	{
		if( data != NULL)
			MP_DELETE_ARR( data);
	}
};

class nrmImage :	public nrmMOIVisibleStub,
					public moIButton,
					public moIImage,
					public moIBinSynch,
					public nrmObject,
					public IChangesListener,
					public IController,
					public desktop::IDesktopSharingDestination
{
public:
	nrmImage(mlMedia* apMLMedia);
	virtual ~nrmImage();

// реализация moMedia
public:
	moILoadable* GetILoadable(){ return this; }
	moIVisible* GetIVisible(){ return this; }
	moIButton* GetIButton(){ return this; }
	moIImage* GetIImage(){ return this; }
	
public:
	void OnChanged(int eventID);
	int GetBinarySize();

// реализация  moILoadable
public:
	bool SrcChanged(); 
	bool Reload();
	void LoadingPriorityChanged( ELoadingPriority aPriority);

	void CreateSpriteIfNeeded(std::wstring& src);

// реализация  IDesktopSharingDestination
public:
	bool SetDynamicTexture( IDynamicTexture* apIDynamicTexture);
	bool DynamicTextureDeleting( IDynamicTexture* apIDynamicTexture);
	bool ResizeImage(); 
	void ResetResize();

// реализация  moIVisible
public:
	// Свойства
	void AbsXYChanged(); // изменилось абсолютное положение 
	void SizeChanged();
	void AbsDepthChanged(); // изменилась абсолютная глубина
	void AbsVisibleChanged(); // изменилась абсолютная видимость
	void AbsOpacityChanged(); // изменилась абсолютная видимость
	mlSize GetMediaSize(); 
	void Crop(short awX, short awY, MLWORD awWidth, MLWORD awHeight);

	mlRect GetCropRect();
	void SetMask(int aiX, int aiY, int aiW, int aiH);
	int IsPointIn(const mlPoint aPnt);

	void Save();

// реализация  moIButton
public:
	void onKeyDown(){}
	void onKeyUp(){}

	void onSetFocus(){}
	void onKillFocus(){}
	void AbsEnabledChanged();

public:
	void SetSharingController(CSharingMouseController* contrl);
	void Update(unsigned int deltaTime);
	moIBinSynch* GetIBinSynch();
	void SetPalette(CToolPalette* palette);
	bool SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize);
	bool SetFullState(unsigned int auStateVersion, const unsigned char* apState, int aiSize, bool anIpadState=false);
	bool UpdateState(unsigned int auStateVersion, BSSCCode aCode, const unsigned char* apData, int aulSize, 
		unsigned long aulOffset=0, unsigned long aulBlockSize=0);
	unsigned long GetFullState(unsigned int& auStateVersion, const unsigned char* apState, int aiSize);
	void Reset();

private:
	void IncreaseVersion();

// реализация moIImage
public:
	void EditableChanged();
	void EditableNowChanged();
	void ColorChanged();
	void SmoothChanged();
	void TileChanged();
	void CropChanged();
	void CompressedChanged();
	void AngleChanged();
	void MaxDecreaseKoefChanged();
	void SysmemChanged();
	bool MakeSnapshot(mlMedia** appVisibles, const mlRect aCropRect);
	void DrawLine(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color, int aiWeight);
	void FillRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor color);
	// setCanvasSize(w,h) - устанавливает размер текстуры 
	void SetCanvasSize(int aiWidth, int aiHeight);
	// рисует эллипс на canvas-е
	void DrawEllipse(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor aColor, int aiWeight, const mlColor aFillColor);
	//рисует прямоугольник
	void DrawRectangle(int aiX1, int aiY1, int aiX2, int aiY2, const mlColor aColor, int aiWeight, const mlColor aFillColor);
	// получить цвет пиксела
	mlColor GetPixel(int aiX, int aiY);
	// сделать себе копию изображения
	void DuplicateFrom(mlMedia* apSourceImage);
	void SetActiveTool(int aToolNumber);
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
	void SetLineDirection(int aDirection);
	void Resample(int aiWidth, int aiHeight);
	void ClearOldScreen();
	void UndoAction();
	void PasteText(bool isPasteOnIconClick = false);
	bool isPaintToolUsed();
	bool isFirstSharingFrameDrawed();

	void CopyToClipboard();	

	bool ChangeState( unsigned char type, unsigned char *aData, unsigned int aDataSize);
	bool GetBinState( BYTE*& aBinState, int& aBinStateSize);
	bool SetIPadDrawing( bool abIPadDrawing);
	bool ApplyIPadState();

	bool IsBinaryStateReceived();

// nrmObject
public:
	virtual	BOOL	SaveMedia(const wchar_t* apwcPath);
	int				GetTextureID();

	void* GetMediaPointer();
	void SetToolPalette(CToolPalette* aPalette);

	CSprite*		m_sprite;
	CToolPalette*	m_palette;
	bool			m_editable;
	bool			m_editableNow;
	__int64			m_lastUpdateTime;
	unsigned int	m_version;
	CSharingMouseController* m_sharingController;	

protected:
	virtual void	UpdateMask( nrmImage* apMask);

private:
	bool IsLoaded();
	void SetLoadingPriority(unsigned char aPriority);
	void OnAfterLoad();

	bool	m_isFirstLoaded;
	bool	m_isNeedToClear;
	bool	m_isOnLoadSended;
	bool	m_isBinaryStateReceived;
	unsigned char m_loadingPriority;

	MP_WSTRING m_src;	
	int m_textureWidthBeforeResize;
	int m_textureHeightBeforeResize;
	CMutex mutexIPad;
	MP_VECTOR<IPadBinState*> v_IPadImage;

};

#endif // !defined(AFX_NRMIMAGE_H__A94D4F09_D0E9_480F_A889_757EF331EA0A__INCLUDED_)
