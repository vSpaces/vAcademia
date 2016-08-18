#pragma once

#include "../CommonRenderManagerHeader.h"

#include "IMouseListener.h"
#include "IDrawingTarget.h"
#include "IDynamicTexture.h"
#include "../../DesktopMan/include/IDesktopMan.h"

class CSharingMouseController : public IMouseListener
{
public:
	CSharingMouseController(oms::omsContext* aContext, float aspectX, float aspectY, char* target);
	~CSharingMouseController(void);

	void OnLMouseDown(float x, float y);
	void OnLMouseUp(float x, float y);
	void OnRMouseDown(float x, float y);
	void OnRMouseUp(float x, float y);
	void OnMouseMove(float x, float y);
	void OnChar(int key, int scancode);
	void OnKeyUp(int key);
	void OnLMouseDblClick(float x, float y);
	void OnRMouseDblClick(float x, float y);

	bool IsNeedMouseEvents();
	void SetNeedEvent(bool needEvent);
	unsigned int GetTextureX(float x);
	unsigned int GetTextureY(float y);

	void SetDrawingTarget(IDrawingTarget* drawingTarget);

	void SetDinamicTexture(IDynamicTexture* apIDynamicTexture);
	void UpdateActiveTarget();

	void SetMouseController(IMouseController* ctrl);

private:
	desktop::IMouseKeyboardHandler* GetIMouseKeyboardHandler();
	bool CheckArea(float x, float y);
	oms::omsContext* context;
	IDrawingTarget* m_drawingTarget;
	bool m_needEvents;
	IDynamicTexture* m_dynamictexture;
	float m_aspectX, m_aspectY;
	MP_STRING alpTarget;

	IMouseController* m_ctrl;

	int m_updateCnt;
};
