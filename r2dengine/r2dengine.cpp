
#include "r2dengine.h"

#include "r2dutils.h"

#include <iostream>
using namespace std;

R2DEngine::R2DEngine() {
	max_gear=5;
	gear=1;
	gas=0;
	power=1;

	ratios[0]=-0.5;
	ratios[1]=1;
	ratios[2]=2;
	ratios[3]=3;
	ratios[4]=4;
	ratios[5]=5;

	engine=cpBodyNew(1,1);
	beam=cpBodyNew(0.01,0.01);
	gear_joint=cpGearJointNew(engine, beam, 0, -get_ratio());
}

R2DEngine::~R2DEngine() {

}

void R2DEngine::set_gear(int g) {

	float prev_ratio=-get_ratio();

	gear=g;
	if(gear>max_gear) gear=max_gear;
	else if(gear<-1) gear=-1;

	float ratio=-get_ratio();
	float phase;
	if(ratio<0) phase=cpBodyGetAngle(engine)*1/ratio - cpBodyGetAngle(beam);
	else phase=0; //-cpBodyGetAngle(engine)*1/ratio + cpBodyGetAngle(beam);

	cpGearJointSetPhase(gear_joint,phase);
	cpGearJointSetRatio(gear_joint,ratio);
}
void R2DEngine::gear_down() {
	set_gear(gear-1);
}
void R2DEngine::gear_up() {
	set_gear(gear+1);
}
int R2DEngine::get_gear() {
	return gear;
}
float R2DEngine::get_ratio() {
	return ratios[gear];
}
float R2DEngine::get_gas() {
	return gas;
}
void R2DEngine::set_input(float v)	{	//gas, 0-1
	gas=R2DUtils::capf(v,0.0,1.0);
}
void R2DEngine::update(long delta) {
	return;
	cpBodyResetForces(engine);
	//torque
	cpBodyApplyForce(engine,cpv(0,1*gas),cpv(0.01,0));
	cpBodyApplyForce(engine,cpv(0,-1*gas),cpv(-0.01,0));

	//float r=engine->a/beam->a;
	//cout<<"ratio "<<r<<endl;
}

void R2DEngine::set_power(float p) {
	power=p;
}
float R2DEngine::get_power() {
	return power;
}
float R2DEngine::get_output_power() {
	return power*gas*ratios[gear];
}

