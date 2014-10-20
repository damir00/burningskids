
#include "dparticle.h"

float transform_none(DParticlesTransform* tr,float x,long ts) {
	return x;
}
float transform_add(DParticlesTransform* tr,float x,long ts) {
	return x+tr->factor*ts;
}
float transform_multiply(DParticlesTransform* tr,float x,long ts) {
	return x*tr->factor*ts;
}


DParticlesTransform::DParticlesTransform() {
	setTransformMode(NONE);
}
DParticlesTransform::DParticlesTransform(DParticlesTransformMode mode,float _factor,float _startValue) {
	setTransformMode(mode);
	factor=_factor;
	startValue=_startValue;
}

void DParticlesTransform::setTransformMode(DParticlesTransformMode _mode) {
	mode=_mode;
	switch(mode) {
		case ADD: transform_func=transform_add; break;
		case MULTIPLY: transform_func=transform_multiply; break;
		default: transform_func=transform_none;
	}
}
float DParticlesTransform::transform(float current,long ts) {
	return transform_func(this,current,ts);
}

DParticles::DParticles() {
	life=1;
}

DParticle::DParticle(dvect _pos,DParticles* p) {
	pos=_pos;
	generator=p;
	size=p->trSize.startValue;
	angle=p->trAngle.startValue;
	alpha=p->trAlpha.startValue;
	life=p->life;
}
void DParticle::update(long ts) {
	size=generator->trSize.transform(size,ts);
	angle=generator->trAngle.transform(angle,ts);
	alpha=generator->trAlpha.transform(alpha,ts);
	life-=ts;
}
