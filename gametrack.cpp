
#include "gametrack.h"

enum {
	GROUP_NORMAL=1,
	GROUP_RAMP=2
};

char QUAD_NODE_TYPE_OBJECT=1<<0;
char QUAD_NODE_TYPE_DECAL=1<<1;


void road_post_draw(DRenderNode* node,void* pclient,void *proad);

void surface_query(R2DSurfaceQuery* query,void* data) {
	GameTrack* t=(GameTrack*)data;
	t->_surfaceQuery(query);
}

GameTrackPosition::GameTrackPosition() {
	angle=0;
}
GameTrackPosition::GameTrackPosition(cpVect _pos,float _angle) {
	pos=_pos;
	angle=_angle;
}

GameTrack::GameTrack(GameTrackDesc* desc,GameGraphic* graphics,
		GameResourceManager* _manager,DRenderNode* parent) {
	manager=_manager;
	ground_image=NULL;
	sim=new R2DSim();
	parent_node=parent;
	render_node=parent->createNode();
	num_start_positions=0;
	num_cars=0;

	sim->set_query_func(surface_query,this);

	DResource *ground[4];
	ground[0]=manager->getImg(desc->colors[0]);
	ground[1]=manager->getImg(desc->colors[1]);
	ground[2]=manager->getImg(desc->colors[2]);
	ground[3]=manager->getImg(desc->colors[3]);
	DResource *ground_a=manager->getImg(desc->alpha);
	initGround(manager->client->createTerrain(ground,ground_a),desc->size);
	size=desc->size;
	dvect size2=size/2;

	int quadtree_levels=0;

	float x=max(size.x,size.y);
	while(x>5) {
		x/=2;
		quadtree_levels++;
	}

	printf("Quadtree depth: %d\n",quadtree_levels);
	quadtree=new DQuadTree(-size2,size2,quadtree_levels,NULL);

	for(vector<GameRoadDesc>::iterator i=desc->roads.begin();i!=desc->roads.end();i++) {
		GameRoadDesc* d=&(*i);
		addRoad(manager->client,new GameRoad(d),manager->getImg(i->color));
	}

	for(vector<GameTrackObject>::iterator i=desc->objects.begin();i!=desc->objects.end();i++) {
		GameTrackObject* o=&(*i);
		GameCarBody* body=manager->getBody(o->id);
		if(!body) {
			printf("Can't load object %s, skipping.\n",o->id.c_str());
			continue;
		}

		float scale=o->size_scale.x;

		GameCar* c=new GameCar(body,graphics,manager,scale);
		addObject(c,o->is_static);
	}


	for(int i=0;i<4;i++) {
		grips[i]=desc->grips[i];
		roughnesses[i]=desc->roughnesses[i];
	}

	float b=0.5;
	float f=1;
	sim->add_static_line(cpv(size2.x,size2.y),cpv(size2.x,-size2.y),b,f);
	sim->add_static_line(cpv(size2.x,size2.y),cpv(-size2.x,size2.y),b,f);
	sim->add_static_line(cpv(-size2.x,-size2.y),cpv(-size2.x,size2.y),b,f);
	sim->add_static_line(cpv(-size2.x,-size2.y),cpv(size2.x,-size2.y),b,f);

	//TODO: multiple update points
	update_radius=100;
	update_rad2=dvect(update_radius,update_radius)/2;

	lifetime_skids=7000;
	runtime=0;

	resource_skid=manager->getImg("media/skid01.png");
	//resource_skid=manager->getImg("media/car.png");
}
GameTrack::~GameTrack() {
	delete(sim);
	sim=NULL;

	parent_node->removeNode(render_node);
	delete(render_node);
	render_node=NULL;

	delete(quadtree);
	quadtree=NULL;

	for(int i=0;i<roads.size();i++) {
		delete(roads[i]);
	}
	roads.clear();

	for(int i=0;i<road_objects.size();i++) {
		delete(road_objects[i]);
	}
	road_objects.clear();

	for(int i=0;i<cars.size();i++) {
		delete(cars[i]);
	}
	cars.clear();

	for(int i=0;i<objects.size();i++) {
		delete(objects[i]);
	}
	objects.clear();

}
void GameTrack::setResource(DResource* r) {
	ground_image=r;
}
DResource* GameTrack::getResource() {
	return ground_image;
}

