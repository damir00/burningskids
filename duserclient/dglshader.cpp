#include "dglshader.h"

using namespace std;

#include <stdio.h>

#include "dutils.h"

bool DGLShader::compileFull(string vertex_source,string fragment_source) {

	p=glCreateProgramObjectARB();
	fs=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	vs=glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);;

	int i;
	GLint compile_status,link_status;

	const char* vsource_c=vertex_source.c_str();
	glShaderSourceARB(vs,1,(const char**)&vsource_c,NULL);
	glCompileShaderARB(vs);

	glGetObjectParameterivARB(vs,GL_OBJECT_COMPILE_STATUS_ARB,&compile_status);
	if(compile_status==GL_FALSE) {
		glGetObjectParameterivARB(vs,GL_OBJECT_COMPILE_STATUS_ARB,&i);
		char* s=new char[32768];
		glGetInfoLogARB(vs,32768,NULL,s);
		printf("Vertex shader compilation failed: %s\n", s);
		delete(s);
		p=0;
		fs=0;
		vs=0;
		return false;
	}

	const char* source_c=fragment_source.c_str();
	glShaderSourceARB(fs,1,(const char**)&source_c,NULL);
	glCompileShaderARB(fs);

	glGetObjectParameterivARB(fs,GL_OBJECT_COMPILE_STATUS_ARB,&compile_status);
	if(compile_status==GL_FALSE) {
		glGetObjectParameterivARB(fs,GL_OBJECT_COMPILE_STATUS_ARB,&i);
		char* s=new char[32768];
		glGetInfoLogARB(fs,32768,NULL,s);
		printf("Shader compilation failed: %s\n", s);
		delete(s);
		p=0;
		fs=0;
		vs=0;
		return false;
	}

	glAttachObjectARB(p,vs);
	glAttachObjectARB(p,fs);
	glLinkProgramARB(p);

	glGetObjectParameterivARB(p,GL_OBJECT_LINK_STATUS_ARB,&link_status);
	if(link_status==GL_FALSE) {
		printf("Shader failed to link!\n");
		return false;
	}

	glUseProgramObjectARB(p);

	char name_buf[]="tex32";
	for(int i=0;i<32;i++) {
		if(i<10) {
			name_buf[3]='0'+i;
			name_buf[4]=0;
		}
		else {
			name_buf[3]='0'+(i/10);
			name_buf[4]='0'+(i%10);
		}
		glUniform1iARB(glGetUniformLocationARB(p,name_buf),i);
	}

	uni_matrix_view=glGetUniformLocationARB(p,"view_matrix");
	uni_matrix_model=glGetUniformLocationARB(p,"model_matrix");

	glUseProgramObjectARB(0);

	return true;
}
bool DGLShader::compileFullFromFile(string vertex_file,string fragment_file) {
	char* vertex_content=get_file_content(vertex_file);
	if(!vertex_content) {
		printf("Can't open file %s\n",vertex_file.c_str());
		return false;
	}
	char* fragment_content=get_file_content(fragment_file);
	if(!fragment_content) {
		printf("Can't open file %s\n",fragment_file.c_str());
		delete[] vertex_content;
		return false;
	}

	bool ret=compileFull(vertex_content,fragment_content);
	if(!ret) {
		printf("Shader %s, %s failed to compile\n",vertex_file.c_str(),
				fragment_file.c_str());
	}

	delete[] vertex_content;
	delete[] fragment_content;

	return ret;
}

bool DGLShader::compileFromFile(string filename) {
	char* content=get_file_content(filename);
	if(!content) {
		printf("Can't open file %s\n",filename.c_str());
		return false;
	}
	bool ret=compile(content);
	delete[] content;
	if(!ret) {
		printf("Shader %s failed to compile\n",filename.c_str());
	}
	return ret;
}

bool DGLShader::compile(string source) {

	p=glCreateProgramObjectARB();
	fs=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	vs=0;

	int i;
	const char* source_c=source.c_str();
	glShaderSourceARB(fs,1,(const char**)&source_c,NULL);
	glCompileShaderARB(fs);

	GLint compile_status;
	glGetObjectParameterivARB(fs,GL_OBJECT_COMPILE_STATUS_ARB,&compile_status);
	if(compile_status==GL_FALSE) {
		glGetObjectParameterivARB(fs,GL_OBJECT_COMPILE_STATUS_ARB,&i);
		char* s=new char[32768];
		glGetInfoLogARB(fs,32768,NULL,s);
		printf("Shader compilation failed: %s\n", s);
		delete(s);
		p=0;
		fs=0;
		return false;
	}

	glAttachObjectARB(p,fs);
	glLinkProgramARB(p);
	glUseProgramObjectARB(p);

	char name_buf[]="tex32";
	for(int i=0;i<32;i++) {
		if(i<10) {
			name_buf[3]='0'+i;
			name_buf[4]=0;
		}
		else {
			name_buf[3]='0'+(i/10);
			name_buf[4]='0'+(i%10);
		}
		glUniform1iARB(glGetUniformLocationARB(p,name_buf),i);
	}

	uni_matrix_view=glGetUniformLocationARB(p,"view_matrix");
	uni_matrix_model=glGetUniformLocationARB(p,"model_matrix");

	glUseProgramObjectARB(0);

	return true;
}
void DGLShader::free() {
	glDeleteProgram(p);
	glDeleteShader(fs);
	glDeleteShader(vs);
}

int DGLShader::getUniform(string name) {
	glUseProgramObjectARB(p);
	int id=glGetUniformLocationARB(p,name.c_str());
	//glUseProgramObjectARB(0);

	if(id==-1) {
		printf("WARNING: failed to get shader uniform %s\n",name.c_str());
	}

	return id;
}
void DGLShader::use() {
	glUseProgramObjectARB(p);
}
void DGLShader::setFloat(int id,float value) {
	glUniform1fARB(id,value);
}
void DGLShader::setVec4(int id,const float *value) {
	glUniform4fvARB(id,1,value);
}
void DGLShader::setVec2(int id,float *value) {
	glUniform2fvARB(id,1,value);
}
void DGLShader::setVec2(int id,float value1,float value2) {
	static float v[]={value1,value2};
	setVec2(id,v);
}
void DGLShader::setMat4(int id,float *value) {
	glUniformMatrix4fvARB(id,1,false,value);
}
void DGLShader::useSetViewMatrix(float* matrix) {
	use();
	setMat4(uni_matrix_view,matrix);
}
void DGLShader::setModelMatrix(float* matrix) {
	setMat4(uni_matrix_model,matrix);
}



