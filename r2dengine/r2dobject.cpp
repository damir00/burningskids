
#include "r2dobject.h"

#include <stdio.h>

#include <iostream>
using namespace std;

R2DObject::R2DObject(R2DObjectType ptype,float mass,float interia) {
	type=ptype;	
	body=cpBodyNew(mass,interia);
	z_pos=0;
	z_vel=0;
	prev_z_pos=0;
	surf_height=0;
	shape=NULL;
	height=0.5;
	num_powered_tyres=0;
	sleeping=true;
	is_car=false;
}

R2DObject::~R2DObject() {

}

void R2DObject::set_pos(cpVect pos) {
	body->p=pos;
}
void R2DObject::set_angle(float angle) {
	body->a=angle;
}
cpVect R2DObject::get_pos() {
	return body->p;
}
float R2DObject::get_angle() {
	return body->a;
}
cpVect R2DObject::get_vel() {
	return body->v;
}
float R2DObject::get_z_vel() {
	return z_vel;
}
double R2DObject::get_lon_vel() {
	return cpvdot(body->v,body->rot);
}
cpVect R2DObject::get_heading() {
	return body->rot;
}

cpBody* R2DObject::get_body() {
	return body;
}

void R2DObject::add_tyre(R2DTyre* tyre) {
	if(tyre->powered) num_powered_tyres++;
	tyres.push_back(tyre);
}

void R2DObject::update_tyres(long delta,surface_query_func sq,void* query_data) {

	surf_height=0; //-INFINITY;

	for(vector<R2DTyre*>::iterator it=tyres.begin();it<tyres.end();it++) {
		R2DTyre* tyre=*it;

		if(!turned) {
			tyre->turn(0);
		}

		tyre->update(delta);


		/*
		cpVect force=tyre->outputForce(
				body->p,		//world pos
				cpvunrotate(	//world vel
						cpvadd(body->v,
								cpvrotate(
										R2DUtils::getBodyPointVel(body,tyre->pos),
						body->rot)),
						body->rot),
				body->a,		//world angle
				sq,query_data);

		cpBodyApplyForce(body,cpvrotate(force,body->rot),cpvrotate(tyre->pos,body->rot));
		*/

		cpVect rot_vel=cpvrotate(R2DUtils::getBodyPointVel(body,tyre->pos),
				body->rot);

		cpVect force=tyre->outputForce2(
				cpBodyLocal2World(body,tyre->pos),
				body->v+rot_vel,
				body->a,
				delta,
				sq,query_data);
		cpBodyApplyForce(body,force,cpvrotate(tyre->pos,body->rot));

		/*
		if(tyre->body) {
			cpVect heading=cpvforangle(body->a+tyre->cur_angle);

			if(tyre->rot_vel>tyre->true_rot_vel) {
				tyre->true_rot_vel=tyre->rot_vel;
			}
			else {
				tyre->true_rot_vel=cpvdot(heading,tyre->body->v);
			}

			cpVect npos=cpBodyLocal2World(body,tyre->pos)+
					heading*tyre->true_rot_vel;

	//		tyre->body->v=cpvsub(npos,tyre->body->p)*0.001;
	//		tyre->body->p=npos;


	//		cpBodySlew(tyre->body,npos,delta);


			cpBodyResetForces(tyre->body);

			cpVect npos2=tyre->body->p+heading*(cpvlength(tyre->body->v)+tyre->true_rot_vel*10);

			cpBodySlew(tyre->body,npos2,delta);

			cpPivotJoint* j=(cpPivotJoint*)tyre->constraint;

			cpFloat force = cpConstraintGetImpulse(tyre->constraint);
			cpFloat maxForce = 0.0003*0.9; //cpConstraintGetMaxForce(tyre->constraint);

			printf("force %f/%f, vel %f, true rot vel %f\n",
					force,maxForce,
					cpvlength(tyre->body->v),
					tyre->true_rot_vel);
		}
		*/

		//printf(" tyre applied force %f %f\n",cpvrotate(force,body->rot).x,cpvrotate(force,body->rot).y);

		tyre->rot_vel=0;
		tyre->brake=0;

		surf_height=fmax(surf_height,tyre->surf_height); //+=tyre->surf_height;

	}
	//height/=(float)tyres.size();

	turned=false;
}

/*
float sec(float z) {
  return 1.0/cos(z);
}
*/

void R2DObject::update(long delta,surface_query_func sq,void* query_data) {
	cpBodyResetForces(body);
	update_tyres(delta,sq,query_data);

	//cpBodyApplyForce(body,cpvclamp(cpvmult(body->v,-0.01),0.000001),cpv(0,0));

	//air resistance
	float len=cpvlength(body->v);

	float f=-pow(len*0.5,3);

	cpBodyApplyForce(body,
			cpvnormalize_safe(body->v)*f,
			//cpvmult(body->v,-len*0.01),
			cpvzero);
}

cpVect R2DObject::get_prev_pos() {
	return prev_pos;
}
void R2DObject::store_pos() {
	prev_pos=body->p;
	prev_z_pos=z_pos;
}
float R2DObject::get_z_pos() {
	return z_pos;
}
void R2DObject::reset_forces() {
	body->f=cpvzero;
	body->t=0;
}

//get collision bit from world z coordinate
//32 collision bits
//collision bit position = world_z / HEIGHT_RESOLUTION
char getCollisionBit(float z) {
	return floor(z/R2D_OBJECT_HEIGHT_RESOLUTION);
}

void R2DObject::update_shapes() {
	if(shape==NULL) return;

	if(shape->group==0) {
		shape->layers=0;
		char start=getCollisionBit(z_pos);
		char end=getCollisionBit(z_pos+height);
		for(int i=start;i<=end;i++) {	//at least one bit will be set
			shape->layers |= 1 << i;
		}
	}
	else {
		//TODO: hax
		shape->layers=0x8000;
		//shape->group=1;
	}
}
cpLayers R2DObject::get_layers() {
	if(shape==NULL) return CP_ALL_LAYERS;
	return shape->layers;
}

R2DObjectType R2DObject::get_type() {
	return type;
}
bool R2DObject::is_static() {
	return type==STATIC;
}




