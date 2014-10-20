
#ifndef _DPARTICLE_H_
#define _DPARTICLE_H_

#include "dvect.h"

enum DParticlesTransformMode { NONE, ADD, MULTIPLY };


class DParticlesTransform {
private:
	DParticlesTransformMode mode;

	float (*transform_func)(DParticlesTransform*,float,long);
public:
	float factor;
	float startValue;

	DParticlesTransform();
	DParticlesTransform(DParticlesTransformMode mode,float factor=1,float startValue=1);

	void setTransformMode(DParticlesTransformMode mode);
	float transform(float current,long ts);
};

struct DParticles {
	DParticlesTransform trSize;
	DParticlesTransform trAlpha;
	DParticlesTransform trAngle;
	long life;

	DParticles();
};

class DParticle {
public:
	DParticles* generator;
	float size;
	float alpha;
	float angle;
	long life;
	dvect pos;

	DParticle(dvect pos,DParticles* particles);
	void update(long ts);
};

#endif

