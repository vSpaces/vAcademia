#pragma once

struct ISoundPtrBase;

class CStopCallback : public RefImplementation<StopCallback>
{	
public:
	CStopCallback( ISoundPtrBase *pSoundPtr);
	virtual ~CStopCallback();

	void destroy();

public:
	ADR_METHOD(void) streamStopped(StopEvent* event);

protected:
	ISoundPtrBase *m_pSoundPtr;
};
