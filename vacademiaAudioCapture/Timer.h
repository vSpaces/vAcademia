#ifndef _TIMER_H
#define _TIMER_H

// All timer resolutions are in microseconds.

struct Timer {
protected:
	__int64 uiStart;
	public:
	static __int64 now();

	Timer();
	bool isElapsed(__int64 us);
	__int64 elapsed() const;
	__int64 restart();
};

#endif
