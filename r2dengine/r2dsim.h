#ifndef _R2DSIM_H_
#define _R2DSIM_H_

#include "chipmunk.h"
#include "r2dstructs.h"
#include "r2dcar.h"
#include "r2dobject.h"
#include "r2dsurface.h"

#include <vector>

using namespace std;

class R2DSim {
	vector<R2DCar*> cars;
	vector<R2DObject*> objects;
	//vector<R2DSurface*> surfaces;

	cpSpace* space;
	cpBody* static_body;

	float gravity;

	surface_query_func query_surface;
	void* query_surface_data;

public:
	R2DSim();
	~R2DSim();

	//objects
	void add_car(R2DCar* car,cpVect pos,double angle,bool is_static=false);
	void add_object(R2DObject* obj,cpVect pos,double angle,bool is_static=false);
	//void add_surface(R2DSurface* surface);

	void car_add_shape_quad(R2DObject* obj,cpVect center,cpVect size,int layer,float bounce,float friction);
	void object_add_shape_circle(R2DObject* obj,cpVect center,float r,int layer,float bounce,float friction);
	void object_add_shape_quad(R2DObject* obj,cpVect start,cpVect end,int layer,unsigned int group,float bounce,float friction);
	void object_add_shape_line(R2DObject* obj,cpVect start,cpVect end,int layer,float bounce,float friction);

	void object_attach(R2DObject* object1,R2DObject* object2,cpVect anchr1,cpVect anchr2);

	void add_static_line(cpVect p1,cpVect p2,float bounce,float friction);

	//simulation
	void update(long delta);

	void set_query_func(surface_query_func func,void *user_data);
	void set_gravity(float gravity);	//m/s^2

	cpSpace* get_space();

	void object_sleep(R2DObject* obj);
	void object_wake(R2DObject* obj);
};

#endif

