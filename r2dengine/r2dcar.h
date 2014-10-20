#ifndef _R2DCAR_H_
#define _R2DCAR_H_

#include <vector>

#include "chipmunk.h"
#include "r2dstructs.h"
#include "r2dtyre.h"
#include "r2dengine.h"
#include "r2dutils.h"
#include "r2dobject.h"

using namespace std;

class R2DCar : public R2DObject {
	R2DEngine* engine;
//	cpBody* body;

public:
	R2DCar(R2DObjectType type,float mass,float interia);
	~R2DCar();

	R2DEngine* get_engine();

	void set_turn(double turn);	//-1 - 1
	void set_acc(double acc);	//0-1
	void set_brake(double brake);	//0-1
	void gear_down();
	void gear_up();
	void gear(int g);
	int get_gear();
	void update(long d,surface_query_func sq,void* query_data);

//	void set_pos(cpVect pos);
//	cpVect get_pos();
//	void set_angle(double angle);
//	double get_angle();

	void snap_to(R2DCar* car,cpVect my_coords,cpVect target_coords);
};

#endif

