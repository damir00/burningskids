#ifndef _DGLFBO_H_
#define _DGLFBO_H_

#include "dglrendertarget.h"

class DGLFBO : public DGLRenderTarget {

	GLuint fbo;
	GLuint texture;
	int width;
	int height;
public:
	static bool supported();

	bool init(int width,int height);
	void activate();
	void deactivate();
	GLuint get_texture();
};

#endif

