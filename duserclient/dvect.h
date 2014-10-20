
#ifndef _DVECT_H_
#define _DVECT_H_

#include "math.h"

#define dvtype float

class dvect {
	public:
	dvtype x,y;

	dvect();
	inline dvect(dvtype _x,dvtype _y) {
		x=_x;
		y=_y;
	}
	void operator+=(dvect v);
	void operator-=(dvect v);
	void operator*=(dvtype v);
	dvect operator*(dvtype v);
	dvect operator*(dvect v);
	dvect operator-(dvect v);
	dvect operator/(dvtype v);
	dvect operator/(dvect v);
	dvect operator+(dvect v);
	dvect operator-();
	dvtype length();
	dvtype length_fast();

	dvect normalize();
	dvect unrotate(dvect v);
	dvect perp();
};

static const dvect dvectzero=dvect(0,0);
static const dvect dvectone=dvect(1,1);

#endif

