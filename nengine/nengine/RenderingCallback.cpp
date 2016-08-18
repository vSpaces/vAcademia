
#include "StdAfx.h"
#include <assert.h>
#include "RenderingCallback.h"
#include "cal3d/memory_leak.h"

CRenderingCallback::CRenderingCallback():
	m_callback(NULL)
{
}

void CRenderingCallback::SetCallBack(void(*callbackfunc)())
{
	m_callback = callbackfunc;
}

void CRenderingCallback::Draw()
{
	assert(m_callback);

	if (m_callback)
	{
		m_callback();
	}
}

CRenderingCallback::~CRenderingCallback()
{
}