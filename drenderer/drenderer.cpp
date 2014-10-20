
#include "drenderer.h"

DRendererConfig::DRendererConfig() {
	title="Hello user";
	width=800;
	height=600;
	fullscreen=false;
}
DRendererConfig::DRendererConfig(string _title,int _width, int _height, bool _fullscreen) {
	title=_title;
	width=_width;
	height=_height;
	fullscreen=_fullscreen;
}



