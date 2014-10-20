
#include "drendernode.h"

#include <stdio.h>
#include <iostream>
using namespace std;

static glm::vec4 vec4zero=glm::vec4(0,0,0,1);
static glm::vec3 vec3up=glm::vec3(0,0,1);

void DRenderNode::init() {
	visible=true;
	object=NULL;
	scale=dvect3(1,1,1);
	post_draw=NULL;
	post_draw_data=NULL;
	alpha=1;
	dynamic=false;
	parent=NULL;
	force_render=false;
	//in frustrum by default because not all paths check
	//nested nodes for frustrum, only top-level
	in_frustrum=true;
	matrix=glm::mat4(1.0f);
}

DRenderNode::DRenderNode() {
	angle=0;
	init();
}
DRenderNode::DRenderNode(dvect _pos,double _angle) {
	init();
	set(_pos,_angle);
}
DRenderNode::DRenderNode(dvect3 _pos,double _angle) {
	init();
	set(_pos,_angle);
}

DRenderNode::~DRenderNode() {
	for(int i=0;i<childs.size();i++) {
		delete(childs[i]);
	}
	childs.clear();
}
void DRenderNode::addNode(DRenderNode* node) {
	childs.push_back(node);
}
void DRenderNode::removeNode(DRenderNode* node) {
	for(vector<DRenderNode*>::iterator it=childs.begin();it!=childs.end();it++) {
		if(*it==node) {
			childs.erase(it);
			break;
		}
	}
}
void DRenderNode::removeFromParent() {
	if(parent) {
		parent->removeNode(this);
	}
	parent=NULL;
}

vector<DRenderNode*> DRenderNode::getChilds() {
	return childs;
}
DRenderNode* DRenderNode::createNode(dvect pos,double angle) {
	DRenderNode* r=new DRenderNode(pos,angle);
	r->parent=this;
	r->force_render=force_render;
	addNode(r);
	return r;
}
DRenderNode* DRenderNode::createNode(dvect3 pos,double angle) {
	DRenderNode* r=new DRenderNode(pos,angle);
	r->parent=this;
	addNode(r);
	return r;
}
void DRenderNode::setPostDraw(void (*post_draw_func)(DRenderNode* node,void* client,void* data),void* user_data) {
	post_draw=post_draw_func;
	post_draw_data=user_data;
}
void DRenderNode::set(dvect _pos,double _angle) {
	pos.x=_pos.x;
	pos.y=_pos.y;
	pos.z=0;
	angle=_angle;
}
void DRenderNode::set(dvect3 _pos,double _angle) {
	pos=_pos;
	angle=_angle;
}
void DRenderNode::calcBB() {
	int merge_start=0;

	if(parent) {
		matrix=parent->matrix;
	}
	else {
		matrix=glm::mat4(1.0f);
	}

	matrix=glm::rotate<float>(
			glm::translate(matrix,glm::vec3(pos.x,pos.y,pos.z)),
			angle,glm::vec3(0,0,1));

	for(int i=0;i<childs.size();i++) {
		childs[i]->calcBB();
	}

	//apply object transformation to matrix
	if(object) {

		//calc BB
		glm::vec4 p1,p2,p3,p4;

		//exception for meshes
		if(object->type==TYPE_ROAD) {
			/*
			DBoundingBox* g_bb=&((DUserClientFreeGlutRoad*)node->object)->geometry->bb;
			p1=node->matrix*glm::vec4(g_bb->start.x,g_bb->start.y,0,1);
			p2=node->matrix*glm::vec4(g_bb->start.x,g_bb->end.y,0,1);
			p3=node->matrix*glm::vec4(g_bb->end.x,g_bb->start.y,0,1);
			p4=node->matrix*glm::vec4(g_bb->end.x,g_bb->end.y,0,1);
			*/
		}
		else {
			dvect size2=object->size/2;
			p1=matrix*glm::vec4(size2.x,size2.y,0,1);
			p2=matrix*glm::vec4(size2.x,-size2.y,0,1);
			p3=matrix*glm::vec4(-size2.x,-size2.y,0,1);
			p4=matrix*glm::vec4(-size2.x,size2.y,0,1);
		}

		/*
		bb.start=dvect(fmin(p1.x,fmin(p2.x,fmin(p3.x,p4.x))),
				fmin(p1.y,fmin(p2.y,fmin(p3.y,p4.y))));

		bb.end=dvect(fmax(p1.x,fmax(p2.x,fmax(p3.x,p4.x))),
				fmax(p1.y,fmax(p2.y,fmax(p3.y,p4.y))));
		*/
		bb.start.x=fmin(p1.x,fmin(p2.x,fmin(p3.x,p4.x)));
		bb.start.y=fmin(p1.y,fmin(p2.y,fmin(p3.y,p4.y)));

		bb.end.x=fmax(p1.x,fmax(p2.x,fmax(p3.x,p4.x)));
		bb.end.y=fmax(p1.y,fmax(p2.y,fmax(p3.y,p4.y)));

		glm::vec4 p=matrix*vec4zero;

		//ignore on road meshes
		//TODO: get rid of this
		if(object->type!=TYPE_ROAD && object->type!=TYPE_LIGHT) {
			matrix=
					glm::scale(

					glm::translate(
					glm::rotate<float>(matrix,
							180/*+object->angle*/,vec3up),

					glm::vec3(-object->size.x/2,
							-object->size.y/2,0)),

					glm::vec3(object->size.x,object->size.y,1));
		}
	}
	else if(childs.size()>0) {
		bb=childs[0]->bb;
		merge_start=1;
	}

	//nested BB
	for(int i=merge_start;i<childs.size();i++) {
		if(childs[i]->visible) {
			bb.merge(&childs[i]->bb);
		}
	}
}


