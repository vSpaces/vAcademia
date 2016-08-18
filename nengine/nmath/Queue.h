#include "Vector3D.h"

#define QUEUE_SIZE 5000

typedef struct _SSegment
{
	CVector3D begin;
	CVector3D end;

	float length;

	int groupId;

	unsigned char typeOfSegment;

	bool active;

	_SSegment (float x1, float y1, float z1 ,float x2, float y2, float z2,int _groupId, unsigned char _typeOfSegment, bool _active = true)
	{
		begin.x = x1;
		begin.y = y1;
		begin.z = z1;

		end.x = x2;
		end.y = y2;
		end.z = z2;

		length = sqrtf((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2- y1));

		groupId = _groupId; 

		typeOfSegment = _typeOfSegment;

		active = _active;
	}

} SSegment;

typedef struct _SQElement
{
	float length;

	_SQElement* prevElement;
	CVector3D curPoint;

	bool isBeginPoint;

	SSegment* segment;

	_SQElement(){};

	_SQElement (CVector3D& _curPoint, _SQElement* _prevElement, float _length, SSegment* _segment, bool _isBeginPoint = false)
	{
		length = _length;
		curPoint = _curPoint;
		prevElement = _prevElement;	
		isBeginPoint = _isBeginPoint;
		segment = _segment;
	}

	void SetPrevElement(_SQElement * _prevElement)
	{
		prevElement = _prevElement;
	}


} SQElement;

class CQueue{
public:

	__forceinline CQueue()
	{
		m_head = m_tail = 0;
	}

	__forceinline void Clear()
	{
		m_head = m_tail = 0;
	}

	bool QWrite(SQElement& point);
	SQElement* QRead();

	SQElement* GetElement(int index);

	int GetTailPosition();

protected:
	SQElement m_queue[QUEUE_SIZE]; 

	int m_head, m_tail; // индексы вершины и хвоста
};
