#include "stdafx.h"
#include "LocationQueryOut.h"

using namespace Protocol;

namespace MapManagerProtocol {

CLocationQueryOut::CLocationQueryOut(unsigned int uiPointsCount, CVector3D points[], const wchar_t* apwcLocationName) 
{
/*	data.add(aiRecordID);
	data.add(aiStartTime);*/

	data.add(uiPointsCount);
	for(unsigned int i=0;i<uiPointsCount;i++){
		data.add(points[i].x);
		data.add(points[i].y);
		data.add(points[i].z);
	}
	data.addStringUnicode((unsigned short)wcslen(apwcLocationName), (BYTE*)apwcLocationName);
}

}