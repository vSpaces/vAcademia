
#pragma once

#include "ITool.h"

class ISynchPlayer
{
public:
	virtual void SetParameter(int paramID, void* param) = 0;
	virtual void SetActiveToolID(unsigned char tool) = 0;
	virtual ITool* GetActiveTool() = 0;
	virtual unsigned char GetActiveToolID() = 0;
	virtual void ClearScreen() = 0;
	virtual void OnEndRestore() = 0;
	virtual void SetIPadDrawing( bool abIPadDrawing) = 0;
	virtual bool GetIPadDrawing() = 0;
};