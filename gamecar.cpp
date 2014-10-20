#include "gamecar.h"

#include "duserclientfreeglut.h"

void post_draw(DRenderNode* node,void* pclient,void *pcar);

void GameCar::addLights(vector<DRenderNode*> *dst,vector<GameObjectLight> *src,
		GameResourceManager* m,bool visible,float scale) {
	for(vector<GameObjectLight>::iterator l=src->begin();l<src->end();l++) {
		DRenderNode* node=render_node->createNode(l->pos*scale,l->angle);
		node->object=m->client->createLight(m->getImg(l->color_filename),l->size*scale,l->center);
		node->visible=visible;
		dst->push_back(node);
	}

}

void GameCar::init(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
		float scale) {

	physic_object->height=0;

	for(vector<GameObjectPart>::iterator p=body->parts.begin();p<body->parts.end();p++) {
		if(p->render) {
			DRenderNode* n=render_node->createNode(p->pos,p->angle);

			dvect size2d=dvect(p->size.x,p->size.y);

			n->object=manager->client->createCar(
					manager->getImg(p->color_filename),
					manager->getImg(p->normal_filename),
					manager->getImg(p->height_filename),
					size2d*scale);

			n->object->geometry=manager->getGeometry(p->color_filename);

			total_size=p->size*scale;

			parts.push_back(n);
		}

		if(p->shape_type!=SHAPE_NONE) {
			dvect s=dvect(p->size.x,p->size.y)/2 * scale;
			GameShape shape=GameShape(p->shape_type,-s,s,p->bounce,p->friction);
			shapes.push_back(shape);
		}
		physic_object->height=fmax(physic_object->height,p->pos.z+p->size.z);
	}

	addLights(&lights_front,&body->lights_front,manager,true,scale);
	addLights(&lights_brake,&body->lights_brake,manager,false,scale);
	addLights(&lights_reverse,&body->lights_reverse,manager,false,scale);
	addLights(&lights_aux,&body->lights_aux,manager,true,scale);

	for(vector<GameCarTyre>::iterator t=body->tyres.begin();t<body->tyres.end();t++) {
		R2DTyre* tyre=new R2DTyre(dvectToCpVect(t->pos*scale),t->angle,t->powered,t->steerable);
		tyre->max_angle_offset=degToRad(t->steer_angle);
		physic_object->add_tyre(tyre);

		tyre_skids.push_back(0);
	}
	physic_object->get_engine()->set_power(body->engine_power);

	//size=body->size;
	carBody = body;

	sensor_data=NULL;
	sensor_enabled=false;

	name="car";
	controls_locked=false;

	lap=0;
	rank=0;
	score=0;
	final_score=0;
	is_done=false;
	is_brake=false;
	//render_node->setPostDraw(post_draw,this);

	last_acc=0;
	last_turn=0;

	sounds.data = carBody->sounds.data();
	resource_manager = manager;

	is_car=false;	//false until initEngine
}


//static
GameCar::GameCar(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
		dvect3 pos,float angle,float scale) {

	render_node=new DRenderNode();
	//render_node=graphic->node_objects->createNode();

	physic_object=new R2DCar(STATIC,body->mass,body->mass_interia);
	init(body,graphic,manager,scale);
	set(cpv(pos.x,pos.y),angle);

	//calc BB
//	manager->client->nodeCalcBB(render_node);
	render_node->calcBB();
	bb=render_node->bb;
	//manager->client->octree->putNode(render_node);
}

GameCar::GameCar(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
		float scale) {

	//old way
//	render_node=graphic->node_objects->createNode();
	render_node=new DRenderNode();

	physic_object=new R2DCar(DYNAMIC,body->mass,body->mass_interia);
	init(body,graphic,manager,scale);

//	set(cpv(pos.x,pos.y),angle);
//	render_node->calcBB();

	//manager->client->octree->putDynamicNode(render_node);
}
GameCar::~GameCar() {
	for(int i=0;i<parts.size();i++) {
		delete(parts[i]->object);
	}
	parts.clear();

	render_node->removeFromParent();

	if (sounds.data) {
		resource_manager->getSoundManager()->carStop(&sounds, true);
		resource_manager->getSoundManager()->disposeSounds(&sounds);
	}

	//lights_node->removeFromParent();
}


void GameCar::set(cpVect pos,float angle) {
	physic_object->set_pos(pos);
	physic_object->set_angle(angle);
	render_node->pos=dvect3(cpVectToDvect(pos),0);
	render_node->angle=radToDeg(angle);
}

