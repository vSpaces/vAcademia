#include "StdAfx.h"
#include ".\vasebmp.h"

CVASEBmp::CVASEBmp(void)
{
	m_pDIB = NULL;
	m_width = 0;
	m_height = 0;
	m_bpp = 0;
	isMemAllocate = false;
}
CVASEBmp::CVASEBmp(void * pDIB, const int & width, const int & height, const int & bpp)
{
	m_pDIB = pDIB;
	m_width = width;
	m_height = height;
	m_bpp = bpp;
	isMemAllocate = false;
}

CVASEBmp::~CVASEBmp(void)
{
	if (isMemAllocate && m_pDIB != NULL)
		SAFE_DELETE_ARRAY(m_pDIB);
}


