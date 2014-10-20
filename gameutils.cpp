
#include "gameutils.h"

float rand_float(float low,float high) {
	return (((float)rand())/RAND_MAX)*(high-low)+low;
}
cpVect rand_vect(float x1,float x2,float y1,float y2) {
	cpVect v={rand_float(x1,x2),rand_float(y1,y2)};
	return v;
}

cpVect dvectToCpVect(dvect vect) {
	cpVect v={vect.x,vect.y};
	return v;
}
dvect cpVectToDvect(cpVect vect) {
	return dvect(vect.x,vect.y);
}


string baseDir(string filename) {
	int pos1=filename.rfind("/");
	int pos2=filename.rfind("\\");

	int pos=pos1>pos2 ? pos1 : pos2;

	if(pos==-1) return filename;
	pos++;
	string r=filename;
	r.erase(pos,r.length()-pos);
	return r;
}

namespace GameMath {
	dvect lineClosest(dvect p1,dvect p2,dvect x) {
		      dvect v = p2-p1;
		      dvect w = x - p1;
		      //TODO: use dvect dot product
		      float c1=cpvdot(dvectToCpVect(w),dvectToCpVect(v));
		      if(c1<=0) return p1;

		      float c2=cpvdot(dvectToCpVect(v),dvectToCpVect(v));
		      if(c2<=c1) return p2;

		      float b = c1 / c2;
		      return p1 + v*b;
	}

	float lineClosestX(dvect p1,dvect p2,dvect x) {
		      dvect v = p2-p1;
		      dvect w = x - p1;
		      //TODO: use dvect dot product
		      float c1=cpvdot(dvectToCpVect(w),dvectToCpVect(v));
		      if(c1<=0) return 0;

		      float c2=cpvdot(dvectToCpVect(v),dvectToCpVect(v));
		      if(c2<=c1) return 1;

		      float b = c1 / c2;
		      return b;
	}
	dvect dvectlerp(dvect v1,dvect v2,float x) {
		dvect r=v2-v1;
		return v1+r*x;
	}
	float clampf(float x,float low,float high) {
		if(x<low) return low;
		if(x>high) return high;
		return x;
	}

}





