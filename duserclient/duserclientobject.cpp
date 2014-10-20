#include "duserclientobject.h"

DUserClientObject::DUserClientObject() {
//	angle=0;
	geometry=NULL;

	render_flags=
		RENDER_FLAG_COLOR |
		RENDER_FLAG_NORMAL |
		RENDER_FLAG_HEIGHT |
		RENDER_FLAG_SPECULAR;
}
DUserClientObject::~DUserClientObject() {
	geometry=NULL;
}

