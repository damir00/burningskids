
#ifndef _DUSERCLIENTFREEGLUT_H_
#define _DUSERCLIENTFREEGLUT_H_

#include "duserclient.h"
#include "duserclientgeometry.h"
#include "dglrendertarget.h"
#include "dglshader.h"

#include <glm/glm.hpp>

#include "freeglut.h"

class DUserClientFreeGlutResource : public DResource {
public:
	GLuint texture_id;
	DUserClientFreeGlutResource();
	DUserClientFreeGlutResource(DResourceType type,int texture);
};

class DUserClientFreeGlutFont : public DResource {
public:
	GLuint texture_id;
	float spacings[256];
	float offsets[256];
	DUserClientFreeGlutFont();
	DUserClientFreeGlutFont(DResourceType type,int texture);
};


class DUserClientFreeGlutObject : public DUserClientObject {
	void init();
public:
	//DUserClientFreeGlutResource* resource;
	DGLShader* shader;

	vector<DUserClientFreeGlutResource*> textures;
	vector<DGLShaderUni<float> > float_unifs;
	vector<DGLShaderUni<float[4]> > float4_unifs;

	DUserClientFreeGlutObject();
	DUserClientFreeGlutObject(dvect size=dvect(1,1));
	~DUserClientFreeGlutObject();

	void setFloat(int id,float value);
	void setFloat4(int id,float v1,float v2,float v3,float v4);
	void addTexture(DResource* resource);
	void setTexture(int id,DResource* resource);
	//void calcGeometry();
};

class DUserClientFreeGlutLight : public DUserClientObject {
public:
	DUserClientFreeGlutResource* color;
	bool cast_shadows;
	dvect origin;

	DUserClientFreeGlutLight(DUserClientFreeGlutResource* color,dvect size,dvect origin);
};

class DUserClientFreeGlutCar : public DUserClientObject {
public:
	DUserClientFreeGlutResource* color;
	DUserClientFreeGlutResource* normal;
	DUserClientFreeGlutResource* height;

	DUserClientFreeGlutCar();
	DUserClientFreeGlutCar(DUserClientFreeGlutResource* color,DUserClientFreeGlutResource* normal,dvect size=dvect(1,1));
	~DUserClientFreeGlutCar();
//	void renderColor();
//	void renderNormal();
};
class DUserClientFreeGlutTerrain : public DUserClientObject {
public:
	DUserClientFreeGlutResource* textures[4];
	DUserClientFreeGlutResource* alphas;
	DUserClientFreeGlutTerrain();
};
class DUserClientFreeGlutRoad : public DUserClientObject {
public:
	DUserClientFreeGlutResource* texture;
	//DUserClientGeometry* geom;
	GLuint vbo_pos;
	GLuint vbo_coord;
	GLuint vbo_index;
	int total_vertices;

	DUserClientFreeGlutRoad();
	DUserClientFreeGlutRoad(DUserClientGeometry* geom,DUserClientFreeGlutResource* texture);
	~DUserClientFreeGlutRoad();
};



class DUserClientFreeGlutCurve : public DUserClientObject {
public:

	DUserClientFreeGlutResource* texture;
	GLint vbo_pos;
	int current_size;
	int max_size;
	float width2;

	DBoundingBox bb;

	dvect prev_pos;
	bool first_segment;

	float new_data[4];

	void calc_new_data(dvect pos);
	void calc_first_data(dvect pos);
	void init(int max_segments,float _width);
	void deinit();
	bool add_segment(dvect pos);
	void update_last_segment(dvect pos);
};



class DUserClientFreeGlut : public DUserClient {
private:
	int window;

	glm::mat4 matrix_identity;
	glm::mat4 matrix;	//current matrix
	std::vector<glm::mat4> matrix_stack;	//...

	DBoundingBox screen_bb;			//screenspace
	DBoundingBox real_screen_bb;	//worldspace

	//shaders!
	DGLShader* shader_base;		//basic color shader
	GLint shader_base_uni_color;

	DGLShader* shader_texture;	//basic texture shader
	GLint shader_texture_uni_color;

	DGLShader* shader_splatting;
	DGLShader* shader_aa;
	DGLShader* shader_text;
	GLint shader_text_uni_color;

	DGLShader* shader_flat_shadow;
	GLint shader_flat_shadow_uni_world_mat;
	GLint shader_flat_shadow_uni_screen_mat;
	GLint shader_flat_shadow_uni_origin;

	DGLShader* shader_bumps;
	GLint shader_bumps_uni_dir;

	DGLShader* shader_lighting;

	DGLShader* shader_shadow;
	GLint shader_shadow_uni_sub;
	GLint shader_shadow_uni_offset;

	//new lighting..
	DGLShader* lighting_textured2;
	GLint shader_lighting_textured2_uni_dir;
	GLint shader_lighting_textured2_uni_origin;

	DGLShader* lighting2;


	GLhandleARB shader_lighting_FSHandle;
	GLhandleARB shader_lighting_PHandle;
	GLint shader_lighting_uni_ambient;
	GLint shader_lighting_uni_sun_dir;
	GLint shader_lighting_uni_sun_color;
	GLint shader_lighting_uni_clouds_offset;
	GLint shader_lighting_uni_clouds_scale;
	GLint shader_lighting_uni_shadow_quality;

