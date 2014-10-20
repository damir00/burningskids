
#ifndef _R2DSTRUCTS_H_
#define _R2DSTRUCTS_H_

#include "chipmunk.h"

struct R2DSurfaceQuery {
	cpVect pos;			//in
	float grip;			//out
	float roughness;
	float height;
};

//typedef int (*unary_func)(int);
typedef void (*surface_query_func)(R2DSurfaceQuery* query,void* data);

#endif

