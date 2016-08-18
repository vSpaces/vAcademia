#include "mlRMML.h" // for precompile heades only
#include "config/oms_config.h"
#include "mlNextUpdateDTimeList.h"
#include "config/prolog.h"

namespace rmml {

mlNextUpdateDTimeList::mlNextUpdateDTimeList()
{
	MP_LIST_INIT_THIS;
}

void mlNextUpdateDTimeList::Reset(){
	erase(begin(), end());
}

void mlNextUpdateDTimeList::InsertDTime(int aiNextUpdateDTime){
	std::list<int>::iterator it;
	for(it = begin(); it != end(); ++it){
		if(aiNextUpdateDTime == *it)
			return;
		if(aiNextUpdateDTime < *it)
			break;
	}
	insert(it,aiNextUpdateDTime);
}

void mlNextUpdateDTimeList::Update(int aiSubDTime){
	std::list<int>::iterator it;
	for(it = begin(); it != end(); ++it){
		*it -= aiSubDTime;
int iDT = *it;
		if(*it <= 0) 
			*it = 0;
	}
}

int mlNextUpdateDTimeList::GetMinimumDTime(){
	if(size() == 0) return -1;
	return front();
}

}
