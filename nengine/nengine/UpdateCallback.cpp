
#include "StdAfx.h"
#include <assert.h>
#include "UpdateCallback.h"
#include "cal3d/memory_leak.h"

CUpdateCallback::CUpdateCallback():
	m_callback(NULL)
{
}

void CUpdateCallback::SetCallBack(void(*callbackfunc)())
{
	m_callback = callbackfunc;
}

void CUpdateCallback::Draw()
{
	Update();
}

void CUpdateCallback::Update()
{
	assert(m_callback);

	if (m_callback)
	{
		m_callback();
	}
}

CUpdateCallback::~CUpdateCallback()
{
}