void GameTrack::addStartPosition(cpVect pos,float angle) {
	if(num_start_positions>=100) return;
	start_positions[num_start_positions++]=GameTrackPosition(pos,angle);
}
int GameTrack::getNumStartPositions() {
	return num_start_positions;
}
GameTrackPosition GameTrack::getStartPosition(int i) {
	return start_positions[i];
}

void GameTrack::setupObject(GameCar* obj,bool is_static) {
	sim->add_car(obj->physic_object,obj->physic_object->get_pos(),
			obj->physic_object->get_angle(),is_static);
	obj->physic_object->get_body()->data=obj;

	for(vector<GameShape>::iterator s=obj->shapes.begin();s<obj->shapes.end();s++) {
		switch(s->type) {
		case SHAPE_CIRCLE:
			sim->object_add_shape_circle(obj->physic_object,
					dvectToCpVect((s->start+s->end)/2),
					(s->start-s->end).length()/2.828427125,
					0,s->bounce,s->friction);
			break;
		case SHAPE_LINE:
			sim->object_add_shape_line(obj->physic_object,
					dvectToCpVect(s->start),
					dvectToCpVect(s->end),0,s->bounce,s->friction);
			break;
		case SHAPE_QUAD:
			sim->object_add_shape_quad(obj->physic_object,
					dvectToCpVect(s->start),
					dvectToCpVect(s->end),0,0,s->bounce,s->friction);
			break;
		case SHAPE_RAMP:
			sim->object_add_shape_quad(obj->physic_object,
					dvectToCpVect(s->start),
					dvectToCpVect(s->end),0,1,s->bounce,s->friction);
			break;
		}
	}
}

void GameTrack::addObject(GameCar* obj,bool is_static) {

	setupObject(obj,is_static);

	obj->update(manager, 0);	//get true pos and bb
	obj->quad_node_type=QUAD_NODE_TYPE_OBJECT;
	if(is_static) {
		quadtree->putNode(obj);
	}
	else {
		quadtree->putDynamicNode(obj);
	}
}

void GameTrack::addCar(GameCar* car) {

	setupObject(car,false);
	sim->object_wake(car->physic_object);

	//sim->add_car(car->physic_object,car->physic_object->get_pos(),car->physic_object->get_angle());
	//sim->car_add_shape_quad(car->physic_object,cpv(0,0),dvectToCpVect(car->getSize()),0);
	num_cars++;

	car->setLap(0);
	car->setScore(0);
	car->setRank(0);
	cars.push_back(car);

	car->quad_node_type=QUAD_NODE_TYPE_OBJECT;
	/*
	if(num_cars==1) {
		objects[0]->object->set_pos(dvectToCpVect(car->getPos()+car->getHeadingVect()*(2)));
		sim->object_attach(car->physic_object,objects[0]->object,cpv(-0.5*4,0),cpv(1*4,0));
	}
	*/
	quadtree->putDynamicNode(car);
	car->initEngine(manager);
}

void GameTrack::addRoad(DUserClient* client,GameRoad* road,DResource* texture) {
	DUserClientObject* obj=client->createRoad(road->geometry.getGeometry(),texture);
	DRenderNode* node=render_node->createNode();
	node->object=obj;
	node->angle=180;	//TODO: boo??
	//node->setPostDraw(road_post_draw,road);
	road_objects.push_back(obj);
	roads.push_back(road);
}


void GameTrack::_tree_wake_objects(GameCar* obj) {

	if(obj->physic_object->sleeping) {
		//printf("waking object\n");
		objects.push_back(obj);
		sim->object_wake(obj->physic_object);
	}
}

bool tree_wake_objects(DQuadTreeNode* node,void* track_p) {
	if(node->quad_node_type!=QUAD_NODE_TYPE_OBJECT) return false;

	((GameTrack*)track_p)->_tree_wake_objects((GameCar*)node);
	return false;
}

DRenderNode* GameTrack::getNewTyreSkid() {
	DUserClientFreeGlutCurve* curve=(DUserClientFreeGlutCurve*)manager->client->createCurve(
			resource_skid,0.7);

	DRenderNode* node=render_node->createNode(dvectzero,0);
	node->object=curve;
	node->force_render=true;

	return node;
}

//removed timed-out skids
void GameTrack::updateSkids() {
	while(skids.size()>0) {
		GameTrackDecal* decal=skids.front();
		if(decal->end_ts<runtime) {
			//delete
			manager->client->releaseCurve(decal->render_node->object);
			decal->do_delete=true;

			decal->render_node->removeFromParent();
			delete(decal->render_node);

			skids.erase(skids.begin());
		}
		else {
			break;
		}
	}
}

