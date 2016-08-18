
#pragma once

class CCal3DModelAccessObject
{
public:
	CCal3DModelAccessObject(CSkeletonAnimationObject* sao);
	~CCal3DModelAccessObject();	

private:
	CSkeletonAnimationObject* m_sao;
};