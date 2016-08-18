
#ifndef _OMS_MW_H_
#define _OMS_MW_H_

namespace oms{

  enum CursorType {
    NoCursor,
    NormalCursor,
    ActiveCursor,
    WaitCursor,
    BeamCursor,
    DragCursor,
    Size1Cursor,
    Size2Cursor,
    Size3Cursor,
    Size4Cursor,
	// 
	CreateCursor,
	CopyCursor,
	DeleteCursor,
	Move3DPlaneCursor,
	Move3DPlaneImpossibilityCursor,
	Rotate3DPlaneCursor,
	Move3DCursor,
	Rotate3DCursor,
	Rotate3DImpossibilityCursor,
	Select3DCursor,
	Place3DCursor,
	Place3DImpossibilityCursor,
	Copy3DCursor,
	Scale3DCursor,
	Scale3DImpossibilityCursor,
	SitplaceUpDown3DCursor,
	//
	PenCursor,
	EraseCursorSize2,
	EraseCursorSize4,
	EraseCursorSize6,
	EraseCursorSize8,
	EraseCursorSize10,
	EraseCursorSize12,
	EraseCursorSize16,
	EraseCursorSize20,
	EraseCursorSize26,
	EraseCursorSize32,
	TextStickerCursor1,
	TextStickerCursor2,
	TextStickerCursor3,
	TextStickerCursor4,
	TextStickerCursor5,
	TextStickerCursor6,
	TextStickerCursor7,
	TextStickerCursor8,
	TextStickerCursor9,
	TextStickerCursor10,
	ImageStickerCursor1,
	ImageStickerCursor2,
	ImageStickerCursor3,
	ImageStickerCursor4,
	ImageStickerCursor5,
	ImageStickerCursor6,
	ImageStickerCursor7,
	ImageStickerCursor8,
	ImageStickerCursor9,
	ImageStickerCursor10,
	HealCursor,
	FireOffCursor,
	RemoveBoxCursor,
	StretcherCursor,
	DragPeople,
	TriageCursor
  };
  #define CT_BIG_CURSOR_MASK 0x20

/**
 * Интерфейс окна
 * Используется менеджером сцен для управления курсором
 * и запроса некоторых других параметров окна
 */
struct mwIWindow{
	// управление видом курсора
	// 0 - нет курсора (скрыть)
	// 1 - нормальный курсор
	// 2 - курсор активной зоны
	// 3 - курсор ожидания
	virtual void SetCursor(int aiType)=0;
	// выдать ширину и высоту клиентской области окна
	virtual int GetClientWidth()=0;
	virtual int GetClientHeight()=0;
	// 
	virtual unsigned int GetMainWindowHandle()=0;
	virtual void MouseMoveToPos( short aX, short aY) = 0;
	virtual void SendMouseMessage( unsigned int msg, unsigned int wParam, short aX, short aY) = 0;
};

}

#endif