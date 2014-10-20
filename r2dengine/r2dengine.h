#ifndef _R2DENGINE_H_
#define _R2DENGINE_H_

#include "chipmunk.h"
//#include "r2dsim.h"
//#include "r2dcar.h"
//#include "r2dobject.h"

class R2DEngine {
	int gear;
	int max_gear;
	float ratios[5];
	float gas;
	float power; //joules

public:

	//for sim
	cpBody *engine;
	cpBody *beam;
	cpConstraint* gear_joint;

	R2DEngine();
	~R2DEngine();
	void gear_up();
	void gear_down();
	void set_gear(int g);
	int get_gear();
	float get_ratio();
	float get_gas();

	void set_input(float v);	//gas, 0-1

	void set_power(float p);
	float get_power();

	float get_output_power();

	void update(long delta);

};

#endif