void GameTrack::update(long delta) {

	runtime+=delta;

	dvect update_start=update_position-update_rad2;
	dvect update_end=update_position+update_rad2;

	//printf("root has %d nodes\n",quadtree->dynamic_nodes.size());

	sim->update(delta);
	for(int i=0;i<objects.size();i++) {
		GameCar* obj=objects[i];

		if(!quad_intersects(update_start,update_end,&obj->bb)) {
			//printf("putting object to sleep\n");
			sim->object_sleep(obj->physic_object);
			objects.erase(objects.begin()+i);
			i--;
			continue;
		}

		obj->update(manager, delta);
		if(!obj->physic_object->is_static())
			DQuadTree::updateNode(obj);
	}

	GameRoad* road=NULL;
	if(roads.size()>0) road=roads[0];

	for(vector<GameCar*>::iterator it=cars.begin();it<cars.end();it++) {
		GameCar* c=(*it);

		if(road) {
			c->road_closest_prev=c->road_closest;

			//int index=c->road_closest_prev.local_pos*
			//		road->geometry.metapoints.size()/road->geometry.app_len-1;

			int index=c->road_closest_prev.index;

			//int i1=max(0,index-3);
			//int i2=min(index+3,(int)road->geometry.metapoints.size()-1);

			int num_points=road->geometry.metapoints.size();
			int i1=(index-3); //%num_points;
			int i2=(index+3); //%num_points;

			c->road_closest=road->geometry.getClosest(c->getPos(),i1,i2);
		}

		updateCarSensors(c);
		c->update(manager, delta);

		//skids
		for(int i=0;i<c->physic_object->tyres.size();i++) {

			R2DTyre* t=c->physic_object->tyres[i];
			if(!t->sliding) {
				if(c->tyre_skids[i]) {

					DRenderNode* node=c->tyre_skids[i];
					DUserClientFreeGlutCurve* curve=(DUserClientFreeGlutCurve*)c->tyre_skids[i]->object;

					node->removeFromParent();
					node->bb=curve->bb;
					node->force_render=false;
					GameTrackDecal* decal=new GameTrackDecal();
					decal->type=DECAL_TYPE_CURVE;
					decal->render_node=node;
					decal->bb=node->bb;
					quadtree->putNode(decal);
					skids.push_back(decal);
					decal->end_ts=runtime+lifetime_skids;

					c->tyre_skids[i]=0;
				}
				continue;
			}

			DUserClientFreeGlutCurve* curve;
			if(!c->tyre_skids[i]) {

				c->tyre_skids[i]=getNewTyreSkid();
				curve=(DUserClientFreeGlutCurve*)c->tyre_skids[i]->object;
				curve->add_segment(cpVectToDvect(t->prev_world_pos));

				c->tyre_skids[i]->bb=curve->bb;
			}
			else {
				curve=(DUserClientFreeGlutCurve*)c->tyre_skids[i]->object;
				DRenderNode* node=c->tyre_skids[i];

				dvect new_pos=cpVectToDvect(t->prev_world_pos);
				if((curve->prev_pos-new_pos).length()>3) {
					if(!curve->add_segment(new_pos)) {

						//move to quadtree
						node->removeFromParent();
						node->force_render=false;
						GameTrackDecal* decal=new GameTrackDecal();
						decal->type=DECAL_TYPE_CURVE;
						decal->render_node=node;
						decal->bb=node->bb;
						decal->end_ts=runtime+lifetime_skids;
						quadtree->putNode(decal);
						skids.push_back(decal);

						c->tyre_skids[i]=getNewTyreSkid();
						curve=(DUserClientFreeGlutCurve*)c->tyre_skids[i]->object;
						curve->add_segment(new_pos);
					}
				}
				else {
					if(curve->current_size==0) {
						curve->add_segment(new_pos);
					}
					else {
						curve->update_last_segment(new_pos);
					}
				}
				node->bb=curve->bb;
			}
		}

		DQuadTree::updateNode(c);
	}

	updateSkids();
	quadtree->iterateNodes(update_start,update_end,tree_wake_objects,this);


	//laps
	if(roads.size()>0) {
		if(roads[0]->cyclic) {
			float p1=roads[0]->geometry.approxLength()*0.2;
			float p2=roads[0]->geometry.approxLength()*0.8;

			for(vector<GameCar*>::iterator it=cars.begin();it<cars.end();it++) {
				GameCar* c=(*it);
				float prog1=c->road_closest_prev.local_pos;
				float prog2=c->road_closest.local_pos;
				if(prog1>p2 && prog2<p1) {
					c->setLap(c->getLap()+1);
				}
				else if(prog2>p2 && prog1<p1) {
					c->setLap(c->getLap()-1);
				}
			}
		}
		else {	//not cyclic
			float p1=roads[0]->geometry.approxLength()*0.99;
			for(vector<GameCar*>::iterator it=cars.begin();it<cars.end();it++) {
				GameCar* c=(*it);
				float prog=c->road_closest.local_pos;
				if(prog>=p1) {
					c->setLap(1);
				}
			}
		}
	}
	//ranks
	for(int i=0;i<cars.size();i++) {
		int r=0;
		for(int n=i+1;n<cars.size();n++) {

			if(cars[i]->isDone()) {
				if(cars[n]->isDone() &&
						cars[n]->getFinalScore()>cars[i]->getFinalScore())
					r++;
			}
			else {
				if(cars[n]->getScore()>cars[i]->getScore() || cars[n]->isDone()) {
					r++;
				}
			}
		}
		cars[i]->setRank(r);
	}
}

