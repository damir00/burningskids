
#include "dglfbo.h"

#include <iostream>
using namespace std;

bool DGLFBO::supported() {
	return true;
}

bool DGLFBO::init(int _width,int _height) {
	width=_width;
	height=_height;

	glGetError();

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	GLint err=glGetError();
	if(err!=GL_NO_ERROR) {
		cout<<"FBO error: "<<gluErrorString(err)<<endl;
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);

	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		glDeleteTextures(1,&texture);
		glDeleteFramebuffersEXT(1,&fbo);
		return false;
	}

	return true;
}
void DGLFBO::activate() {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	//glPushAttrib(GL_VIEWPORT_BIT);
	//glViewport(0,0,width, height);
}
void DGLFBO::deactivate() {
	//glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
GLuint DGLFBO::get_texture() {
	return texture;
}

