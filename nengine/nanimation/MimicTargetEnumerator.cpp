
#include "StdAfx.h"
#include "MimicTargetEnumerator.h"


#define TARGET_DEFINE_START		void CMimicTargetEnumerator::Init() {
#define TARGET_DEFINE(x, y)			m_targetList.insert(MP_MAP<MP_STRING, int>::value_type(x, y));
#define TARGET_DEFINE_END		}

CMimicTargetEnumerator::CMimicTargetEnumerator():
	MP_MAP_INIT(m_targetList)
{
	Init();
}

CMimicTargetEnumerator* CMimicTargetEnumerator::GetInstance()
{
	static CMimicTargetEnumerator* obj = NULL;

	if (!obj)
	{
		obj = MP_NEW(CMimicTargetEnumerator);
	}

	return obj;
}

int CMimicTargetEnumerator::GetTargetID(const std::string& target)const
{
	std::map<MP_STRING, int>::const_iterator it = m_targetList.find(MAKE_MP_STRING(target));

	if (it != m_targetList.end())
	{
		return (*it).second;
	}
	else
	{
		return -1;
	}
}

TARGET_DEFINE_START
	TARGET_DEFINE(MAKE_MP_STRING("eye_left"), TARGET_EYE_LEFT);
	TARGET_DEFINE(MAKE_MP_STRING("eye_right"), TARGET_EYE_RIGHT);
	TARGET_DEFINE(MAKE_MP_STRING("eye_close"), TARGET_EYE_CLOSE);
	TARGET_DEFINE(MAKE_MP_STRING("smile"), TARGET_SMILE);
	TARGET_DEFINE(MAKE_MP_STRING("smile_start"), TARGET_SMILE_START);
	TARGET_DEFINE(MAKE_MP_STRING("lip_sync1"), TARGET_LIP_SYNC1);
	TARGET_DEFINE(MAKE_MP_STRING("lip_sync2"), TARGET_LIP_SYNC2);
	TARGET_DEFINE(MAKE_MP_STRING("lip_sync3"), TARGET_LIP_SYNC3);
	TARGET_DEFINE(MAKE_MP_STRING("lip_sync4"), TARGET_LIP_SYNC4);
	TARGET_DEFINE(MAKE_MP_STRING("lip_sync5"), TARGET_LIP_SYNC5);
	TARGET_DEFINE(MAKE_MP_STRING("au0"), TARGET_AU0);
	TARGET_DEFINE(MAKE_MP_STRING("au1"), TARGET_AU1);
	TARGET_DEFINE(MAKE_MP_STRING("au2"), TARGET_AU2);
	TARGET_DEFINE(MAKE_MP_STRING("au3"), TARGET_AU3);
	TARGET_DEFINE(MAKE_MP_STRING("au4"), TARGET_AU4);
	TARGET_DEFINE(MAKE_MP_STRING("au5"), TARGET_AU5);
TARGET_DEFINE_END
	
CMimicTargetEnumerator::~CMimicTargetEnumerator()
{
}