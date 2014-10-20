
#include "r2dtyre.h"

#include "math.h"
#include <iostream>
using namespace std;

#include <stdio.h>

float R2DTyre::factor_out_force_mult=0.00005;
float R2DTyre::factor_grip_force_mult=0.00001;
float R2DTyre::factor_slide_treshold=0.5;

R2DTyre::R2DTyre() {
	powered=false;
	steerable=false;
	init();
}

R2DTyre::R2DTyre(cpVect _pos,double _angle,bool _powered,bool _steerable) {
	pos=_pos;
	angle=_angle;
	powered=_powered;
	steerable=_steerable;
	cur_angle=angle;

	init();
}
void R2DTyre::init() {

	max_angle_offset=0.1;
	rot_vel=0;
	true_rot_vel=0;
	angle=cur_angle=0;
	brake=0;

	lerper.init(-1,1,0.002);
	lerper.center();

	surf_height=0;
	has_contact=true;

	body=NULL;
	constraint=NULL;

	sliding=false;
	prev_world_pos=cpvzero;
}

R2DTyre::~R2DTyre() {

}

/*
//version 3
cpVect R2DTyre::outputForce2(cpVect world_pos,
		cpVect world_vel,
		float car_angle,
		float time_delta,
		surface_query_func sq,
		void* query_data) {

	R2DSurfaceQuery q;
	q.pos=world_pos;
	sq(&q,query_data);	//query surface
	surf_height=q.height;
	if(!has_contact) return cpvzero;


	float world_angle=car_angle+cur_angle;
	cpVect world_heading=cpvforangle(world_angle);

	cpVect next_world_pos=world_pos+world_vel*time_delta;

	true_rot_vel=cpvdot(world_vel,world_heading);
	true_rot_vel=cpfmax(true_rot_vel,rot_vel);
	if(brake) true_rot_vel=0;

	cpVect pos_dif=world_pos-desired_pos;

	desired_pos=world_pos+world_heading*(true_rot_vel*1)*time_delta;

	out_force=(desired_pos-next_world_pos)*0.0001;

	if(!sliding) {
		out_force=out_force-pos_dif*factor_grip_force_mult;
	}

	float clamp=0.00001;

	sliding=false;
	if(cpvlength(pos_dif)>1*q.grip) {
		clamp*=0.5;
		sliding=true;
	}

	prev_world_pos=world_pos;

	if(sliding || true) {
		out_force=cpvclamp(out_force,clamp);
	}

	return out_force;
}
*/

/*
//version 2
cpVect R2DTyre::outputForce2(cpVect world_pos,
		cpVect world_vel,
		float car_angle,
		float time_delta,
		surface_query_func sq,
		void* query_data) {

	R2DSurfaceQuery q;
	q.pos=world_pos;
	sq(&q,query_data);	//query surface
	surf_height=q.height;
	if(!has_contact) return cpvzero;


	float world_angle=car_angle+cur_angle;
	cpVect world_heading=cpvforangle(world_angle);

	cpVect next_world_pos=world_pos+world_vel*time_delta;

	true_rot_vel=cpvdot(world_vel,world_heading);
	true_rot_vel=cpfmax(true_rot_vel,rot_vel);
	if(brake) true_rot_vel=0;

	cpVect pos_dif=world_pos-desired_pos;

	desired_pos=world_pos+world_heading*(true_rot_vel*1)*time_delta;

	out_force=(desired_pos-next_world_pos)*factor_out_force_mult;

	if(!sliding) {
		out_force=out_force-pos_dif*factor_grip_force_mult;
	}

	float clamp=0.001;

	sliding=false;

	//float max_force=0.000005*q.grip;

//	if(powered)
//		printf("diff %f\n",cpvlength(pos_dif));

	if(cpvlength(pos_dif)>factor_slide_treshold*q.grip) {
	//if(cpvlength(out_force)>max_force) {
		clamp*=0.5;
		sliding=true;
	}

	prev_world_pos=world_pos;

	if(sliding) {
		out_force=out_force*0.5;
		//out_force=cpvclamp(out_force,max_force);
	}

	printf("force len %f\n",cpvlength(out_force));

	return out_force;
}
*/


