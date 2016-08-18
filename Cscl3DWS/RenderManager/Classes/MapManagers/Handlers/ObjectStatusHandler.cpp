
#include "StdAfx.h"
#include "RMContext.h"
#include "GetObjectStatusIn.h"
#include "ObjectStatusHandler.h"

using namespace MapManagerProtocol;

bool CObjectStatusHandler::Handle(CCommonPacketIn* apQueryIn, cm::cmIMapManCallbacks* mpCallbacks)
{
	ATLASSERT( FALSE);
	CGetObjectStatusIn packetIn((BYTE*)apQueryIn->GetData(), apQueryIn->GetDataSize());
	if (!packetIn.Analyse())
	{
		ATLASSERT( FALSE);
	}
	int iStatus = packetIn.GetStatus();
	// если объект нашелся
	if(iStatus == 1){
		// ?? определить по zoneID, входит ли он в список зон, которые видит клиент
		// если не входит, значит и не загрузится (-2 - объект вне поля видимости или в другой реальности)
		unsigned int uiZoneID = packetIn.GetZoneID();
		if (gRM->mapMan->IsActiveZone(ZoneID(uiZoneID))){
			iStatus = 3; // 3 - объект существует на сервере и находится в поле видимости, но еще не загружен
			// реально может уже и загрузился, но пока выдаем все равно 3, 
			// потому что SceneManager по любому проверит, не появился ли уже объект в сцене
		}else
			iStatus = -2; // -2 - объект вне поля видимости или в другой реальности, поэтому не будет загружен и создан
	}

	/*unsigned int uiUserData = 0;
	int iUserDataSize = 0;
	unsigned int* pUserData = (unsigned int*)apQueryIn->GetUserData(iUserDataSize);
	if(iUserDataSize >= 4){
		uiUserData = *pUserData;
	}
*/
	if (mpCallbacks != NULL)
	{
		ATLASSERT( FALSE);
		//mpCallbacks->onReceivedObjectStatus(packetIn.GetObjectID(), packetIn.GetBornRealityID(), iStatus, (void*)uiUserData);
	}
	return true;
}