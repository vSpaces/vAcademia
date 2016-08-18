#include "Timer.h"
#include <windows.h>

Timer::Timer() {
	uiStart = now();
}

__int64 Timer::elapsed() const {
	return now() - uiStart;
}

bool Timer::isElapsed(__int64 us) {
	if (elapsed() > us) {
		uiStart += us;
		return true;
	}
	return false;
}

__int64 Timer::restart() {
	__int64 n = now();
	__int64 e = n - uiStart;
	uiStart = n;
	return e;
}

__int64 Timer::now() {
	static double scale = 0;

	if (scale == 0) {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		scale = 1000000. / freq.QuadPart;
	}

	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	__int64 e = li.QuadPart;

	return static_cast<__int64>(e * scale);
}
