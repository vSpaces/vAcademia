#pragma once

class IWorldEditor
{
public:

	virtual bool GetConnectionState()=0;
	virtual void SelectClickedObject()=0;
	virtual void OnChar(int keycode) = 0;
	virtual void SetMButtonPress(bool enable, int x = -1, int y = -1)=0;

	virtual void SetLButtonDown(int x, int y) = 0;
	virtual void SetLButtonUp(int x, int y) = 0;
	virtual void SetLMouseDblClick(int x, int y) =0;

	virtual bool IsMButtonPressed() = 0;
	virtual bool IsLButtonPressed() = 0;
	virtual bool IsDragObjectByMouse() = 0;
	virtual void MouseMove(int x, int y) = 0;
	virtual bool IsFreeCam() = 0;
	virtual void StopDragObjectByMouse() = 0;
};
