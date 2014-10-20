
#ifndef _DUSERCLIENTOBJECT_H_
#define _DUSERCLIENTOBJECT_H_

#include "dvect.h"
#include "duserclientgeometry.h"

enum DUserClientObjectRenderFlags {
	RENDER_FLAG_COLOR=1<<0,
	RENDER_FLAG_NORMAL=1<<1,
	RENDER_FLAG_HEIGHT=1<<2,
	RENDER_FLAG_SPECULAR=1<<3
};

class DUserClientObject {
	//int resource;
public:
	//dvect pos;
	dvect size;
	int type;
	char render_flags;	//color,normal,height,shadow, ...
	DUserClientGeometry* geometry;

	DUserClientObject();
	~DUserClientObject();
};

#endif

