#ifndef _R2DOBJECT_H_
#define _R2DOBJECT_H_

enum R2DObjectType { STATIC, DYNAMIC };

#include "r2dtyre.h"

#include "chipmunk.h"
#include <vector>
#include <math.h>

#define R2D_OBJECT_HEIGHT_RESOLUTION 0.5	//0.5 meter z collision resolution

using namespace std;

class R2DObject {
	R2DObjectType type;	//TODO: used anywhere?

protected:
	cpBody* body;
	cpVect prev_pos;
	bool turned;	//tyres turned this frame?

	int num_powered_tyres;	//used to distribute engine power to tyres

	void update_tyres(long delta,surface_query_func sq,void* query_data);
public:

	float z_pos;
	float z_vel;
	float prev_z_pos;
	float surf_height;	//height of terrain under the object
	float height;		//in meters
	cpShape* shape;

	bool sleeping;
	bool is_car;

	vector<R2DTyre*> tyres;

	R2DObject(R2DObjectType type,float mass,float interia);
	~R2DObject();
	R2DObjectType get_type();
	bool is_static();
	void set_pos(cpVect pos);
	void set_angle(float angle);
	cpVect get_pos();
	float get_z_pos();
	cpVect get_prev_pos();
	float get_angle();
	cpVect get_vel();
	float get_z_vel();
	double get_lon_vel();	//TODO: switch to float?
	cpVect get_heading();

	void add_tyre(R2DTyre* tyre);

	void store_pos();
	virtual void update(long delta,surface_query_func sq,void* query_data);
	void update_shapes();
	cpLayers get_layers();	//collision layers

	cpBody* get_body();
	void reset_forces();
};

#endif

