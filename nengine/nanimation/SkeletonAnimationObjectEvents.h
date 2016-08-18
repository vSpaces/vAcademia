
#pragma once

typedef struct _SAddIdleEvent
{
	int motionID;
	int between;
	int freq;

	_SAddIdleEvent()
	{
		motionID = -1;
		between = 0;
		freq = 1;
	}
} SAddIdleEvent;

typedef struct _SClearEvent
{
	unsigned int id;
	float time;

	_SClearEvent()
	{
		id = 0xFFFFFFFF;
		time = 1.0f;
	}
} SClearEvent;

typedef struct _SBlendCycleEvent
{
	unsigned int id;
	float weight;
	float delay;
	bool isAsynch;

	_SBlendCycleEvent()
	{
		id = 0xFFFFFFFF;
		weight = 0.0f;
		delay = 1.0f;
		isAsynch = false;
	}
} SBlendCycleEvent;

typedef struct _SExecuteActionEvent
{
	unsigned int id;
	float delayIn;
	float delayOut;

	_SExecuteActionEvent()
	{
		id = 0xFFFFFFFF;
		delayIn = 1.0f;
		delayOut = 1.0f;
	}
} SExecuteActionEvent;

typedef struct _SSetActionEvent
{
	unsigned int id;
	float time;
	
	_SSetActionEvent()
	{
		id = 0xFFFFFFFF;
		time = 1.0f;
	}
} SSetActionEvent;

#define EVENT_ID_ASSIGN_MOTION		1
#define EVENT_ID_ADD_IDLE			2
#define EVENT_ID_REMOVE_IDLES		3
#define EVENT_ID_LOCK_IDLES			4
#define EVENT_ID_UNLOCK_IDLES		5
#define EVENT_ID_CLEAR_ACTION		6
#define EVENT_ID_CLEAR_CYCLE		7
#define EVENT_ID_BLEND_CYCLE		8
#define EVENT_ID_EXECUTE_ACTION		9
#define EVENT_ID_SET_ACTION			10