	GLhandleARB shader_lighting_textured_FSHandle;
	GLhandleARB shader_lighting_textured_PHandle;
	GLint shader_lighting_textured_uni_dir;
	GLint shader_lighting_textured_uni_origin;

//	GLhandleARB shader_bumps_FSHandle;
//	GLhandleARB shader_bumps_PHandle;

	GLhandleARB shader_shadow_FSHandle;
	GLhandleARB shader_shadow_PHandle;

	GLhandleARB shader_normal_vs;


	int curve_max_segments;		//global max
	GLuint curve_vbo_index;		//curve shared vbos
	GLuint curve_vbo_tex;


	//render targets
	DGLRenderTarget* rt_normal;
	//rgb=normal
	//a=free

	DGLRenderTarget* rt_color;
	//rgb=color
	//a=free

	DGLRenderTarget* rt_height;
	//r=height or flat shadow
	//g=shadow
	//b=cluds

	DGLRenderTarget* rt_final;


	//0-1, affects rt_height size
	//affects shadows, clouds and flat shadows quality
	float shadow_quality;
	int rt_height_width;
	int rt_height_height;

	GLuint vbo_quad_pos;			//quad (0,0) - (1,1)
	GLuint vbo_quad_centered_pos;	//quad(0.5,-0.5) - (-0.5,0.5)
	GLuint vbo_quad_coord;			//normalized texcoord (0,0) - (1,1)
	GLuint vbo_screen_pos;
	GLuint *vbo_text_coord;
	GLuint vbo_clouds_pos;

	void createWindow(string title,int width,int height,bool fullscreen,int x,int y);
	void closeWindow();
	void initGL();

	void render_init_matrix();
	void init_shader_lighting();
	void init_shader_lighting_textured();
	void init_shader_splatting();
	void init_shader_bumps();
	void init_shader_shadow();
	void init_shader_aa();
	void init_shader_text();
	void init_shader_flat_shadow();
	void init_shader_base();
	void init_shader_texture();

	void init_rtts(int w,int h);
	void init_vbos();

	void init_curves(int num_vbos,int max_segments);

	void shader_uniform_vec2(GLint uni,float x,float y);
	void shader_uniform_vec2(GLint uni,dvect v);
	void shader_uniform_vec3(GLint uni,float x,float y,float z);
	void shader_uniform_vec3(GLint uni,dvect3 v);

	DGLRenderTarget* new_render_target();
	void matrix_init_perspective(int w,int h);
	void matrix_init_ortho(int w,int h);

	void renderTexture(GLuint tex,dvect size=dvect(1,1));

	void renderColor(DRenderNode* node);
	void renderNormal(DRenderNode* node,float angle=0);
	void renderLights(DRenderNode* node,float angle=0,vector<DRenderNode*>* nodes=NULL);
	void renderLights2(DRenderNode* node,float angle=0,vector<DRenderNode*>* nodes=NULL);
	void renderShadow(DRenderNode* node,float angle=0,float height=0);
	void renderShadow2();
	//void renderShadow3();
	void renderShadowPoint(DRenderNode* node,float angle,float light_angle,float light_tan);
	void renderHeight(DRenderNode* node,float angle=0,float height=0);
	void renderClouds();
	void renderFlatShadow(DRenderNode* node,float height,dvect3 origin,DBoundingBox* target_bb);
	void renderFlatShadow2(DRenderNode* node,float height,dvect3 origin,DBoundingBox* target_bb);

	void _setAmbient();
	void _setSunDir();
	void _setSunColor();

	//for debug
	int real_window_width;
	int real_window_height;
	float render_upscale;

	//cached vbos
	std::vector<GLint> curve_vbos;

public:
	void sortChildren(DRenderNode* node);

	DUserClientFreeGlut(DUserClientConfig config);
	~DUserClientFreeGlut();

	//resources
	bool supportsResourceType(DResourceType type);
	DResource* loadResource(DResourceType type,string file);
	DResource* loadResourceImage(char* data,int width,int height,int depth);

	void unloadResource(DResource* resource);

	//objects
	DUserClientObject* createObject(dvect size=dvect(1,1));
	DUserClientObject* createCar(DResource* color,DResource* normal,DResource* height,dvect size=dvect(1,1));
	DUserClientObject* createTerrain(DResource* colors[4],DResource* alphas,dvect size=dvect(1,1));
	DUserClientObject* createLight(DResource* resource,dvect size=dvect(1,1),dvect origin=dvect(0.5,0.5));
	DUserClientObject* createRoad(DUserClientGeometry* geom,DResource* texture);
	DUserClientObject* createCurve(DResource* resource,float width);

	void releaseCurve(DUserClientObject* curve);

	void saveMatrix();
	void restoreMatrix();
	void transform(dvect pos,dvect scale,double rot);
	void transform(dvect3 pos,dvect3 scale,double rot);
	void transformCam(dvect pos,dvect scale,double rot);

	void prerender();
	void render(long ts,duserclient_get_nodes get_nodes);
	void postrender();
	//void renderObject(DUserClientObject* object);
	void renderParticle(DParticle* particle);

	void setShadowQuality(float q);

	//stuff
	//buffer must be of size config.width*config.height*3
	//image is in RGB format
	void screenshot(char* buffer);

	//drawing
	void lineTo(dvect to);
	void text(DResource* res,string text,float size);
	void quad(dvect pos,dvect size,bool fill);

	//stuff
	void _glutRender();
	void _resizeWindow(int width,int height);
	void _keyboard(int key,bool down);
	void _mouseMove(int x,int y);
	void _mouseButton(int key,bool down);
};

#endif

