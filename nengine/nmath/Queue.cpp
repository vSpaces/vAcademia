#include "StdAfx.h"
#include "Queue.h"

bool CQueue::QWrite(SQElement& point)//запись в конец очереди
{
	if  ( (m_tail + 1) == QUEUE_SIZE)
	{
		return false;//очередь полна
	}

	m_queue[m_tail] = point;

	m_tail++;

	return true;
}

SQElement* CQueue::QRead()//выборка из начала
{
	if (m_head == m_tail) // очередь пуста
	{
		return 0; 
	}

	SQElement* ptr = (&m_queue[0]) + m_head;

	m_head++;

	return ptr;
}

int CQueue::GetTailPosition()
{
	return m_tail;
}

SQElement* CQueue::GetElement(int index)
{
	return &m_queue[index];
}