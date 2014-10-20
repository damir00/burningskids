
#include "r2dsim.h"
#include "r2dutils.h"

#include <stdio.h>
#include <iostream>
using namespace std;

void surface_query_default(R2DSurfaceQuery* query,void* data) {
	query->grip=1;
	query->roughness=0;
}

R2DSim::R2DSim() {
	gravity=-0.01;	//normal gravity
	space=cpSpaceNew();
	static_body=cpBodyNew(INFINITY,INFINITY);

	query_surface=&surface_query_default;
	query_surface_data=NULL;

	//cpSpaceUseSpatialHash(space, 20, 1000);
}
R2DSim::~R2DSim() {
	cpSpaceFree(space);
}

void R2DSim::add_car(R2DCar* car,cpVect pos,double angle,bool is_static) {
	car->set_pos(pos);
	car->set_angle(angle);

	//cars.push_back(car);

	if(is_static) {
		cpBodySetMass(car->get_body(),INFINITY);
		cpBodySetMoment(car->get_body(),INFINITY);
	}
	else {
		//cpSpaceAddBody(space,car->get_body());
		car->update(1,query_surface,query_surface_data);
		car->store_pos();
	}

	//engine sim
	/*
	cpSpaceAddBody(space,car->get_engine()->engine);
	cpSpaceAddBody(space,car->get_engine()->beam);
	cpSpaceAddConstraint(space,car->get_engine()->gear_joint);
	*/

	/*
	//tyre sim the constraint-way
	for(int i=0;i<car->tyres.size();i++) {
		R2DTyre* t=car->tyres[i];
		cpBody* car_body=car->get_body();

		t->body=cpBodyNew(0.001,0.001);
		t->constraint=cpPivotJointNew2(t->body, car_body,
				cpvzero, t->pos);

//		t->constraint->maxForce=0.00003;
		//cpConstraintSetMaxForce(t->constraint,0.0003);

		cpSpaceAddBody(space,t->body);
		cpSpaceAddConstraint(space,t->constraint);

		t->body->p=cpBodyLocal2World(car_body,t->pos);
	}
	*/

	car->update_shapes();

	car->z_pos=car->surf_height;
	car->sleeping=true;

//	cout<<"sim added car\n";
}

void R2DSim::add_object(R2DObject* obj,cpVect pos,double angle,bool is_static) {
	obj->set_pos(pos);
	obj->set_angle(angle);

	if(is_static) {
		cpBodySetMass(obj->get_body(),INFINITY);
		cpBodySetMoment(obj->get_body(),INFINITY);
	}
	else {
		//cpSpaceAddBody(space,obj->get_body());
		obj->update(1,query_surface,query_surface_data);
		obj->store_pos();
		//objects.push_back(obj);
	}

	obj->z_pos=obj->surf_height;
	obj->sleeping=true;
	//obj->update_shapes();	//useless here since no shapes yet attached
}
/*
void R2DSim::add_surface(R2DSurface* surface) {
	surfaces.push_back(surface);
}
*/

float get_3d_dist(float x,float y,float z) {
	return sqrt(x*x+y*y+z*z);
}

void R2DSim::update(long delta) {
//	cout<<"sim update "<<delta<<endl;

	for(vector<R2DCar*>::iterator car_it=cars.begin();car_it<cars.end();car_it++) {
		(*car_it)->store_pos();
	}
	for(vector<R2DObject*>::iterator obj_it=objects.begin();obj_it<objects.end();obj_it++) {
		(*obj_it)->store_pos();
	}

	cpSpaceStep(space,delta);

	//reset forces..
	for(vector<R2DCar*>::iterator car_it=cars.begin();car_it<cars.end();car_it++) {
		(*car_it)->reset_forces();
		(*car_it)->update(delta,query_surface,query_surface_data);
	}
	for(vector<R2DObject*>::iterator obj_it=objects.begin();obj_it<objects.end();obj_it++) {
		(*obj_it)->reset_forces();
		(*obj_it)->update(delta,query_surface,query_surface_data);
	}

	//jumping physics
	for(vector<R2DCar*>::iterator car_it=cars.begin();car_it<cars.end();car_it++) {
		R2DCar* c=*car_it;
		float grav_force=((float)(gravity*delta))/1000;	//TODO: migrate gravity to /1000 to optimise?
		c->z_vel+=grav_force;
		c->z_pos+=c->z_vel*delta;

		bool airborne;
		if(c->z_pos<c->surf_height) {
			c->z_pos=c->surf_height;
			airborne=false;

			float z_delta=c->z_pos-c->prev_z_pos;
			float new_z_vel=z_delta/delta*1;

			//now damp horizontal and vertical speed
			//bigger change in vertical speed=bigger change in horizontal speed
			//3d velocity vector must have same length

			//printf("z vel %f new z vel %f\n",c->z_vel,new_z_vel);

			float velocity=get_3d_dist(c->get_vel().x,c->get_vel().y,c->z_vel*5);
			//hax to match z_pos with already applied gravity
			float new_velocity=get_3d_dist(c->get_vel().x,c->get_vel().y,(new_z_vel+grav_force)*5);
			c->z_vel=new_z_vel;

			if(new_velocity>velocity) {
				float ratio=velocity/new_velocity;
				/*
				printf("speed ratio %f zdelta %f vel %f new vel %f\n",
						ratio,z_delta,
						velocity,new_velocity);
				*/
				//ratio*=ratio;
				//not 100% proper, might break collision responses a bit
				c->get_body()->v.x*=ratio;
				c->get_body()->v.y*=ratio;
				c->z_vel*=ratio;
			}

		}
		else {
			//printf("car height %f\n",c->z_pos);
			airborne=true;
		}
		//tyre surface contact
		bool have_contact=!airborne;
		/*
		for(vector<R2DTyre*>::iterator tyre_it=c->tyres.begin();tyre_it<c->tyres.end();tyre_it++) {
			R2DTyre* t=*tyre_it;
			if(t->surf_height+R2D_TYRE_CONTACT_OFFSET<c->height) {
			}
			else {
				have_contact=true;
				break;
			}
		}
		*/
		for(vector<R2DTyre*>::iterator tyre_it=c->tyres.begin();tyre_it<c->tyres.end();tyre_it++) {
			(*tyre_it)->has_contact=have_contact;
		}

		//update shape layers because we changed object height

		c->update_shapes();
	}
}

