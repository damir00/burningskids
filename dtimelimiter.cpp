
#include "dtimelimiter.h"

#include <iostream>
using namespace std;

long DTimeLimiter::getTime() {
  gettimeofday(&mtime, NULL);
  return mtime.tv_sec*1000000+mtime.tv_usec;
}

DTimeLimiter::DTimeLimiter() {
	min_delta=0;
}
DTimeLimiter::DTimeLimiter(long _min_delta) {
	min_delta=_min_delta*1000;
}
void DTimeLimiter::firstMark() {
	prev_tick=getTime();
	current_tick=prev_tick;
	delta=0;
}
void DTimeLimiter::mark() {
	current_tick=getTime();
	delta=current_tick-prev_tick;
	prev_tick=current_tick;
}
void DTimeLimiter::wait() {
	if(min_delta==0) return;
	long ddelta=min_delta-delta;
	if(ddelta>0) {
		usleep(ddelta);
	}
}
long DTimeLimiter::getDelta() {
	return delta;
}


