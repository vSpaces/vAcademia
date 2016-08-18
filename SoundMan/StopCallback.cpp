#include "StdAfx.h"
#include "StopCallBack.h"

CStopCallback::CStopCallback( ISoundPtrBase *pSoundPtr)
{
	ATLASSERT( pSoundPtr);
	if ( pSoundPtr)
		m_pSoundPtr = pSoundPtr;
	else
		m_pSoundPtr = NULL;
}

CStopCallback::~CStopCallback()
{
	m_pSoundPtr = NULL;
}

void CStopCallback::destroy()
{
	MP_DELETE_THIS;
}

void ADR_CALL CStopCallback::streamStopped( StopEvent* event) 
{
	OutputStream *stream = event->getOutputStream();
	StopEvent::Reason reason = event->getReason();	
	if ( m_pSoundPtr)
		m_pSoundPtr->on_stopped( reason);	
}