//TODO: support for more than 1 shape
void R2DSim::car_add_shape_quad(R2DObject* obj,cpVect center,cpVect size,int layer,
		float bounce,float friction) {
	obj->shape=R2DUtils::shape_quad(obj->get_body(),size,center);
	obj->shape->group=0;
	obj->update_shapes();

	obj->shape->e=bounce;
	obj->shape->u=friction;
}
void R2DSim::object_add_shape_circle(R2DObject* obj,cpVect center,float r,int layer,
		float bounce,float friction) {

	obj->shape=R2DUtils::shape_circle(obj->get_body(),r,center);
	obj->shape->group=0;
	obj->update_shapes();

	obj->shape->e=bounce;
	obj->shape->u=friction;
}
void R2DSim::object_add_shape_quad(R2DObject* obj,cpVect start,cpVect end,int layer,
		unsigned int group,float bounce,float friction) {
	obj->shape=R2DUtils::shape_rect(obj->get_body(),start,end);
	obj->shape->group=group;
	obj->update_shapes();

	obj->shape->e=bounce;
	obj->shape->u=friction;
}
void R2DSim::object_add_shape_line(R2DObject* obj,cpVect start,cpVect end,int layer,
		float bounce,float friction) {
	obj->shape=R2DUtils::shape_line(obj->get_body(),start,end);
	obj->shape->group=0;
	obj->update_shapes();

	obj->shape->e=bounce;
	obj->shape->u=friction;
}

void R2DSim::object_attach(R2DObject* object1,R2DObject* object2,cpVect anchr1,cpVect anchr2) {
	cpConstraint* c=cpPivotJointNew2(object1->get_body(),object2->get_body(),anchr1,anchr2);
	cpSpaceAddConstraint(space,c);
}

void R2DSim::add_static_line(cpVect p1,cpVect p2,float bounce,float friction) {
	cpShape* s=R2DUtils::shape_line(static_body,p1,p2);
	s->e=bounce;
	s->u=friction;

	R2DUtils::space_add_shape(space,s,true);
}

void R2DSim::set_query_func(surface_query_func func,void* data) {
	if(func) query_surface=func;
	else query_surface=&surface_query_default;
	query_surface_data=data;
}
void R2DSim::set_gravity(float _gravity) {
	gravity=_gravity;
}
cpSpace* R2DSim::get_space() {
	return space;
}

void R2DSim::object_sleep(R2DObject* obj) {
	if(obj->sleeping) return;
	obj->sleeping=true;
	cpSpaceRemoveBody(space,obj->get_body());
	cpSpaceRemoveShape(space,obj->shape);

	for(int i=0;i<objects.size();i++) {
		if(objects[i]==obj) {
			objects.erase(objects.begin()+i);
			break;
		}
	}
}
void R2DSim::object_wake(R2DObject* obj) {
	if(!obj->sleeping) return;
	obj->sleeping=false;

	cpSpaceAddBody(space,obj->get_body());
	cpSpaceAddShape(space,obj->shape);

	if(!obj->is_static()) {
		if(obj->is_car) {
			cars.push_back((R2DCar*)obj);
		}
		else {
			objects.push_back(obj);
		}
	}
}


