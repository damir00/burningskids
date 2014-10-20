
#include "gameobject.h"

GameObject::GameObject(DUserClientObject* obj,DRenderNode* parent,dvect pos,float angle) {
	node=parent->createNode();
	node->object=obj;
	node->pos=dvect3(pos,0);
	node->angle=angle;
	object=new R2DObject(DYNAMIC,0.5,0.5);
	object->set_pos(dvectToCpVect(pos));
	object->set_angle(degToRad(angle));
}
GameObject::GameObject(GameCarBody* body,DRenderNode* root_node,GameResourceManager* manager) {
	node=root_node->createNode();
	node->pos=dvect3(0,0,0);
	node->angle=0;
	object=new R2DObject(DYNAMIC,body->mass,body->mass_interia);
	object->set_pos(cpvzero);
	object->set_angle(0);

	for(vector<GameObjectPart>::iterator p=body->parts.begin();p<body->parts.end();p++) {

		if(p->render) {
			DRenderNode* n=node->createNode(p->pos,p->angle);
			n->object=manager->client->createCar(
					manager->getImg(p->color_filename),
					manager->getImg(p->normal_filename),
					manager->getImg(p->height_filename),
					dvect(p->size.x,p->size.y));
		}
		if(p->shape_type!=SHAPE_NONE) {
			dvect s=dvect(p->size.x,p->size.y)/2;
			GameShape circle_s=GameShape(p->shape_type,-s,s);
			shapes.push_back(circle_s);
		}
		object->height=p->size.z;
	}
}

GameObject::~GameObject() {
	node->removeFromParent();
	delete(node);
	node=NULL;
}

void GameObject::update(long delta) {
	node->pos=dvect3(cpVectToDvect(object->get_pos()),0);
	node->angle=radToDeg(object->get_angle());
}

void GameObject::setAngle(float a) {
	node->angle=a;
	object->set_angle(degToRad(a));
}
float GameObject::getAngle() {
	return node->angle;
}
void GameObject::setPos(dvect pos) {
	node->pos=dvect3(pos,0);
	object->set_pos(dvectToCpVect(pos));
}
dvect GameObject::getPos() {
	return dvect(node->pos.x,node->pos.y);
}

