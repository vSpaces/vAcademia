// stdafx.cpp : source file that includes just the standard includes
//	RenderManager.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#pragma warning (disable:4530)
#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
// Global variables
#include "NRManager.h"
#include "nrmPreview.h"
#include "../include/clefGuids.h"

//#include "../include/plugins/n3dplugsguids.h"

oms::omsContext*			m_pTracerContext = NULL;

#include <crtdbg.h>

RM_DECL oms::omsresult rm::CreateRenderManager(oms::omsContext* amcx)
{
#ifdef _DEBUG
//_CrtSetBreakAlloc(464);
//_CrtSetBreakAlloc(833);
#endif
	MP_NEW_P(m_pRenderManager, CNRManager, amcx);
	amcx->mpRM = m_pRenderManager;
	nrmPreviewRecorder* recorder = NULL;
	MP_NEW_V(recorder, nrmPreviewRecorder, amcx);
	amcx->mpPreviewRecorder = recorder;
	m_pTracerContext = amcx;
	return OMS_OK;
}

RM_DECL void rm::DestroyRenderManager(oms::omsContext* amcx)
{
	assert(amcx);
	if(amcx->mpRM != NULL)
	{
		MP_DELETE_UNSAFE((CNRManager*)amcx->mpRM);
		amcx->mpRM = NULL;
	}
	if(amcx->mpPreviewRecorder != NULL)
	{
		MP_DELETE_UNSAFE((nrmPreviewRecorder*)amcx->mpPreviewRecorder);
		amcx->mpPreviewRecorder = NULL;
	}
}


void	rm_trace(const char* tx,...)
{
	if( m_pTracerContext&& m_pTracerContext->mpTracer)
	{
		va_list ptr;
		va_start(ptr, tx);
		char sErrorMessage[ 2500] = "RM: ";
		vsprintf( &sErrorMessage[strlen(sErrorMessage)], tx, ptr);
		m_pTracerContext->mpTracer->trace(sErrorMessage);
		va_end(ptr);
	}
}