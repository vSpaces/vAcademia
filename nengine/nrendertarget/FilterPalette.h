
#pragma once

#include "Manager.h"
#include "Filter.h"

class CFilterPalette : public CManager<CFilter>
{
public:
	CFilterPalette();
	~CFilterPalette();

	void LoadAll();
};