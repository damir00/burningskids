
#ifndef _DGLSHADER_H_
#define _DGLSHADER_H_

#include <glew.h>
#include <gl.h>
#include <glu.h>

#include <string>

class DGLShader {
public:
	GLhandleARB vs;
	GLhandleARB fs;
	GLhandleARB p;

	GLint uni_matrix_view;
	GLint uni_matrix_model;

	bool compile(std::string source);
	bool compileFromFile(std::string filename);
	bool compileFull(std::string vertex_source,std::string fragment_source);
	bool compileFullFromFile(std::string vertex_file,std::string fragment_file);
	void use();
	void free();
	int getUniform(std::string name);
	//program must be used when setting
	void setFloat(int id,float value);
	void setVec4(int id,const float *value);
	void setVec2(int id,float *value);
	void setVec2(int id,float value1,float value2);
	void setMat4(int id,float *value);

	void useSetViewMatrix(float* matrix);
	void setModelMatrix(float* matrix);
};

template <class itemType>
struct DGLShaderUni {
	GLint id;
	itemType value;

	DGLShaderUni(int _id,itemType _value) {
		id=_id;
		value=_value;
	}
	DGLShaderUni(int _id) {
		id=_id;
	}
};

#endif
