
#pragma once

#include "ITool.h"
#include "Synch.h"
#include "CommonObject.h"
#include "IDrawingTarget.h"
#include "IMouseListener.h"
#include "IMouseController.h"

#define TOOLS_COUNT				10

class CToolPalette : public IMouseListener, public ISynchPlayer, public CCommonObject
{
public:
	CToolPalette();
	~CToolPalette();

	void SetDrawingTarget(IDrawingTarget* drawingTarget);
	void SetParameter(int paramID, void* param);
	void SetParameter(unsigned int toolID, int paramID, void* param);
	void SetCommonParameter(int paramType,  void* param);
 
	void OnRMouseUp(float x, float y);
	void OnRMouseDown(float x, float y);
	void OnLMouseUp(float x, float y);
	void OnLMouseDown(float x, float y);
	void OnMouseMove(float x, float y);
	void OnChar(int keycode, int scancode);
	void OnKeyUp(int keycode);
	void OnLMouseDblClick(float x, float y);
	void OnRMouseDblClick(float x, float y);

	void OnActivate();

	ITool* GetActiveTool();
	unsigned char GetActiveToolID();
	void SetActiveToolID(unsigned char tool);

	void ClearScreen();
	void OnEndRestore();

	void RedoAction(void* data);
	void RedoAction(unsigned int toolID, void* data);
	
	bool IsNeedMouseEvents();
	bool IsToolsUsed();

	CSynch* GetSynch();

	void UndoAction();
	void PasteText(bool isPasteOnIconClick = false);

	void SetMouseController(IMouseController* ctrl);

	void SetIPadDrawing( bool am_bIPadDrawing);
	bool GetIPadDrawing();

private:
	unsigned short GetTextureX(float x);
	unsigned short GetTextureY(float y);

	unsigned short GetScaledTextureX(float x);
	unsigned short GetScaledTextureY(float y);

	CSynch* m_synch;

	IDrawingTarget* m_drawingTarget;

	bool m_isLMouseDown;
	bool m_isRMouseDown;
	bool m_isToolUsed;

	unsigned char m_mode;
	unsigned char m_activeToolID;
	ITool* m_tools[TOOLS_COUNT];

	__int64 m_lastPressButtonTime;

	IMouseController* m_ctrl;
	bool m_bIPadDrawing;
};