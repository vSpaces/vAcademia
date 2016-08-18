
#pragma once

class CTimeProcessor
{
friend class CGlobalSingletonStorage;

public:
	CTimeProcessor();
	~CTimeProcessor();

	// initialize Time Processor
	void Init();
	// start timing
	void Start();

	// get current time (in ms)
	__int64 GetTickCount();

	// get current time (in microseconds)
	__int64 GetMicroTickCount();
	
private:
	CTimeProcessor(const CTimeProcessor&);
	CTimeProcessor& operator=(const CTimeProcessor&);

	__int64 GetTime();

	__int64 m_startTime;
	__int64 m_freq;
};
