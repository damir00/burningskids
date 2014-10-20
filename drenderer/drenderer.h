
#ifndef _DRENDERER_H_
#define _DRENDERER_H_

#include "freeglut.h"


struct DRendererConfig {
	string title;
	int width;
	int height;
	bool fullscreen;
	DRendererConfig();
	DRendererConfig(string _title,int _width, int _height, bool _fullscreen=false);
};

class DRenderer {
private:
	int window;

	void createWindow(string title,int width,int height,bool fullscreen);
	void closeWindow();

	void initGL();

public:



};

#endif


