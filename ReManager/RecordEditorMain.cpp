// RecordEditor.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "RecordEditor.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

RECORDEDITOR_API unsigned int CreateRecordEditor( oms::omsContext* aContext)
{
	CRecordEditor *pRecordEditor = new CRecordEditor( aContext);
	aContext->mpRecordEditor = pRecordEditor;
	pRecordEditor->Run();
	return OMS_OK;
}

RECORDEDITOR_API unsigned int DestroyRecordEditor( oms::omsContext* aContext)
{
	if (aContext == NULL || aContext->mpRecordEditor == NULL)
		return -1;
	CRecordEditor* pRecordEditor = (CRecordEditor*)aContext->mpRecordEditor;
	aContext->mpRecordEditor = NULL;
	delete pRecordEditor;
	return OMS_OK;
}



