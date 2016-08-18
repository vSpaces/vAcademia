#include "StdAfx.h"
#include "QueryPackageOut.h"

using namespace ResManProtocol;

CQueryPackageOut::CQueryPackageOut(unsigned int queryNum, const BYTE *aData, unsigned int aDataSize)
{
	data.add(queryNum);
	data.addData(aDataSize, (BYTE*)aData);
}