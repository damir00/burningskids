#include "dvect.h"

#include <math.h>

dvect::dvect() {
	x=y=0;
}
/*
dvect::dvect(dvtype _x,dvtype _y) {
	x=_x;
	y=_y;
}
*/
void dvect::operator+=(dvect v) {
	x+=v.x;
	y+=v.y;
}
void dvect::operator-=(dvect v) {
	x-=v.x;
	y-=v.y;
}
void dvect::operator*=(dvtype v) {
	x*=v;
	y*=v;
}
dvect dvect::operator*(dvtype v) {
	return dvect(x*v,y*v);
}
dvect dvect::operator*(dvect v) {
	return dvect(x*v.x,y*v.y);
}
dvect dvect::operator-() {
	return dvect(-x,-y);
}
dvect dvect::operator-(dvect v) {
	return dvect(x-v.x,y-v.y);
}
dvect dvect::operator/(dvtype v) {
	return dvect(x/v,y/v);
}
dvect dvect::operator/(dvect v) {
	return dvect(x/v.x,y/v.y);
}
dvect dvect::operator+(dvect v) {
	return dvect(x+v.x,y+v.y);
}
dvtype dvect::length() {
	return sqrt(x*x+y*y);
}
dvtype dvect::length_fast() {
	return (x*x+y*y);
}

dvect dvect::normalize() {
	dvtype len=length();
	if(len==0) return dvect(x,y);
	x/=len;
	y/=len;
	return dvect(x,y);
}

dvect dvect::unrotate(dvect v) {
	return dvect(x*v.x + y*v.y, y*v.x - x*v.y);
}
dvect dvect::perp() {
	return dvect(-y,x);
}
