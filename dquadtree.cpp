#include "dquadtree.h"
#include <assert.h>
#include <stdio.h>

using namespace std;


//at depth=0, quadtree will not subdivide
DQuadTree::DQuadTree(dvect start, dvect end,int _depth,DQuadTree* _parent) {
	bb=DBoundingBox(start,end);
	center=(bb.start+bb.end)*0.5;

	childs[0]=0;
	childs[1]=0;
	childs[2]=0;
	childs[3]=0;
	depth=_depth;
	parent=_parent;

	iteration_id=0;
}
DQuadTree::~DQuadTree() {
	delete(childs[0]);
	delete(childs[1]);
	delete(childs[2]);
	delete(childs[3]);
}

void DQuadTree::getChildPos(int i,dvect* p1,dvect* p2) {
	switch(i) {
	case 0:
		*p1=bb.start;
		*p2=(bb.start+bb.end)*0.5;
		return;
	case 1: {
		*p1=dvect(center.x,bb.start.y);
		*p2=dvect(bb.end.x,center.y);
		return;
	}
	case 2: {
		*p1=dvect(bb.start.x,center.y);
		*p2=dvect(center.x,bb.end.y);
		return;
	}
	case 3:
		*p1=(bb.start+bb.end)*0.5;
		*p2=bb.end;
		return;
	}
}

bool DQuadTree::checkChildPut(int i,DQuadTreeNode* node) {
	//assume depth>0
	if(!childs[i]) {
		dvect p1,p2;
		getChildPos(i,&p1,&p2);
		if(quad_intersects(p1,p2,&node->bb)) {
			childs[i]=new DQuadTree(p1,p2,depth-1,this);
			childs[i]->putNode(node);
			return true;
		}
		return false;
	}
	if(childs[i]->bb.intersects(&node->bb)) {
		childs[i]->putNode(node);
		return true;
	}
	return false;
}

void DQuadTree::putNode(DQuadTreeNode* node) {
	//assume our BB contains node BB
	if(depth>0) {
		for(int i=0;i<4;i++) {
			if(checkChildPut(i,node)) {
		//		printf("added node to child %d, depth %d\n",i,depth);
		//		return;
			}
		}
	}
	else {
		addNode(node);
	}
}

void DQuadTree::removeStaticNode(int index) {
	DQuadTreeNode* n=static_nodes[index];
	static_nodes.erase(static_nodes.begin()+index);
	n->refcount--;
	if(n->refcount<=0) {
		delete(n);
	}
}
void DQuadTree::iterateNodes(dvect point,dquadtree_iterate func,void* data) {
	if(!bb.contains(point)) return;

	if(parent) {
		iteration_id=parent->iteration_id;
	}
	else {
		iteration_id++;
	}

	for(int i=0;i<4;i++) {
		if(childs[i]) childs[i]->iterateNodes(point,func,data);
	}

	for(int i=0;i<static_nodes.size();i++) {
		DQuadTreeNode* n=static_nodes[i];

		if(n->do_delete) {
			removeStaticNode(i);
			i--;
			continue;
		}

		if(n->last_iteration_id==iteration_id) continue;
		n->last_iteration_id=iteration_id;
		if(func(n,data)) {
			n->do_delete=true;
			removeStaticNode(i);
			i--;
			continue;
		}
	}
	for(int i=0;i<dynamic_nodes.size();i++) {
		DQuadTreeNode* n=dynamic_nodes[i];

		if(n->last_iteration_id==iteration_id) continue;
		n->last_iteration_id=iteration_id;
		func(n,data);
	}
}

