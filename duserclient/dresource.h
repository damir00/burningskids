
#ifndef _DRESOURCE_H_
#define _DRESOURCE_H_

#include <string>

using namespace std;

enum DResourceType {
	IMAGE,
	IMAGE_TRANSPARENT,
	MODEL,
	AUDIO,
	FONT
};

class DResource {
public:
	DResourceType type;

	static DResource* load(DResourceType type,string file);
};

#endif

