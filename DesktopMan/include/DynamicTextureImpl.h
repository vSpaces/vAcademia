#pragma once

#include "../../nengine/interfaces/idynamictexture.h"
#include "TextureLoader.h"
#include "SharingContext.h"
#include "../include/view/RemoteDesktopWindow.h"

class VASEFramesDifPack;

typedef struct _VASE_FRAME
{
	VASEFramesDifPack*	pack;
	bool				isKeyframe;
} VASE_FRAME;

//////////////////////////////////////////////////////////////////////////
class DESKTOPMAN_API CDynamicTextureImpl : public IDynamicTexture
{
public:
	CDynamicTextureImpl(SHARING_CONTEXT* apContext);
	virtual ~CDynamicTextureImpl();

public:
	// ¬озвращает флаг, что его не удалили
	bool	IsLive();
	// ƒобавл€ет новый кадр
	void	AddVASEPack(VASEFramesDifPack* aPack, bool abKeyFrame);
	// ”станавливает режим работы с отрисовкой
	void	EnableNormalMode();

	
	void UseNewCodec()
	{
		useNewCodec = true;
	}
	void NotUseNewCodec()
	{
		useNewCodec = false;
	}
	void SetViewWindow(CRemoteDesktopWindow* window);
	void DesktopManDestroy();
	void SetTarget( LPCSTR alpcName);
	std::string GetTextureInfo();
	void ReturnRealSize();
	void SetTextureSizeBeforeResize(int width, int height);
	void ShowLastFrame();
public:
	// ќбновить состо€ние объекта (не текстуру);
	virtual void Update();
	// ќбновить текстуру
	virtual void UpdateFrame();

	// ¬ернуть ссылку на массив с построчным содержимым кадра	
	virtual void* GetFrameData();

	// ¬ернуть ширину исходной картинки в пиксел€х
	virtual unsigned short GetSourceWidth();
	// ¬ернуть высоту исходной картинки в пиксел€х
	virtual unsigned short GetSourceHeight();
	// ¬ернуть ширину результирующей текстуры в пиксел€х
	virtual unsigned short GetTextureWidth();
	// ¬ернуть высоту результирующей текстуры в пиксел€х
	virtual unsigned short GetTextureHeight();
	// ”становить размер результирующей текстуры
	virtual void SetTextureSize(int width, int height);
	// ¬ернуть количество цветовых каналов, 3 дл€ RGB, 4 дл€ RGBA
	virtual unsigned char GetColorChannelCount();
	// ѕор€док цветовых каналов пр€мой (RGB); или обратный (BGR);
	virtual bool IsDirectChannelOrder();

	// Ќужно ли обновл€ть текстуру (или сейчас еЄ содержимое не измен€етс€?);
	virtual bool IsNeedUpdate();

	// ”ничтож себ€
	// (требуетс€ дл€ того, чтобы nengine мог сам решить когда его безопасно удалить);
	virtual void Destroy();

	void* 	GetUserData(unsigned int userDataID);
	void 	SetUserData(unsigned int userDataID, void* data);

	void SetFreezState(bool isFreezed);
	bool IsFreezed();
	bool IsFirstFrameSetted();

private:
	void	MakeWhiteBackBufferBitmap();
	void	OnNewKeyframeReceived(VASEFramesDifPack* aPack);
	void	SetKeyFrameSize(CSize aSize);
	void	CleanUpFrames();
	HBITMAP CreateDIBSection( unsigned int auWidth, unsigned int auHeight
								, unsigned int auBPP, void** lppBits
								, BITMAPINFO &bmpInfo, bool abReverseHMode = true);
	void	CopyFrameToBackbuffer( unsigned int auWidth, unsigned int auHeight
									, void* lpData, unsigned int auSize
									, CPoint position, bool abReverseHMode);
	void	DrawStatistics( CDC& cdc);
	bool	DestroyFromDesktopMan();
	bool	DestroyFromNengine();
	bool	DestroyFromUnpacing();
	bool	CanDelete();
	void	Release();
	void	Clear();

private:
	SHARING_CONTEXT* context;
	CRemoteDesktopWindow* viewWindow;
	bool useNewCodec;
	typedef MP_VECTOR<VASE_FRAME>	VecFrames;
	typedef VecFrames::iterator VecFramesIt;
	VecFrames	frames;
	bool	isChanged;
	bool	isDestroyed;
	bool	isOneKeyReceived;
	bool	isDestroyedInDesktopMan;
	bool	isDestroyedInEngine;
	bool	m_isFreezed;

	// last keyframe parameters
	CSize			keyFrameSize;
	unsigned int	keyFrameBPP;

	// Texture Bitmap params
	CDC				backBufferDC;
	CBitmap			backBufferBitmap;
	CSize			backBufferSize;
	CSize			textureRequestSize;
	void*			backBufferBits;
	unsigned int	backBufferStride;
	// содержит изображение в исходных размерах
	CDC				backBackBufferDC;
	CBitmap			backBackBitmap;
	void*			backBackBits;

	// Statistics
		// общее количество полученных байт по сессии
	unsigned int	totalBytesReceived;
		// количество полученных на последнем кадре байт
	unsigned int	lastFrameBytesReceived;
		// fps обновлени€ текстуры
	float			updateFPS;
		// fps получени€ сетевых пакетов
	float			receivedFPS;

	CTextureLoader	textureLoader;
	CCriticalSection csFrames;
	CCriticalSection csDestroy;

	bool			normalModeEnabled;
	// isUnpackingNow - true - поток распаковки начал работу, текстуру удалим после его отработки в UpdateFrame
	bool			isUnpackingNow;
	bool			m_deleteResultGiven;
	bool			m_lostDestination;

	void*			m_userData[4];
	CComString		destination;

	long			m_countRecvFrames;
	long			m_countDrawFrames;
	long			m_timeFromLastRedraw;
	long			m_timeFromLastRecvFrame;


	int				m_textureWidthBeforeResize;
	int				m_textureHeigthBeforeResize;

	bool			m_isNeedToSetKeyFrameSize;

	long			m_totalSizeFrame;

};