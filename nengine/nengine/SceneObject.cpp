
#include "StdAfx.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "cal3d/memory_leak.h"

ISceneObject::ISceneObject()
{
	m_priority = 100;
	m_renderMode = MODE_RENDER_SOLID;
}

void ISceneObject::SetRenderPriority(int priority)
{
	m_priority = priority;
}

int	ISceneObject::GetRenderPriority()const
{
	return m_priority;
}

void ISceneObject::SetRenderMode(int mode)
{
	m_renderMode = mode;
}

int ISceneObject::GetRenderMode()const
{
	return m_renderMode;
}

ISceneObject::~ISceneObject()
{
	
}