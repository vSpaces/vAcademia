
#pragma once

#include "Manager.h"
#include "FilterSequence.h"

class CFilterSequencePalette : public CManager<CFilterSequence>
{
public:
	CFilterSequencePalette();
	~CFilterSequencePalette();

	void LoadAll();
};