
#include "StdAfx.h"
#include "XMLList.h"
#include "FilterPalette.h"

CFilterPalette::CFilterPalette()
{
	
}

void CFilterPalette::LoadAll()
{
	CXMLList filterList(L"filters.xml", false);

	std::string fileName;

	while (filterList.GetNextValue(fileName))
	{
		int filterID = AddObject(fileName);
		CFilter* filter = GetObjectPointer(filterID);
		filter->Load("filters/" + fileName);
	}
} 

CFilterPalette::~CFilterPalette()
{
	
}