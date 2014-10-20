#include "duserclient.h"

/*
DQuadTree::DQuadTree(dvect start,dvect end,int depth) {
	if(depth==0) {
		childs=NULL;
		return;
	}
	dvect half=end-start;
	int depth2=depth-1;
	childs=new DQuadTree*[4];
	childs[0]=new DQuadTree(start,half,depth2);
	childs[1]=new DQuadTree(dvect(half.x,start.y),dvect(end.x,half.y),depth2);
	childs[2]=new DQuadTree(dvect(start.x,half.y),dvect(half.x,end.y),depth2);
	childs[3]=new DQuadTree(half,end,depth2);
}
DQuadTree::~DQuadTree() {
	if(childs) {
		delete(childs[0]);
		delete(childs[1]);
		delete(childs[2]);
		delete(childs[3]);
		delete[] childs;
	}
}

void DUserClient::quadtreeBuild(dvect size,int depth) {
	quadtreeReset();
	quadtree_root=new DQuadTree(-size/2,size/2,depth);
}
void DUserClient::quadtreeReset() {
	if(quadtree_root) {
		delete(quadtree_root);
		quadtree_root=NULL;
	}
}
*/


//client
DUserClient::DUserClient(DUserClientConfig _config) {
	config=_config;

	for(int i=0;i<256;i++) {
		keys[i]=false;
		prev_keys[i]=false;
	}

	for(int i=0;i<16;i++) {
		mouse_keys[i]=false;
		mouse_prev_keys[i]=false;
	}
	mouse_pos=dvect(0,0);

	root_node=new DRenderNode();
	root_node_overlay=new DRenderNode();
	root_node_overlay->force_render=true;

	cam_angle=0;

//	quadtree_root=NULL;
}
DUserClient::~DUserClient() {

}

bool DUserClient::supportsResourceType(DResourceType type) {
	return false;
}

DResource* DUserClient::loadResource(DResourceType type,string file) {
	return DResource::load(type,file);
}

DUserClientObject* DUserClient::loadObject(DResource* resource) {
	DUserClientObject * object=new DUserClientObject();
	objects.push_back(object);
	return object;
}
DUserClientObject* DUserClient::createObject(dvect _size) {
	return new DUserClientObject();
}

void DUserClient::unloadResource(DResource* resource) {
}

DRenderNode * DUserClient::getRootNode() {
	return root_node;
}
DRenderNode * DUserClient::getOverlayNode() {
	return root_node_overlay;
}

void DUserClient::saveMatrix() { }
void DUserClient::restoreMatrix() { }
void DUserClient::transform(dvect pos,dvect scale,double rot) { }
void DUserClient::transform(dvect3 pos,dvect3 scale,double rot) { }
void DUserClient::transformCam(dvect pos,dvect scale,double rot) { }
void DUserClient::camera(dvect pos,double angle) {
	cam_pos=pos;
	cam_angle=angle;
}

void DUserClient::update(long ts,duserclient_get_nodes get_nodes) {
	//TODO: replace with memcpy?
	for(int i=0;i<256;i++) {
		prev_keys[i]=keys[i];
	}
	for(int i=0;i<16;i++) {
		mouse_prev_keys[i]=mouse_keys[i];
	}

	render(ts,get_nodes);
}

void DUserClient::renderNode(DRenderNode* node) {
	if(!node->visible) return;

	saveMatrix();
	transform(node->pos,node->scale,node->angle);
	if(node->object) {
		renderObject(node->object);
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderNode(*it);
	}
	if(node->post_draw) {
		node->post_draw(node,this,node->post_draw_data);
	}

	restoreMatrix();
}

void DUserClient::render(long ts,duserclient_get_nodes get_nodes) {
	prerender();
	transformCam(-cam_pos,dvect(1,1),-cam_angle);

	for(int i=0;i<objects.size();i++) {
		renderObject(objects[i]);
	}

	renderNode(root_node);

	for(vector<DParticle*>::iterator it=particles.begin();it<particles.end();it++) {
		DParticle* particle=*it;
		particle->update(ts);
		if(particle->life<=0) {
			particles.erase(it);
		}
		else {
			renderParticle(particle);
		}
	}

	postrender();
}
void DUserClient::prerender() {
}
void DUserClient::postrender() {
}
void DUserClient::renderObject(DUserClientObject* obj) {
}
void DUserClient::renderParticle(DParticle* particle) {
}


//enviroment
void DUserClient::setAmbient(dvect3 ambient) {
	ambient_color=ambient;
	_setAmbient();
}
void DUserClient::setSunDir(dvect3 dir) {
	sun_dir=dir.normalize();
	_setSunDir();
}
void DUserClient::setSunColor(dvect3 color) {
	sun_color=color;
	_setSunColor();
}
void DUserClient::setCloudsResource(DResource* r) {
	clouds_resource=r;
}
void DUserClient::setCloudsScale(dvect scale) {
	clouds_scale=scale;
}
void DUserClient::setCloudsVel(dvect vel) {
	clouds_vel=vel;
}


//drawing
void DUserClient::color(float r,float g,float b,float a) {
	curr_color[0]=r;
	curr_color[1]=g;
	curr_color[2]=b;
	curr_color[3]=a;
}
void DUserClient::moveTo(dvect pos) {
	cursor=pos;
}
void DUserClient::lineTo(dvect to) {
	cursor=to;
}


//input
bool DUserClient::isKeyDown(int key) {
	return keys[key];
}
bool DUserClient::isKeyPressed(int key) {
	return (keys[key] && !prev_keys[key]);
}
dvect DUserClient::getMousePos() {
	return mouse_pos;
}
bool DUserClient::isMouseDown(int key) {
	return mouse_keys[key];
}
bool DUserClient::isMousePressed(int key) {
	return (mouse_keys[key] && !mouse_prev_keys[key]);
}


void DUserClient::addParticle(dvect pos,DParticles* generator) {
	particles.push_back(new DParticle(pos,generator));
}
/*
void DUserClient::nodeSetDynamic(DRenderNode* node) {	//call only once
	node->dynamic=true;
	dynamic_nodes.push_back(node);
}
void DUserClient::nodeRehash(DRenderNode* node) {		//call everytime static node is moved
	dynamic_nodes.push_back(node);
}
*/

