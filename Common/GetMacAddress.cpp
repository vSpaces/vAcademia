#include "stdafx.h"
#include <wtypes.h>
#include <nb30.h>

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff[30];
} ASTAT, * PASTAT;

CComString __GetMacAddress(int AdapterIndex)
{
    ASTAT Adapter;
	
    NCB ncb;
    UCHAR uRetCode;
	
    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBRESET;
    ncb.ncb_lana_num = AdapterIndex;
	
    uRetCode = Netbios(&ncb);
	
    memset(&ncb, 0, sizeof(ncb));
    ncb.ncb_command = NCBASTAT;
    ncb.ncb_lana_num = AdapterIndex;    
	    
    strcpy_s((char *)ncb.ncb_callname, NCBNAMSZ, "*");
	    
	ncb.ncb_buffer = (unsigned char *) &Adapter;
	ncb.ncb_length = sizeof(Adapter);
	
    uRetCode = Netbios(&ncb);
    
    CComString sMacAddress = "001E8CE72150";
	
    if (uRetCode == 0)
    {
    	sMacAddress.Format(_T("%02x%02x%02x%02x%02x%02x"),
    	    Adapter.adapt.adapter_address[0],
            Adapter.adapt.adapter_address[1],
            Adapter.adapt.adapter_address[2],
            Adapter.adapt.adapter_address[3],
            Adapter.adapt.adapter_address[4],
            Adapter.adapt.adapter_address[5]);
    }
    return sMacAddress;
}

CComString GetMacAddress()
{
	NCB Ncb; 
	UCHAR uRetCode; 
	LANA_ENUM lenum; 
	int i = 0; 
	memset(&Ncb, 0, sizeof(Ncb)); 
	Ncb.ncb_command = NCBENUM; 
	Ncb.ncb_buffer = (UCHAR *)&lenum; 
	Ncb.ncb_length = sizeof(lenum); 

	uRetCode = Netbios( &Ncb ); 
	CComString sMacAddress;

	for(i=0; i < lenum.length ;i++)
	{
		sMacAddress = __GetMacAddress(lenum.lana[i]);
		if ( sMacAddress.GetLength() > 0)
			return sMacAddress;
	}
	return "001E8CE72150";
}