//relative to world
//version 1
cpVect R2DTyre::outputForce2(cpVect world_pos,
		cpVect world_vel,
		float car_angle,
		float time_delta,
		surface_query_func sq,
		void* query_data) {

	R2DSurfaceQuery q;
	q.pos=world_pos;
	sq(&q,query_data);	//query surface
	surf_height=q.height;
	if(!has_contact) return cpvzero;


	float world_angle=car_angle+cur_angle;
	cpVect world_heading=cpvforangle(world_angle);

	cpVect next_world_pos=world_pos+world_vel*time_delta;

	true_rot_vel=cpvdot(world_vel,world_heading);
	true_rot_vel=cpfmax(true_rot_vel,rot_vel);
	if(brake) true_rot_vel=0;

	cpVect pos_dif=world_pos-desired_pos;

	desired_pos=world_pos+world_heading*(true_rot_vel*1)*time_delta;

	out_force=(desired_pos-next_world_pos)*0.0001;

	float clamp=0.00001;

	sliding=false;
	if(cpvlength(pos_dif)>1*q.grip) {
		clamp*=0.5;
		sliding=true;
	}

	prev_world_pos=world_pos;

	if(sliding || true) {
		out_force=cpvclamp(out_force,clamp);
	}

	return out_force;
}


cpVect R2DTyre::outputForce(cpVect _pos,cpVect _vel,
		double car_angle,surface_query_func sq,void* query_data) {

	vel=_vel;

	heading=cpvforangle(cur_angle);
	cpVect heading_perp=cpvperp(heading);	//tangent

	desired_pos=cpvmult(cpvproject(vel,heading),(1-brake));
	desired_pos=cpvadd(desired_pos,cpvmult(heading,rot_vel));
/*
 * 	out_force=cpvsub(desired_pos,vel);
*/
	cpVect tyre_pos=_pos+cpvrotate(pos,heading);

	R2DSurfaceQuery q;
	q.pos=tyre_pos;
	sq(&q,query_data);	//query surface

	surf_height=q.height;
	if(!has_contact) return cpvzero;

	double vel_long;
	double vel_lang;
	double force_long=0;
	double force_lang=0;

	vel_long=cpvdot(vel,heading);
	vel_lang=cpvdot(vel,heading_perp);

	force_long-=vel_long*(brake);
	force_long+=rot_vel;
	force_lang=-vel_lang;

	//force_lang*=2;

	double max_force_lang=0.0060*q.grip;
	double max_force_long=0.0060*q.grip;

	float c=2;

	/*
	printf("pos %f %f, prev desired %f %f, dist %f\n",
			_pos.x,_pos.y,
			prev_world_pos.x,prev_world_pos.y,
			cpvdist(_pos,prev_world_pos));
	if(cpvdist(_pos,prev_world_pos)>0.1) {
		sliding=true;
		force_lang/=c;
		force_long/=c;
	}
	prev_world_pos=pos+desired_pos*10;
	*/

	/*
	sliding=false;
	if(force_lang>max_force_lang) {
		force_lang=max_force_lang/c;
		sliding=true;
	}
	else if(force_lang<-max_force_lang) {
		force_lang=-max_force_lang/c;
		sliding=true;
	}
	*/
	/*
	if(force_long>max_force_long) {
		force_long=max_force_long/c;
	}
	else if(force_long<-max_force_long) {
		force_long=-max_force_long/c;
	}
	*/

//	cout<<"lang force "<<force_lang<<endl;

	out_force=cpvmult(heading,force_long);
	out_force=cpvadd(out_force,cpvmult(heading_perp,force_lang));

	out_force=cpvmult(out_force,0.02);	//TODO: get rid of this

	return out_force;
}

void R2DTyre::update(long delta) {
	cur_angle=angle+lerper.update(delta)*max_angle_offset;
}
void R2DTyre::turn(float x) {
//	cout<<"turn "<<x<<", current "<<lerper.getValue()<<endl;
	if(R2DUtils::abs(x)<R2DUtils::abs(lerper.getValue())) {
		lerper.set(x);
//		cout<<"set\n";
	}
	else {
		lerper.target(x);
//		cout<<"target\n";
	}
}

