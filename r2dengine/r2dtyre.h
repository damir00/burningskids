#ifndef _R2DTYRE_H_
#define _R2DTYRE_H_

#include "chipmunk.h"

#include "r2dlerper.h"
#include "r2dutils.h"
#include "r2dstructs.h"

#define R2D_TYRE_CONTACT_OFFSET 0.03	//meters

class R2DTyre {
	R2DLerper lerper;

	void init();
public:
	cpBody* body;
	cpConstraint* constraint;

	cpVect pos;
	cpVect prev_world_pos;
	double grip;

	double angle;	//relative angle in rad
	double cur_angle;
	double max_angle_offset;

	double rot_vel;	//aka acc
	double true_rot_vel;	//rotation velocity
	double brake;	//0-1

	bool powered;
	bool steerable;
	bool has_contact;	//ground contact
	bool sliding;

	cpVect heading;
	cpVect out_force;
	cpVect desired_pos;
	cpVect vel;

	float surf_height;

	//factors for tweaking
	static float factor_out_force_mult;
	static float factor_grip_force_mult;
	static float factor_slide_treshold;


	cpVect outputForce(cpVect pos,cpVect vel,double angle,surface_query_func sq,void* query_data);
	cpVect outputForce2(cpVect world_pos,
			cpVect world_vel,
			float car_angle,
			float time_delta,
			surface_query_func sq,
			void* query_data);

	R2DTyre();
	R2DTyre(cpVect pos,double angle,bool powered,bool steerable);
	~R2DTyre();

	void update(long delta);
	void turn(float x);	//-1-1
};

#endif

