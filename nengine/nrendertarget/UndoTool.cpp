
#include "StdAfx.h"
#include "UndoTool.h"
#include "ToolsParameters.h"

CUndoTool::CUndoTool()
{
}

void CUndoTool::SetDrawingTarget(IDrawingTarget* drawingTarget)
{
	m_drawingTarget = drawingTarget;
}

void CUndoTool::SetParameter(int /*paramID*/, void* /*param*/)
{
}

void CUndoTool::OnMouseMove(int /*x*/, int /*y*/)
{
}

void CUndoTool::OnLMouseDown(int /*x*/, int /*y*/)
{
}

void CUndoTool::OnLMouseUp(int /*x*/, int /*y*/)
{
}

void CUndoTool::OnRMouseDown(int /*x*/, int /*y*/)
{
}

void CUndoTool::OnRMouseUp(int /*x*/, int /*y*/)
{
}

void CUndoTool::OnChar(int /*keyCode*/, int /*scancode*/)
{
}

void CUndoTool::RedoAction(void* /*data*/)
{
}

void CUndoTool::OnSelected()
{
}

void CUndoTool::OnDeselected()
{
}

void CUndoTool::OnEndRestoreState()
{
}

int CUndoTool::GetSynchRequiredType()
{
	return SYNCH_MOVE_NONE;
}

void CUndoTool::Destroy()
{
	MP_DELETE_THIS;
}

CUndoTool::~CUndoTool()
{
}
