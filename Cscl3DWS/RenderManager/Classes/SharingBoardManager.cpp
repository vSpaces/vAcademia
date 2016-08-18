#include "stdafx.h"
#include ".\sharingboardmanager.h"
#include "sharingmousecontroller.h"
#include "UserData.h"
#include "RMContext.h"

CSharingBoardManager::CSharingBoardManager(oms::omsContext* aContext)
{
	context = aContext;
	gRM->SetSharingBoardManager(this);
}

bool CSharingBoardManager::CreateSharingBoard(C3DObject* obj, nrmImage* img, int& /*textureID*/, int /*auID*/, float aspectX, float aspectY)
{
	USES_CONVERSION;
	MP_NEW_P4(contrl, CSharingMouseController, context, aspectX, aspectY, W2A(obj->GetName()));
	img->SetSharingController(contrl);
	obj->SetUserData(USER_DATA_SHARING_TARGET, contrl);
	return true;
}

CSharingBoardManager::~CSharingBoardManager()
{
}
