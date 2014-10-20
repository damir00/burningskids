#ifndef _R2D_LERPER_H_
#define _R2D_LERPER_H_

class R2DLerper {
	float c_low,c_high;
	float current;
	float vel;
	float c_target;
public:
	R2DLerper();
	void init(float low,float high,float vel);
	void target(float x);
	void set(float x);
	float update(long delta);
	float getValue();

	void low();
	void high();
	void center();
};

#endif

