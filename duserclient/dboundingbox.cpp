
#include "dboundingbox.h"

DBoundingBox::DBoundingBox() {}
DBoundingBox::DBoundingBox(dvect _start,dvect _end) {
	start=_start;
	end=_end;
}
void DBoundingBox::build(dvect p1,dvect p2) {
	start=p1;
	end=p1;
	merge(p2);
}

bool DBoundingBox::intersects(DBoundingBox* box) {
	return (start.x<box->end.x && end.x>box->start.x &&
			start.y<box->end.y && end.y>box->start.y);
}
bool DBoundingBox::intersects(dvect p1,dvect p2) {
	return (start.x<p2.x && end.x>p1.x &&
			start.y<p2.y && end.y>p1.y);
}
bool DBoundingBox::contains(DBoundingBox* box) {
	return (start.x<box->start.x && end.x>box->end.x &&
			start.y<box->start.y && end.y>box->end.y);
}
bool DBoundingBox::contains(dvect p1,dvect p2) {
	return (start.x<p1.x && end.x>p2.x &&
			start.y<p1.y && end.y>p2.y);
}
bool DBoundingBox::contains(dvect point) {
	return (start.x<point.x && end.x>point.x &&
			start.y<point.y && end.y>point.y);
}

void DBoundingBox::merge(DBoundingBox* box) {
	start.x=fmin(start.x,box->start.x);
	start.y=fmin(start.y,box->start.y);
	end.x=fmax(end.x,box->end.x);
	end.y=fmax(end.y,box->end.y);
}
void DBoundingBox::merge(dvect point) {
	start.x=fmin(start.x,point.x);
	start.y=fmin(start.y,point.y);
	end.x=fmax(end.x,point.x);
	end.y=fmax(end.y,point.y);
}
void DBoundingBox::grow(float val) {
	start.x-=val;
	start.y-=val;
	end.x+=val;
	end.y+=val;
}
dvect DBoundingBox::getSize() {
	return (end-start);
}

bool quad_contains(dvect start,dvect end,DBoundingBox *box) {
	return (start.x<box->start.x && end.x>box->end.x &&
			start.y<box->start.y && end.y>box->end.y);
}
bool quad_intersects(dvect start,dvect end,DBoundingBox* box) {
	return (start.x<box->end.x && end.x>box->start.x &&
			start.y<box->end.y && end.y>box->start.y);
}
