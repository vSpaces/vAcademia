
#pragma once

#include "CommonHeader.h"

class CMimicTargetEnumerator
{
public:
	CMimicTargetEnumerator();
	~CMimicTargetEnumerator();

	static CMimicTargetEnumerator* GetInstance();

	int GetTargetID(const std::string& target)const;

private:
	void Init();

	MP_MAP<MP_STRING, int> m_targetList;
};