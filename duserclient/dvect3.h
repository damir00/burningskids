#ifndef _DVECT3_H_
#define _DVECT3_H_

#include "dvect.h"

class dvect3 {
	public:
	dvtype x,y,z;

	dvect3();
	dvect3(dvtype x,dvtype y,dvtype z);
	dvect3(dvect xy,dvtype z);
//	void operator+=(dvect3 v);
	void operator*=(dvtype v);
	dvect3 operator*(dvtype v);
//	dvect3 operator*(dvect3 v);
//	dvect3 operator-(dvect3 v);
//	dvect3 operator/(dvtype v);
//	dvect3 operator+(dvect3 v);
//	dvect3 operator-();
	dvtype length();
	dvtype length_fast();

	dvect3 normalize();
};

#endif

