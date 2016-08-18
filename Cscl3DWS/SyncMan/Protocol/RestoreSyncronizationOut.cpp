#include "stdafx.h"
#include "RestoreSyncronizationOut.h"
#include "SendObjectOut.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace SyncManProtocol{

CRestoreSyncronizationOut::CRestoreSyncronizationOut(){

}

void CRestoreSyncronizationOut::Create( const wchar_t *apwcRequestedUrl, const syncObjectState* mpcAvatarState)
{
	data.clear();

	// Добавим URL телепортации
	int iLen = wcslen(apwcRequestedUrl);
	data.addStringUnicode( iLen, (BYTE*)apwcRequestedUrl);

	if( mpcAvatarState)
	{
		// Добавим последнее состояние
		CSendObjectOut sendObjectOut;
		sendObjectOut.Create( *mpcAvatarState);
		data.add( sendObjectOut.GetDataSize());
		data.addData( sendObjectOut.GetDataSize(), sendObjectOut.GetData());
	}
	else
	{
		data.add( (int)0);
	}
}

}