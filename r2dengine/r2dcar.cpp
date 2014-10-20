#include "r2dcar.h"
#include "math.h"
#include <iostream>

#include <stdio.h>

R2DCar::R2DCar(R2DObjectType type,float mass,float interia) :
	R2DObject(type,mass,interia) {

	//body=cpBodyNew(mass,interia);
	engine=new R2DEngine();
	turned=false;
	is_car=true;
}
R2DCar::~R2DCar() {

}

void R2DCar::set_turn(double turn) {
	for(vector<R2DTyre*>::iterator it=tyres.begin();it<tyres.end();it++) {
		R2DTyre* tyre=*it;
		if(!tyre->steerable) continue;
		tyre->turn(turn);
	}
	turned=true;
}
void R2DCar::set_acc(double acc) {
	engine->set_input(acc);

	if(num_powered_tyres<=0) return;

	float tyre_power=engine->get_output_power()/(float)num_powered_tyres;
	for(vector<R2DTyre*>::iterator it=tyres.begin();it<tyres.end();it++) {
		R2DTyre* tyre=*it;
		if(!tyre->powered) continue;
		tyre->rot_vel=tyre_power;
	}
}
void R2DCar::set_brake(double brake) {
	for(vector<R2DTyre*>::iterator it=tyres.begin();it<tyres.end();it++) {
		R2DTyre* tyre=*it;
		tyre->brake=brake;
	}
}
void R2DCar::gear_up() {
	engine->gear_up();
}
void R2DCar::gear_down() {
	engine->gear_down();
}
void R2DCar::gear(int g) {
	engine->set_gear(g);
}
int R2DCar::get_gear() {
	return engine->get_gear();
}

void R2DCar::update(long d,surface_query_func sq,void* query_data) {
	engine->update(d);

	cpBodyResetForces(body);
	update_tyres(d,sq,query_data);

	//add some drag
//	cpBodyApplyForce(body,cpvmult(body->v,-0.0001),cpv(0,0));
	//air resistance
	float len=cpvlength(body->v);

	float f=-pow(len*0.5,3);

	cpBodyApplyForce(body,
			cpvnormalize_safe(body->v)*f,
			//cpvmult(body->v,-len*0.01),
			cpvzero);

}
void R2DCar::snap_to(R2DCar* car,cpVect my_coords,cpVect target_coords) {

}
R2DEngine* R2DCar::get_engine() {
	return engine;
}

