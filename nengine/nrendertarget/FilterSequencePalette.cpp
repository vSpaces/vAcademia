#include "StdAfx.h"
#include "XMLList.h"
#include "FilterSequencePalette.h"

CFilterSequencePalette::CFilterSequencePalette()
{
	
}

void CFilterSequencePalette::LoadAll()
{
	CXMLList filterSeqList(L"sequences.xml", false);

	std::string fileName;

	while (filterSeqList.GetNextValue(fileName))
	{
		int filterID = AddObject(fileName);
		CFilterSequence* filterSeq = GetObjectPointer(filterID);
		filterSeq->Load("sequences/" + fileName, false);
	}
} 

CFilterSequencePalette::~CFilterSequencePalette()
{
	
}