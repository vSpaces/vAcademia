
#pragma once

#include "CommonObject.h"

class CDisplayList : public CCommonObject
{
public:
	CDisplayList(void);
	~CDisplayList(void);

	int GetGLNumber()const;
	void SetGLNumber(int num);

	GLenum GetTarget()const;
	void SetTarget(GLenum target);

	GLenum GetAccess()const;
	void SetAccess(GLenum access);

	GLenum GetUsage()const;
	void SetUsage(GLenum usage);

private:
	int m_num;

	GLenum m_target;
	GLenum m_access;
	GLenum m_usage;
};