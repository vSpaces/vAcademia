
#include "StdAfx.h"
#include "Commands.h"
#include "CommandQueue.h"

CCommandQueue::CCommandQueue():
	MP_VECTOR_INIT(m_outCmdQueue),
	MP_VECTOR_INIT(m_inCmdQueue)
{
	InitializeCriticalSection(&m_outCs);
	InitializeCriticalSection(&m_inCs);
}

void CCommandQueue::AddInCommand(CDataStorage* inCmdData)
{
	if (inCmdData != NULL)
	{
		EnterCriticalSection(&m_inCs);
		m_inCmdQueue.push_back(inCmdData);
		LeaveCriticalSection(&m_inCs);
	}
}

void CCommandQueue::AddOutCommand(CDataStorage* outCmdData)
{
	if (outCmdData != NULL)
	{
		EnterCriticalSection(&m_outCs);
		
		m_outCmdQueue.push_back(outCmdData);
		
		LeaveCriticalSection(&m_outCs);
	}
}

CDataStorage* CCommandQueue::GetInCommand()
{
	CDataStorage* inCmdData = NULL;

	EnterCriticalSection(&m_inCs);
	if (!m_inCmdQueue.empty())
	{
		inCmdData = m_inCmdQueue.front();
		m_inCmdQueue.erase(m_inCmdQueue.begin(), m_inCmdQueue.begin() + 1);
	}
	LeaveCriticalSection(&m_inCs);

	return inCmdData;
}

CDataStorage* CCommandQueue::GetOutCommand()
{
	CDataStorage* outCmdData = NULL;

	EnterCriticalSection(&m_outCs);

	if (!m_outCmdQueue.empty())
	{
		outCmdData = m_outCmdQueue.front();
		m_outCmdQueue.erase(m_outCmdQueue.begin(), m_outCmdQueue.begin() + 1);
	}
	LeaveCriticalSection(&m_outCs);

	return outCmdData;
}

CCommandQueue::~CCommandQueue()
{
	DeleteCriticalSection(&m_outCs);
	DeleteCriticalSection(&m_inCs);
}