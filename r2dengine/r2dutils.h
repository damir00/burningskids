
#ifndef _R2DUTILS_H_
#define _R2DUTILS_H_

#include "chipmunk.h"

class R2DUtils {
	public:

	static cpShape* space_add_shape(cpSpace* space,cpShape* shape,bool is_static) {
		if(is_static)
			return cpSpaceAddStaticShape(space, shape);
		return cpSpaceAddShape(space, shape);
	}

	static cpShape* shape_quad(cpBody* body,cpVect size,cpVect offset) {
		size.x/=2;
		size.y/=2;
	        cpVect verts[] = {
		        cpv(-size.x,-size.y),
		        cpv(-size.x, size.y),
		        cpv( size.x, size.y),
		        cpv( size.x,-size.y),
		};
		//return space_add_shape(space, cpPolyShapeNew(body, 4, verts, offset),is_static);
	    return cpPolyShapeNew(body, 4, verts, offset);
	}
	static cpShape* shape_rect(cpBody* body,cpVect start,cpVect end) {
	        cpVect verts[] = {
		        cpv(start.x,start.y),
		        cpv(start.x,  end.y),
		        cpv(  end.x,  end.y),
		        cpv(  end.x,start.y),
		};
		//return space_add_shape(space, cpPolyShapeNew(body, 4, verts, cpvzero),is_static);
	    return cpPolyShapeNew(body, 4, verts, cpvzero);
	}
	static cpShape* shape_circle(cpBody* body,float r,cpVect offset) {
		//return space_add_shape(space,cpCircleShapeNew(body,r,offset),is_static);
		return cpCircleShapeNew(body,r,offset);
	}
	static cpShape* shape_line(cpBody* body,cpVect start,cpVect end) {
		//return space_add_shape(space, cpSegmentShapeNew(body,start,end,0.1),is_static);
		return cpSegmentShapeNew(body,start,end,0.1);
	}
	static cpVect getBodyPointVel(cpBody* body,cpVect point) {
		return cpvmult(cpvperp(point),body->w);
	}

	static float abs(float x) {
		if(x<0) return -x;
		return x;
	}

	static float capf(float x,float low,float high) {
		if(x<low) return low;
		if(x>high) return high;
		return x;
	}
};


#endif

