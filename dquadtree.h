#ifndef _DQUADTREE_H_
#define _DQUADTREE_H_

#include "dvect.h"
#include "dboundingbox.h"

#include <vector>

class DQuadTreeNode {
public:
	int last_iteration_id;
	void* tree_ptr;
	DBoundingBox bb;
	char quad_node_type;		//user defined type
	//will mark node for deletion.
	//marked node will never be returned and will be removed from leafs
	//when found. for static nodes only
	bool do_delete;
	short refcount;	//by how many leafs the node is owned

	DQuadTreeNode() {
		last_iteration_id=-1;
		tree_ptr=0;
		quad_node_type=0;
		do_delete=false;
		refcount=0;
	}
};

//return true to remove from elements
typedef bool (*dquadtree_iterate)(DQuadTreeNode* node,void* data);

class DQuadTree {
	DQuadTree* parent;
	DQuadTree* childs[4];
	std::vector<DQuadTreeNode*> static_nodes;

	int depth;
	int iteration_id;

	void getChildPos(int i,dvect* p1,dvect* p2);
	void addNode(DQuadTreeNode* node);
	bool checkChildPut(int i,DQuadTreeNode* node);

	void addDynamicNode(DQuadTreeNode* node);
	void removeDynamicNode(DQuadTreeNode* node);

	void removeStaticNode(int index);
public:
	std::vector<DQuadTreeNode*> dynamic_nodes;
	DBoundingBox bb;
	dvect center;
	//at depth=0, quadtree will not subdivide
	DQuadTree(dvect start, dvect end,int depth,DQuadTree* parent=0);
	~DQuadTree();

	//when static DQuadTreeNode is added, it is owned by the tree and should
	//never be freed from outside
	void putNode(DQuadTreeNode* node);
	//returns exactly all nodes intersecting with (start,end)
	void iterateNodes(dvect start,dvect end,dquadtree_iterate func,void* data);
	//returns all nodes in the same leaf as point
	void iterateNodes(dvect point,dquadtree_iterate func,void* data);

	bool reinsertDynamicNode(DQuadTreeNode* node,DQuadTree* child);
	void putDynamicNode(DQuadTreeNode* node);
	bool checkChildPutDynamic(int i,DQuadTreeNode* node);

	static void updateNode(DQuadTreeNode* node);
};

#endif

