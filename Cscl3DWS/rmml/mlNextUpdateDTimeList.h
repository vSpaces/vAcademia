#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _mlNextUpdateDTimeList_H_
#define _mlNextUpdateDTimeList_H_

#include <list>

namespace rmml {

class mlNextUpdateDTimeList: public MP_LIST_DC<int>{
//	 mlNUDTimes;
public:
	mlNextUpdateDTimeList();

	void Reset();
	void InsertDTime(int aiNextUpdateDTime);
	void Update(int aiSubDTime);
	int GetMinimumDTime();
};

}

#endif // _mlNextUpdateDTimeList_H_
