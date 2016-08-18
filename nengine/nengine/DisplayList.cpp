
#include "StdAfx.h"
#include <Assert.h>
#include "DisplayList.h"
#include "cal3d/memory_leak.h"

CDisplayList::CDisplayList(void):
	m_num(-1)
{
}

CDisplayList::~CDisplayList(void)
{
}

int CDisplayList::GetGLNumber()const
{
	assert(m_num != -1);

	return m_num;
}

void CDisplayList::SetGLNumber(int num)
{
	m_num = num;
}

GLenum CDisplayList::GetTarget()const
{
	return m_target;
}

void CDisplayList::SetTarget(GLenum target)
{
	m_target = target;
}

GLenum CDisplayList::GetAccess()const
{
	return m_access;
}

void CDisplayList::SetAccess(GLenum access)
{
	m_access = access;
}

GLenum CDisplayList::GetUsage()const
{
	return m_usage;
}

void CDisplayList::SetUsage(GLenum usage)
{
	m_usage = usage;
}