void GameCar::initEngine(GameResourceManager* manager) {
	srand( (unsigned)time( NULL ) );

	is_car = true;

	if (sounds.data) {
		manager->getSoundManager()->carStart(&sounds);
	}
}

/*
void GameCar::setSize(dvect size) {
	render_object->size=size;
}
*/
void GameCar::update(GameResourceManager* m, long time) {
	//physic_object->update(time);

	float z=physic_object->get_z_pos();
	dvect3 pos=dvect3(cpVectToDvect(physic_object->get_pos()),z);
	double angle=radToDeg(physic_object->get_angle());

	render_node->set(pos,angle);
	//lights_node->set(pos,angle);

	float scale=1+z/5;
	dvect3 msize=dvect3(scale,scale,scale);
	render_node->scale=msize;
	//lights_node->scale=msize;

	if(!is_brake) brake_on(false);
	is_brake=false;

	if(sensor_enabled) {
		sensorsUpdate();
	}

	render_node->calcBB();
	bb=render_node->bb;

	if(is_car && sounds.data) {
		m->getSoundManager()->carUpdate(&sounds, getSpeed(), cpVectToDvect(physic_object->get_pos()));
	}
	/*
	render_node->calcBB();	//TODO: break into calcBBNested and calcBB
	DRenderOctree* t=(DRenderOctree*)render_node->tree_ptr;
	t->reinsertDynamicNode(render_node,NULL);
	*/

//	cout<<"car angle "<<physic_object->get_angle()<<endl;
}

void GameCar::accelerate(float power) {

	if(power<0) {
		brake(-power);
		return;
	}

	if(controls_locked) return;

	//printf("vel %f\n",physic_object->get_lon_vel());

	power=GameMath::clampf(power,0,1);
	if(physic_object->get_lon_vel()<-0.001) {
		physic_object->set_brake(0.08);
		brake_on(true);
		reverse_on(true);
	}
	else {
		physic_object->gear(1);
		physic_object->set_acc(power);
		brake_on(false);
		reverse_on(false);
	}
	last_acc=power;
}
void GameCar::acc_idle() {
	physic_object->set_acc(0);
	last_acc=0;
}
void GameCar::brake(float power) {
	if(controls_locked) return;

	power=GameMath::clampf(power,0,1);

	if(physic_object->get_lon_vel()>0.001) {
		physic_object->set_brake(0.08);	//TODO
		brake_on(true);
		reverse_on(false);
	}
	else {
		physic_object->gear(0);
		physic_object->set_acc(power);
		brake_on(false);
		reverse_on(true);
	}
	last_acc=-power;
}

void GameCar::handbrake() {
	if(controls_locked) return;
	for(int i=0;i<physic_object->tyres.size();i++) {
		R2DTyre* t=physic_object->tyres[i];
		if(!t->steerable) {
			t->brake=1;
		}
	}
}

void GameCar::turn(float x) {
	if(controls_locked) return;
	x=GameMath::clampf(x,-1,1);
	physic_object->set_turn(x);

	last_turn=x;
}

