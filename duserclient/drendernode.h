
#ifndef _DRENDERNODE_H_
#define _DRENDERNODE_H_

#include "dvect3.h"
#include "dresource.h"
#include "duserclientobject.h"
#include "dboundingbox.h"

//needed for BB calculations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

using namespace std;

//TODO: move
enum DUserClientObjectType {
	TYPE_OBJECT,
	TYPE_CAR,
	TYPE_TERRAIN,
	TYPE_LIGHT,
	TYPE_ROAD,
	TYPE_CURVE
};

class DRenderNode {
	void init();
public:
	void (*post_draw)(DRenderNode* node,void* client,void* data);
	void* post_draw_data;
	DRenderNode* parent;
	vector<DRenderNode*> childs;
	DUserClientObject * object;
	dvect3 pos;
	dvect3 scale;
	double angle;
	float alpha;
	bool visible;
	bool dynamic;
	bool in_frustrum;
	bool force_render;	//don't check for frustrum

	glm::mat4 matrix;	//absolute matrix
	DBoundingBox bb;	//absolute

	DRenderNode();
	DRenderNode(dvect pos,double angle);
	DRenderNode(dvect3 pos,double angle);
	~DRenderNode();

	//add child node. This node has ownership over child node
	//until it gets removed.
	//when node is destroyed it destroys all it's children
	void addNode(DRenderNode* node);
	void removeFromParent();
	void removeNode(DRenderNode* node);
	DRenderNode* createNode(dvect pos=dvect(0,0),double angle=0);
	DRenderNode* createNode(dvect3 pos,double angle=0);

	vector<DRenderNode*> getChilds();

	void set(dvect pos,double angle);
	void set(dvect3 pos,double angle);

	void setPostDraw(void (*post_draw)(DRenderNode* node,void* client,void* data),void* user_data);
	void calcBB();
};

#endif



