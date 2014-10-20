#ifndef _DGLRENDERTARGET_H_
#define _DGLRENDERTARGET_H_

#include <glew.h>
#include <gl.h>
#include <glu.h>

class DGLRenderTarget {
public:
	virtual bool init(int width,int height)=0;
	virtual void activate()=0;
	virtual void deactivate()=0;
	virtual GLuint get_texture()=0;
};

#endif

