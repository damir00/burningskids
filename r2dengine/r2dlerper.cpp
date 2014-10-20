
#include "r2dlerper.h"

#include <iostream>
using namespace std;

R2DLerper::R2DLerper() {
	current=0;
}
void R2DLerper::init(float _low,float _high,float _vel) {
	c_low=_low;
	c_high=_high;
	vel=_vel;

	center();
}
void R2DLerper::target(float x) {
	c_target=x;
}
void R2DLerper::set(float x) {
	c_target=x;
	current=x;
}

void R2DLerper::low() {
	c_target=current=c_low;
}
void R2DLerper::high() {
	c_target=current=c_high;
}
void R2DLerper::center() {
	c_target=current=(c_low+c_high)/2;
}

float R2DLerper::update(long delta) {

	if(current<c_target) {
		current+=vel*delta;
		if(current>c_target) {
			current=c_target;
		}
	}
	else {
		current-=vel*delta;
		if(current<c_target) {
			current=c_target;
		}
	}
	return current;
}
float R2DLerper::getValue() {
	return current;
}
