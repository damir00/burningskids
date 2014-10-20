
#ifndef _DTIMELIMITER_H_
#define _DTIMELIMITER_H_

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

class DTimeLimiter {
	struct timeval mtime;

	long prev_tick;
	long current_tick;
	long delta;

	long getTime();
public:
	long min_delta;

	DTimeLimiter();
	DTimeLimiter(long min_delta);
	void firstMark();
	void mark();
	void wait();
	long getDelta();	//nanoseconds
};

#endif

