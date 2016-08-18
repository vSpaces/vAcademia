#pragma once
#include "nrmImage.h"

class CSharingBoardManager
{
public:
	CSharingBoardManager(oms::omsContext* aContext);
	~CSharingBoardManager(void);

	bool CreateSharingBoard(C3DObject* obj, nrmImage* img, int& textureID, int auID, float aspectX, float aspectY);

private:
	oms::omsContext* context;
};