void DQuadTree::iterateNodes(dvect start,dvect end,dquadtree_iterate func,void* data) {
	if(!bb.intersects(start,end)) return;

	if(parent) {
		iteration_id=parent->iteration_id;
	}
	else {
		iteration_id++;
	}

	for(int i=0;i<4;i++) {
		if(childs[i]) childs[i]->iterateNodes(start,end,func,data);
	}
	for(int i=0;i<static_nodes.size();i++) {
		DQuadTreeNode* n=static_nodes[i];

		if(n->do_delete) {
			removeStaticNode(i);
			i--;
			continue;
		}

		if(n->last_iteration_id==iteration_id) continue;
		if(!quad_intersects(start,end,&n->bb)) continue;

		if(n->last_iteration_id!=iteration_id-1) {
			//printf("Object appeared\n");
		}

		n->last_iteration_id=iteration_id;
		if(func(n,data)) {
			n->do_delete=true;
			removeStaticNode(i);
			i--;
			continue;
		}
	}
	for(int i=0;i<dynamic_nodes.size();i++) {
		DQuadTreeNode* n=dynamic_nodes[i];
		if(n->last_iteration_id==iteration_id) continue;
		if(!quad_intersects(start,end,&n->bb)) continue;

		if(n->last_iteration_id!=iteration_id-1) {
			//printf("Object appeared\n");
		}

		n->last_iteration_id=iteration_id;
		func(n,data);
	}
}

void DQuadTree::addNode(DQuadTreeNode* node) {
	static_nodes.push_back(node);
	node->refcount++;
}

//recursion only upwards
bool DQuadTree::reinsertDynamicNode(DQuadTreeNode* node,DQuadTree* child) {
	//if child==0, we hold the node currently

	if(!bb.contains(&node->bb)) {
		if(parent) {
			//one of the higher nodes should accept it
			if(child==0) {
				removeDynamicNode(node);
			}
			return parent->reinsertDynamicNode(node,this);
		}
		if(child!=0) {
			//called from child
			//root node, add
			addDynamicNode(node);
			return true;
		}
		//called from outside, root node, no changes
		return true;
	}

	//bb contains node
	for(int i=0;i<4;i++) {
		if(child==0 || childs[i]!=child) {
			if(checkChildPutDynamic(i,node)) {
				removeDynamicNode(node);
				return true;
			}
		}
	}

	if(child) {
		addDynamicNode(node);
	}
	//no child accepted the node
	return true;
}

void DQuadTree::removeDynamicNode(DQuadTreeNode* node) {
	for(int i=0;i<dynamic_nodes.size();i++) {
		if(dynamic_nodes[i]==node) {
			dynamic_nodes.erase(dynamic_nodes.begin()+i);
			break;
		}
	}
}

//dynamic
void DQuadTree::putDynamicNode(DQuadTreeNode* node) {
	//assume our BB contains node BB
	if(depth>0) {
		for(int i=0;i<4;i++) {
			if(checkChildPutDynamic(i,node)) {
				return;
			}
		}
	}
	addDynamicNode(node);
}

void DQuadTree::addDynamicNode(DQuadTreeNode* node) {
#ifdef NDEBUG
	for(int i=0;i<dynamic_nodes.size();i++) {
		assert(dynamic_nodes[i]!=node);
	}
#endif

	dynamic_nodes.push_back(node);
	node->tree_ptr=this;
}

bool DQuadTree::checkChildPutDynamic(int i,DQuadTreeNode* node) {
	//assume depth>0
	if(!childs[i]) {
		dvect p1,p2;
		getChildPos(i,&p1,&p2);
		if(quad_contains(p1,p2,&node->bb)) {
			childs[i]=new DQuadTree(p1,p2,depth-1,this);
			childs[i]->putDynamicNode(node);
			return true;
		}
		return false;
	}
	if(childs[i]->bb.contains(&node->bb)) {
		childs[i]->putDynamicNode(node);
		return true;
	}
	return false;
}

void DQuadTree::updateNode(DQuadTreeNode* node) {
	DQuadTree *t=(DQuadTree*)node->tree_ptr;
	if(!t) {
//		printf("WARN: node not in tree\n");
		return;
	}
	t->reinsertDynamicNode(node,0);
}