void GameTrack::initGround(DUserClientObject* obj,dvect size) {
	obj->size=size;
	render_node->object=obj;
}

void GameTrack::_surfaceQuery(R2DSurfaceQuery* query) {
	query->grip=grips[0];
	dvect pos=cpVectToDvect(query->pos);

	query->height=0;

	for(vector<GameRoad*>::iterator i=roads.begin();i!=roads.end();i++) {
		if((*i)->pointInside(pos)) {
			query->grip=(*i)->surface_grip;
			query->roughness=(*i)->surface_roughness;
			break;
		}
	}

	//TODO: hax!
	cpShape *s=cpSpacePointQueryFirst(sim->get_space(), query->pos, 0x8000, 0);
	if(s) {
		cpVect local_pos=cpBodyWorld2Local(s->body,query->pos);

		static const dvect p_offset=dvect(0.5,0.5);

		GameCar* obj=(GameCar*)s->body->data;
		if(obj) {
			dvect local_p=cpVectToDvect(local_pos);
			dvect size=(obj->shapes[0].end-obj->shapes[0].start);
			local_p=local_p/size+p_offset;
			query->height=(1-local_p.x);

			/*
			printf("world %f %f local %f %f\n",
					query->pos.x,query->pos.y,
					local_p.x,local_p.y);
			*/
		}
		else {
			query->height=0;
		}
	}

	//query->height=fmax(cos(query->pos.x/5),0);
	//query->height=((float)((int)(query->pos.x*10)%10))/10;
	/*
	if(query->pos.x>0 && query->pos.x<10) {
		query->height=query->pos.x/10;
	}
	else if (query->pos.x>=10 && query->pos.x<13) {
		query->height=(13-query->pos.x)/3;
	}
	else {
		query->height=0;
	}
	*/
}
dvect GameTrack::surfaceQuery2(dvect pos) {
	for(vector<GameRoad*>::iterator i=roads.begin();i!=roads.end();i++) {
		if((*i)->pointInside(pos)) {
			return dvect((*i)->surface_grip,(*i)->surface_roughness);
		}
	}
	return dvectzero;
}


void road_post_draw(DRenderNode* node,void* pclient,void *proad) {
	DUserClient* client=(DUserClient*)pclient;
	GameRoad* road=(GameRoad*)proad;

	GameRoadGeometry *geom=&road->geometry;

	static float points_per_unit=5;

	float len=geom->approxLength();
	int points=len/points_per_unit;

	client->color(1,0,0);

	client->saveMatrix();
	client->transform(dvectzero,dvect(1,1),180);

	for(int i=0;i<points;i++) {
		float pos=((float)i)/((float)points)*len;
		GameRoadGeometryMetapoint mp=geom->getMeta(pos);
		dvect pos_left=mp.center+mp.left;
		dvect pos_right=mp.center-mp.left;

		client->moveTo(pos_left);
		client->lineTo(pos_right);
	}

	for(int i=0;i<geom->points.size();i++) {
		GameRoadGeometryPoint p=geom->points[i];
		
		client->color(0,1,0);
		client->moveTo(p.pos);
		client->lineTo(p.pos+p.control_1);

		client->color(0,0,1);
		client->moveTo(p.pos);
		client->lineTo(p.pos+p.control_2);
	}

	client->color(1,1,0);
	for(int i=0;i<geom->metapoints.size()-1;i++) {
		client->moveTo(geom->metapoints[i].center+geom->metapoints[i].left);
		client->lineTo(geom->metapoints[i].center);
		client->lineTo(geom->metapoints[i+1].center);
	}

	client->restoreMatrix();
}
dvect GameTrack::getSize() {
	return size;
}
unsigned int GameTrack::getRoadSize() {
	return roads.size();
}
GameRoad* GameTrack::getRoad(unsigned int i) {
	return roads[i];
}

