
#ifndef _GAMEUTILS_H_
#define _GAMEUTILS_H_

#include "chipmunk.h"
#include "duserclient.h"

#include <stdlib.h>


#define PI 3.14159265
#define PI_M2 PI*2

float rand_float(float low,float high);
cpVect rand_vect(float x1,float x2,float y1,float y2);

cpVect dvectToCpVect(dvect vect);
dvect cpVectToDvect(cpVect vect);

//strings
string baseDir(string filename);

namespace GameMath {
	dvect lineClosest(dvect p1,dvect p2,dvect x);
	float lineClosestX(dvect p1,dvect p2,dvect x);
	dvect dvectlerp(dvect v1,dvect v2,float x);

	float clampf(float x,float low,float high);
}

#endif


