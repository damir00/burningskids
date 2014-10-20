#ifndef _DBOUNDINGBOX_H_
#define _DBOUNDINGBOX_H_

#include "dvect.h"

class DBoundingBox {
public:
	dvect start,end;

	DBoundingBox();
	DBoundingBox(dvect start,dvect end);

	bool intersects(DBoundingBox* box);
	bool intersects(dvect p1,dvect p2);
	bool contains(DBoundingBox* box);
	bool contains(dvect p1,dvect p2);
	bool contains(dvect point);
	void merge(DBoundingBox* box);
	void merge(dvect point);

	void build(dvect p1,dvect p2);	//sorts p1 and p2

	//expands for val on all sides
	void grow(float val);
	dvect getSize();
};

bool quad_contains(dvect start,dvect end,DBoundingBox *box);
bool quad_intersects(dvect start,dvect end,DBoundingBox *box);


#endif