void post_draw(DRenderNode* node,void* pclient,void *pcar) {
	DUserClient* client=(DUserClient*)pclient;
	GameCar* car=(GameCar*)pcar;

	/*
	client->color(0,1,0);
	client->moveTo(dvect(1,0.5));
	client->lineTo(dvect(-1,0.5));
	client->lineTo(dvect(-1,-0.5));
	client->lineTo(dvect(1,-0.5));
	client->lineTo(dvect(1,0.5));
	*/

	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
		if(t->sliding) {
			client->color(1,0,0);
		}
		else {
			client->color(0,1,0);
		}
		dvect p=cpVectToDvect(t->pos);
		float s=0.2;
		dvect p1=p-dvect(-s,-s);
		dvect p2=p-dvect(s,s);
		client->moveTo(p1);
		client->lineTo(dvect(p1.x,p2.y));
		client->lineTo(p2);
		client->lineTo(dvect(p2.x,p1.y));
		client->lineTo(p1);

		/*
		client->saveMatrix();
		client->transform(-car->getPos(),dvect(1,1),-car->getAngleDeg());

		p=cpVectToDvect(t->body->p);
		client->restoreMatrix();
		*/

	}

	client->color(1,1,1);
	client->lineTo(dvect(0,0));
	return;

	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
		client->color(1,1,0);
		client->moveTo(cpVectToDvect(t->pos));
		client->lineTo(cpVectToDvect(cpvadd(t->pos,cpvmult(t->vel,5000))));
	}
	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
		client->color(1,0,0);
		client->moveTo(cpVectToDvect(t->pos));
		client->lineTo(cpVectToDvect(cpvadd(t->pos,cpvmult(t->heading,10))));
		}
	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
		client->color(0,1,0);
		client->moveTo(cpVectToDvect(t->pos));
		client->lineTo(cpVectToDvect(cpvadd(t->pos,cpvmult(t->desired_pos,5000000))));
		}
	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
		client->color(0,0,1);
		client->moveTo(cpVectToDvect(t->pos));
		client->lineTo(cpVectToDvect(cpvadd(t->pos,cpvmult(t->out_force,1000000))));
	}

	for(int i=0;i<car->physic_object->tyres.size();i++) {
		R2DTyre* t=car->physic_object->tyres[i];
/*
		printf("Tyre %d: out force %f %f, desired pos %f %f, vel %f %f, heading %f %f, has contact %d\n",
				i,
				t->out_force.x,t->out_force.y,
				t->desired_pos.x,t->desired_pos.y,
				t->vel.x,t->vel.y,
				t->heading.x,t->heading.y,
				t->has_contact
				);
*/
	}
}

dvect GameCar::getHeadingVect() {
	return cpVectToDvect(physic_object->get_heading());
}
dvect GameCar::getPos() {
	//return dvect(render_node->pos.x,render_node->pos.y);
	dvect p;
	p.x=render_node->pos.x;
	p.y=render_node->pos.y;
	return p;
}
dvect GameCar::getPrevPos() {
	return cpVectToDvect(physic_object->get_prev_pos());
}
float GameCar::getAngleDeg() {
	return render_node->angle;
}
float GameCar::getSpeed() {
	return cpvlength(physic_object->get_vel());
}
dvect GameCar::getSpeedVect() {
	return cpVectToDvect(physic_object->get_vel());
}

dvect3 GameCar::getTotalSize() {
	return total_size;
}

void GameCar::brake_on(bool on) {
	for(vector<DRenderNode*>::iterator n=lights_brake.begin();n<lights_brake.end();n++)
		(*n)->visible=on;
	is_brake=on;
}
void GameCar::reverse_on(bool on) {
	for(vector<DRenderNode*>::iterator n=lights_reverse.begin();n<lights_reverse.end();n++)
		(*n)->visible=on;
}

void GameCar::sensorsDataInit() {
	for(int i=0;i<sensor_pixels;i++)
		sensor_data[i]=1;
}

void GameCar::sensorsInit(int num_pixels,float spread_angle,float length) {
	if(sensor_data) delete(sensor_data);
	sensor_data=new float[num_pixels];
	sensor_pixels=num_pixels;
	sensor_spread=spread_angle;
	sensor_length=length;
	sensor_enabled=true;
	sensorsDataInit();
}
void GameCar::sensorsEnable(bool on) {
	sensor_enabled=on;
}
float* GameCar::getSensorData() {
	return sensor_data;
}
int GameCar::getSensorPixels() {
	return sensor_pixels;
}
bool GameCar::getSensorEnabled() {
	return sensor_enabled;
}
float GameCar::getSensorSpread() {
	return sensor_spread;
}
float GameCar::getSensorLength() {
	return sensor_length;
}
void GameCar::sensorsUpdate() {
	/*
	for(int i=0;i<sensor_pixels;i++) {
		sensor_data[i]=rand_float(0,1);
	}
	*/
}
float GameCar::getSensorAngle(int index) {
	return -sensor_spread/2+(sensor_spread/(sensor_pixels-1))*index;
}
void GameCar::setControlsLocked(bool locked) {
	controls_locked=locked;
}

cpLayers GameCar::getLayers() {
	return physic_object->get_layers();
}

int GameCar::getLap() {
	return lap;
}
void GameCar::setLap(int _lap) {
	lap=_lap;
}
float GameCar::getScore() {
	return score;
}
void GameCar::setScore(float s) {
	score=s;
}
int GameCar::getRank() {
	return rank;
}
void GameCar::setRank(int r) {
	rank=r;
}

void GameCar::setFinalScore(float s) {
	final_score=s;
	is_done=true;
}
float GameCar::getFinalScore() {
	return final_score;
}
bool GameCar::isDone() {
	return is_done;
}