void GameTrack::updateCarSensors(GameCar* car) {
	if(!car->getSensorEnabled()) return;
	float* data=car->getSensorData();
	float spread=car->getSensorSpread();
	int pixels=car->getSensorPixels();
	float len=car->getSensorLength();

	cpSpace* space=sim->get_space();
	cpBody* body=car->physic_object->get_body();

	for(int i=0;i<pixels;i++) {
		float angle=car->getSensorAngle(i);

		float y_offset=0.8; //fabs(angle)/45;

		cpVect start={0,y_offset};
		cpVect end=cpvmult(cpvforangle(degToRad(angle)),len);

		start=cpBodyLocal2World(body,start);
		end=cpBodyLocal2World(body,end);

		cpSegmentQueryInfo info;
		cpSpaceSegmentQueryFirst(space,start,end,car->getLayers(),CP_NO_GROUP,&info);

		data[i]=info.t;
	}

}

bool iterate_tree_nodes(DQuadTreeNode* node,void* track_p) {
	GameTrack* track=(GameTrack*)track_p;

	if(node->quad_node_type==QUAD_NODE_TYPE_OBJECT) {
		track->iterated_nodes.push_back(((GameCar*)node)->render_node);
	}
	else {
		GameTrackDecal* decal=(GameTrackDecal*)node;
		if(decal->end_ts!=-1 && decal->end_ts<track->getRuntime() &&
				decal->type!=DECAL_TYPE_CURVE) {
			//remove decal
			//curves get removed elsewhere
			decal->render_node->removeFromParent();

			delete(decal->render_node);
			//don't delete decal - it is owned by quadtree
			return true;
		}
		track->iterated_nodes.push_back(decal->render_node);
	}
	return false;
}

void GameTrack::iterateTreeNodes(DBoundingBox* box) {
	iterated_nodes.clear();
	quadtree->iterateNodes(box->start,box->end,iterate_tree_nodes,this);
}


void GameTrack::addDecal(DUserClientObject* object,dvect pos,float rot) {
	DRenderNode* node=new DRenderNode();
	node->pos=dvect3(pos,0);
	node->angle=rot;
	node->object=object;
	node->calcBB();

	GameTrackDecal* decal=new GameTrackDecal();
	decal->render_node=node;
	decal->bb=node->bb;

	quadtree->putNode(decal);
}

void GameTrack::addLight(DResource* resource,dvect pos,dvect size,float rot) {
	DUserClientObject* obj=manager->client->createLight(resource,size);
	addDecal(obj,pos,rot);
}
void GameTrack::addSimpleDecal(DResource* resource,DUserClientObjectRenderFlags render_flag,
		dvect pos,dvect size,float rot) {
	DResource* color=0;
	DResource* normal=0;
	DResource* height=0;
	DResource* specular=0;
	if(render_flag & RENDER_FLAG_COLOR)		color=resource;
	if(render_flag & RENDER_FLAG_NORMAL)	normal=resource;
	if(render_flag & RENDER_FLAG_HEIGHT)	height=resource;
	if(render_flag & RENDER_FLAG_SPECULAR)	specular=resource;

	//DUserClientObject* obj=manager->client->createCar(color,normal,height,size);

	DUserClientObject* obj=manager->client->createObject(size);

	{
		DUserClientFreeGlutObject* fg_obj=(DUserClientFreeGlutObject*)obj;
		fg_obj->addTexture(resource);

		static DGLShader* shader=0;

		if(!shader) {
			shader=new DGLShader();
			if(!shader->compileFullFromFile(
					"shader/base_vert.glsl",
					"shader/texture_frag.glsl")) {
				printf("Decal shader failed\n");
			}
		}

		fg_obj->shader=shader;
		fg_obj->setFloat4(shader->getUniform("color"),
				rand_float(0,1),
				rand_float(0,1),
				rand_float(0,1),1);
	}

	obj->render_flags=render_flag;

	addDecal(obj,pos,rot);
}

long GameTrack::getRuntime() {
	return runtime;
}


