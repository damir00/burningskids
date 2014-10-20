#include "dvect3.h"

dvect3::dvect3() {
	x=y=z=0;
}
dvect3::dvect3(dvtype _x,dvtype _y,dvtype _z) {
	x=_x;
	y=_y;
	z=_z;
}
dvect3::dvect3(dvect xy,dvtype _z) {
	x=xy.x;
	y=xy.y;
	z=_z;
}

dvtype dvect3::length() {
	return sqrt(x*x+y*y+z*z);
}
dvtype dvect3::length_fast() {
	return (x*x+y*y+z*z);
}

dvect3 dvect3::normalize() {
	dvtype len=length();
	if(len==0) return dvect3(x,y,z);
	x/=len;
	y/=len;
	z/=len;
	return dvect3(x,y,z);
}

void dvect3::operator*=(dvtype v) {
	x*=v;
	y*=v;
	z*=v;
}
dvect3 dvect3::operator*(dvtype v) {
	return dvect3(x*v,y*v,z*v);
}
