#include "duserclientfreeglut.h"
#include "dutils.h"
#include "dglfbo.h"

#include <string.h>
#include <math.h>
#include <sys/time.h>

#define ILUT_USE_OPENGL

#include "il.h"
#include "ilu.h"
#include "ilut.h"

#include <glew.h>
#include <gl.h>
#include <glu.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//some constants
static const float color_white[]={1,1,1,1};
static const float color_red[]={1,0,0,1};


GLuint vbo_create(int num_vertices,float* data) {
	GLuint id=0;

	glGenBuffers(1,&id);
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glBufferData(GL_ARRAY_BUFFER,num_vertices*2*sizeof(float),data,GL_STATIC_DRAW);

	return id;
}

GLuint vbo_create_dynamic(int num_vertices) {
	GLuint id=0;

	glGenBuffers(1,&id);
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glBufferData(GL_ARRAY_BUFFER,num_vertices*2*sizeof(float),0,GL_STREAM_DRAW);

	return id;
}
void vbo_update(GLuint id,int offset,int size,float* data) {
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glBufferSubData(GL_ARRAY_BUFFER,offset*sizeof(float),size*sizeof(float),data);
}
void vbo_delete(GLuint id) {
	glDeleteBuffers(1,&id);
}

void vbo_use_pos(GLuint id) {
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glVertexPointer(2,GL_FLOAT,0,(char *)NULL);
}
void vbo_use_coord(GLuint id) {
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glTexCoordPointer(2,GL_FLOAT,0,(char *)NULL);
}
void vbo_use_pos_own(float* data) {
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glVertexPointer(2,GL_FLOAT,0,data);
}
void vbo_use_coord_own(float* data) {
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glTexCoordPointer(2,GL_FLOAT,0,data);
}

void vbo_draw(int vertices) {
	glDrawArrays(GL_TRIANGLE_STRIP,0,vertices);
}
GLuint *vbo_create_text_coord(int width,int height) {
	int cells=width*height;

	GLuint *vbos=new GLuint[cells];

	for(int i=0;i<cells;i++) {
		dvect c1=dvect(i%width,(i-1)/width+1)/width;
		dvect c2=c1+(dvect(-1,-1)/width);

		float data[]={
			c2.x,c2.y,
			c1.x,c2.y,
			c2.x,c1.y,
			c1.x,c1.y
		};

		vbos[i]=vbo_create(4,data);
	}
	return vbos;
}

void shader_use(GLuint id) {
	glUseProgramObjectARB(id);
}

void DUserClientFreeGlutCurve::calc_new_data(dvect pos) {
	dvect tan=(prev_pos-pos).perp().normalize()*width2;
	dvect p1=pos+tan;
	dvect p2=pos-tan;

	new_data[0]=-p1.x;
	new_data[1]=-p1.y;
	new_data[2]=-p2.x;
	new_data[3]=-p2.y;
}
void DUserClientFreeGlutCurve::calc_first_data(dvect pos) {

	dvect tan=(prev_pos-pos).perp().normalize()*width2;
	dvect p1=prev_pos+tan;
	dvect p2=prev_pos-tan;

	new_data[0]=-p1.x;
	new_data[1]=-p1.y;
	new_data[2]=-p2.x;
	new_data[3]=-p2.y;
}
void DUserClientFreeGlutCurve::init(int max_segments,float _width) {
	current_size=0;
	max_size=max_segments*4;
	width2=_width/2;
	first_segment=true;
}
void DUserClientFreeGlutCurve::deinit() {
	vbo_delete(vbo_pos);
}
bool DUserClientFreeGlutCurve::add_segment(dvect pos) {
	if(first_segment) {
		prev_pos=pos;
		first_segment=false;
		bb.start=bb.end=pos;	//init bb
		return true;
	}
	if(current_size>=max_size) return false;

	if(current_size==0) {	//build first segment
		calc_first_data(pos);
		vbo_update(vbo_pos,current_size,4,new_data);
		current_size+=4;
		calc_new_data(pos);
		vbo_update(vbo_pos,current_size,4,new_data);

		bb.build(prev_pos,pos);
	}
	else {
		calc_new_data(pos);
		vbo_update(vbo_pos,current_size,4,new_data);
		bb.merge(pos);
	}

	current_size+=4;
	prev_pos=pos;

	return true;
}
void DUserClientFreeGlutCurve::update_last_segment(dvect pos) {
	calc_new_data(pos);
	vbo_update(vbo_pos,current_size-4,4,new_data);
}



//resource

DUserClientFreeGlutResource::DUserClientFreeGlutResource() {
	texture_id=-1;
}
DUserClientFreeGlutResource::DUserClientFreeGlutResource(DResourceType _type,int texture) {
	type=_type;
	texture_id=texture;
}
DUserClientFreeGlutFont::DUserClientFreeGlutFont(DResourceType _type,int texture) {
	type=_type;
	texture_id=texture;
	for(int i=0;i<256;i++) {
		spacings[i]=100;
	}
}

//object

void DUserClientFreeGlutObject::init() {
	type=TYPE_OBJECT;
	size=dvectone;
	shader=NULL;
}

DUserClientFreeGlutObject::DUserClientFreeGlutObject(): DUserClientObject() {
	init();
}
DUserClientFreeGlutObject::DUserClientFreeGlutObject(dvect _size): DUserClientObject() {
	init();
	size=_size;
}
DUserClientFreeGlutObject::~DUserClientFreeGlutObject() {
	shader=NULL;
}

void DUserClientFreeGlutObject::setFloat(int id,float value) {
	for(int i=0;i<float_unifs.size();i++) {
		if(float_unifs[i].id==id) {
			float_unifs[i].value=value;
			return;
		}
	}
	float_unifs.push_back(DGLShaderUni<float>(id,value));
}
void DUserClientFreeGlutObject::setFloat4(int id,float v1,float v2,float v3,float v4) {
	for(int i=0;i<float4_unifs.size();i++) {
		if(float4_unifs[i].id==id) {

			float4_unifs[i].value[0]=v1;
			float4_unifs[i].value[1]=v2;
			float4_unifs[i].value[2]=v3;
			float4_unifs[i].value[3]=v4;
			return;
		}
	}
	DGLShaderUni<float[4]> uni=DGLShaderUni<float[4]>(id);
	uni.value[0]=v1;
	uni.value[1]=v2;
	uni.value[2]=v3;
	uni.value[3]=v4;

	float4_unifs.push_back(uni);
}

void DUserClientFreeGlutObject::addTexture(DResource* resource) {
	textures.push_back((DUserClientFreeGlutResource*)resource);
}
void DUserClientFreeGlutObject::setTexture(int id,DResource* resource) {
	if(id<0 || id>=textures.size()) return;
	textures[id]=((DUserClientFreeGlutResource*)resource);
}

/*
void DUserClientFreeGlutObject::calcGeometry(DResource* resource) {
	if(geometry) {
		delete(geometry);
		geometry=NULL;
	}
	if(!resource) {
		return;
	}
}
*/

//deprecated
/*
void DUserClientFreeGlutObject::render() {

	if(!resource) return;

	glPushMatrix();
	glTranslatef(pos.x,pos.y,0);
	glRotatef(radToDeg(angle),0,0,1);
	glTranslatef(-size.x/2,-size.y/2,0);

	if(resource->type==IMAGE_TRANSPARENT) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,resource->texture_id);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(1,0); glVertex2f(size.x,0);
	glTexCoord2f(0,1); glVertex2f(0,size.y);
	glTexCoord2f(1,1); glVertex2f(size.x,size.y);
	glEnd();

	glDisable(GL_BLEND);

	glPopMatrix();
}
*/

DUserClientFreeGlutLight::DUserClientFreeGlutLight(DUserClientFreeGlutResource* _color,
		dvect _size,dvect _origin) {
	color=_color;
	size=_size;
	origin=_origin;
	cast_shadows=true;
	type=TYPE_LIGHT;
}

//car
DUserClientFreeGlutCar::DUserClientFreeGlutCar() {
	color=NULL;
	normal=NULL;
	type=TYPE_CAR;
}
DUserClientFreeGlutCar::DUserClientFreeGlutCar(DUserClientFreeGlutResource* _color,
		DUserClientFreeGlutResource* _normal,dvect _size) {
	color=_color;
	normal=_normal;
	size=_size;
	//pos=-size/2;	//center it
	type=TYPE_CAR;
}
DUserClientFreeGlutCar::~DUserClientFreeGlutCar() {
}

DUserClientFreeGlutTerrain::DUserClientFreeGlutTerrain() {
	type=TYPE_TERRAIN;
}

DUserClientFreeGlutRoad::DUserClientFreeGlutRoad() {
	type=TYPE_ROAD;
	//geom=NULL;
	size=dvectone;
}
DUserClientFreeGlutRoad::DUserClientFreeGlutRoad(DUserClientGeometry* _geom,DUserClientFreeGlutResource* _texture) {
	type=TYPE_ROAD;
	geometry=_geom;
	size=dvectone;
	texture=_texture;

	//TODO: support for multiple chunks
	if(geometry->chunks.size()==0) return;

	DUserClientGeometryChunk *chunk=geometry->chunks[0];
	total_vertices=chunk->num_vertices;

	glGenBuffersARB(1,&vbo_index);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,vbo_index);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, chunk->num_indices*sizeof(int),
			chunk->indices, GL_STATIC_DRAW_ARB);

	vbo_pos=vbo_create(total_vertices,chunk->vertices);
	vbo_coord=vbo_create(total_vertices,chunk->tex_coords);

}
DUserClientFreeGlutRoad::~DUserClientFreeGlutRoad() {
	//TODO: cleanup
}


//oops, glut is global
DUserClientFreeGlut* client;

void clientResize(int w,int h);
void clientKeyboard(unsigned char key,int x,int y);
void clientKeyboardUp(unsigned char key,int x,int y);
void clientKeyboardSpecial(int key,int x,int y);
void clientKeyboardSpecialUp(int key,int x,int y);
void clientPassiveMotion(int x,int y);
void clientMouse(int button,int state,int x,int y);

DUserClientFreeGlut::DUserClientFreeGlut(DUserClientConfig _config) : DUserClient(_config) {
	client=this;

	ilInit();
	iluInit();
	ilutRenderer(ILUT_OPENGL);

	matrix_identity=glm::mat4(1.0f);
	matrix_stack.push_back(matrix_identity);

	//debug
	render_upscale=1;
	real_window_width=config.width;
	real_window_height=config.height;
	config.width*=render_upscale;
	config.height*=render_upscale;

	int argc=0;
	glutInit(&argc,NULL);
	createWindow(config.title,real_window_width,real_window_height,config.fullscreen,
			config.window_x,config.window_y);

	GLenum glew_err = glewInit();
	if(glew_err!=GLEW_OK) {
	  printf("Glew init failed: %s\n", glewGetErrorString(glew_err));
	  return;
	}
	printf("GL version %s\n",glGetString(GL_VERSION));
	printf("GLEW version %s\n",glewGetString(GLEW_VERSION));

	init_shader_lighting();
	init_shader_lighting_textured();
	init_shader_bumps();
	init_shader_splatting();
	init_shader_shadow();
	init_shader_aa();
	init_shader_text();
	init_shader_flat_shadow();
	init_shader_base();
	init_shader_texture();

	init_vbos();

	initGL();
	rt_normal=new_render_target();
	rt_color=new_render_target();
	rt_height=new_render_target();
	rt_final=new_render_target();

	setShadowQuality(config.shadow_quality);

	rt_height_width=config.width; //*shadow_quality;
	rt_height_height=config.height; //*shadow_quality;

	init_rtts(config.width,config.height);

	clouds_resource=NULL;

	screen_bb.start=dvect(-3,-3);
	screen_bb.end=dvect(2,2);

	init_curves(200,100);
	//octree=new DRenderOctree(dvect(-5000,-5000),dvect(5000,5000),15);
}
DUserClientFreeGlut::~DUserClientFreeGlut() {
	closeWindow();
}

void DUserClientFreeGlut::init_rtts(int width,int height) {
	bool rt_normal_inited=rt_normal->init(width,height);
	bool rt_color_inited=rt_color->init(width,height);
	bool rt_height_inited=rt_height->init(width,height);
	bool rt_final_inited=rt_final->init(width,height);

	cout<<"RT normal init "<<(rt_normal_inited ? "succeeded" : "failed")<<"\n";
	cout<<"RT color init "<<(rt_color_inited ? "succeeded" : "failed")<<"\n";
	cout<<"RT height init "<<(rt_height_inited ? "succeeded" : "failed")<<"\n";
	cout<<"RT final init "<<(rt_final_inited ? "succeeded" : "failed")<<"\n";

	rt_final->deactivate();
}


void DUserClientFreeGlut::init_curves(int num_vbos,int max_segments) {
	curve_max_segments=max_segments;

	for(int i=0;i<num_vbos;i++) {
		curve_vbos.push_back(vbo_create_dynamic(max_segments*2));
	}


	int index_count=max_segments*2;
	int tex_count=max_segments*2*2;

	float* data_tex=new float[tex_count];
	int* data_index=new int[index_count];

	for(int i=0;i<max_segments;i++) {
		//data_index[i]=
	}

	glGenBuffersARB(1,&curve_vbo_index);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,curve_vbo_index);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,index_count*sizeof(int),
			data_index,GL_STATIC_DRAW_ARB);

	for(int i=0;i<tex_count;i+=2) {
		data_tex[i+1]=(i/2)%2;
		data_tex[i]=(float)(i/2)/10;
	}

	curve_vbo_tex=vbo_create(max_segments*2,data_tex);

	delete[] data_tex;
	delete[] data_index;
}


bool DUserClientFreeGlut::supportsResourceType(DResourceType type) {
	return (type==IMAGE || type==IMAGE_TRANSPARENT || type==FONT);
}

DResource* DUserClientFreeGlut::loadResourceImage(char* data,int width,int height,int depth) {
	if(!data || (depth!=1 && depth!=3 && depth!=4)) {
		return new DUserClientFreeGlutResource(IMAGE_TRANSPARENT,0);
	}

	GLuint texture_id;
	glGenTextures(1,&texture_id);
	glBindTexture(GL_TEXTURE_2D,texture_id);

	static GLint internal_formats[]={GL_RED,GL_RED,GL_RGB,GL_RGBA};
	static GLint formats[]={GL_RED,GL_RED,GL_RGB,GL_RGBA};
	int f=depth-1;

	glTexImage2D(GL_TEXTURE_2D,0,internal_formats[f],
			width,height,0,formats[f],GL_UNSIGNED_BYTE,data);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	return new DUserClientFreeGlutResource(IMAGE_TRANSPARENT,texture_id);
}

DResource* DUserClientFreeGlut::loadResource(DResourceType type,string file) {
	if(!supportsResourceType(type)) return NULL;

	GLuint texture_id;
	switch(type) {
		case FONT: {
			if(!ilLoadImage(file.c_str())) {
				return NULL;
			}
			glGenTextures(1,&texture_id);
			glBindTexture(GL_TEXTURE_2D,texture_id);

			int w=ilGetInteger(IL_IMAGE_WIDTH);
			int h=ilGetInteger(IL_IMAGE_HEIGHT);

			char *data=new char[w*h];

			ilCopyPixels(0,0,0,w,h,1,IL_LUMINANCE,IL_UNSIGNED_BYTE,data);

			glTexImage2D(GL_TEXTURE_2D,0,GL_ALPHA,w,h,0,GL_ALPHA,GL_UNSIGNED_BYTE,data);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

			delete[] data;

			DUserClientFreeGlutFont *font=new DUserClientFreeGlutFont(type,texture_id);

			if(file.length()>4) {
				string s_filename=file.erase(file.length()-4,4)+".txt";
				FILE* s_file=fopen(s_filename.c_str(),"r");
				if(s_file) {

					for(int i=0;i<256;i++) {
						float d,o;
						if(fscanf(s_file,"%f %f",&d,&o)!=2) break;
						font->spacings[i]=d;
						font->offsets[i]=o;
					}
					fclose(s_file);
				}
			}
			return font;
		}
		default:
			texture_id=ilutGLLoadImage((char*)file.c_str());

			glBindTexture(GL_TEXTURE_2D,texture_id);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
			glGenerateMipmap(GL_TEXTURE_2D);

			return new DUserClientFreeGlutResource(type,texture_id);
	}
	return NULL;


	/*
	GLuint texture_id;
	int channels;
	ILint il_format;
	GLint gl_format;
	GLint gl_format2;

	switch(type) {
		case FONT:
			channels=1;
			il_format=IL_LUMINANCE;
			gl_format=GL_ALPHA;
			break;
		case IMAGE:
			channels=3;
			il_format=IL_RGB;
			gl_format=GL_RGB;
			break;
		case IMAGE_TRANSPARENT:
			channels=4;
			il_format=IL_RGBA;
			gl_format=GL_RGBA;
			gl_format2=GL_RGBA;
			break;
		default:
			return NULL;
	}

	glGenTextures(1,&texture_id);
	ilLoadImage(file.c_str());
	glBindTexture(GL_TEXTURE_2D,texture_id);

	int w=ilGetInteger(IL_IMAGE_WIDTH);
	int h=ilGetInteger(IL_IMAGE_HEIGHT);

	char *data=new char[w*h*channels];

	ilCopyPixels(0,0,0,w,h,1,il_format,IL_UNSIGNED_BYTE,data);

	glTexImage2D(GL_TEXTURE_2D,0,gl_format,w,h,0,gl_format,GL_UNSIGNED_BYTE,data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if(type==FONT || true) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		gluBuild2DMipmaps( GL_TEXTURE_2D, gl_format, w,h, gl_format, GL_UNSIGNED_BYTE, data );
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SWAP_BYTES, IL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);



	delete(data);

	if(type==FONT) return new DUserClientFreeGlutFont(type,texture_id);
	return new DUserClientFreeGlutResource(type,texture_id);
	*/
}

void DUserClientFreeGlut::unloadResource(DResource* resource) {
	glDeleteTextures(1,&((DUserClientFreeGlutResource*)resource)->texture_id);
}

//objects
DUserClientObject* addObject(DUserClientObject* object);


DUserClientObject* DUserClientFreeGlut::createObject(dvect size) {
	DUserClientFreeGlutObject* obj=new DUserClientFreeGlutObject(size);

	/*
	DGLShader* shader=new DGLShader();

	char test_shader[]=""
			"uniform float val1;"
			"uniform sampler2D tex0;"
			"void main() {"
			"gl_FragColor=1-texture2D(tex0,gl_TexCoord[0].xy);"
			"}";

	if(shader->compile(test_shader)) {
		obj->shader=shader;
		int val1=shader->getFloat("val1");
		obj->setFloat(val1,0.5);
	}
	else {
		delete(shader);
	}
	*/

	return obj;
}
DUserClientObject* DUserClientFreeGlut::createCar(DResource* color,DResource* normal,DResource* height,dvect size) {
	/*
	DUserClientFreeGlutCar* car=new DUserClientFreeGlutCar((DUserClientFreeGlutResource*)color,(DUserClientFreeGlutResource*)normal,size);
	car->height=(DUserClientFreeGlutResource*)height;
	return car;
	*/

	DUserClientFreeGlutObject* obj=new DUserClientFreeGlutObject(size);
	obj->addTexture(color);
	obj->addTexture(normal);
	obj->addTexture(height);
	obj->type=TYPE_CAR;

	return obj;
}
DUserClientObject* DUserClientFreeGlut::createTerrain(DResource* colors[4],DResource* alphas,dvect size) {
	DUserClientFreeGlutTerrain* t=new DUserClientFreeGlutTerrain();
	t->size=size;
	for(int i=0;i<4;i++) t->textures[i]=(DUserClientFreeGlutResource*)colors[i];
	t->alphas=(DUserClientFreeGlutResource*)alphas;
	return t;
}
DUserClientObject* DUserClientFreeGlut::createLight(DResource* resource,dvect size,dvect origin) {
	DUserClientFreeGlutLight* light=new DUserClientFreeGlutLight(
			(DUserClientFreeGlutResource*)resource,size,origin);

	/*
	DUserClientFreeGlutObject* light=new DUserClientFreeGlutObject(size);
	light->addTexture(resource);
	light->type=TYPE_LIGHT;
	*/

	return light;
}
DUserClientObject* DUserClientFreeGlut::createRoad(DUserClientGeometry* geom,DResource *texture) {
	DUserClientFreeGlutRoad* r=new DUserClientFreeGlutRoad(geom,(DUserClientFreeGlutResource*)texture);
	return r;
}
DUserClientObject* DUserClientFreeGlut::createCurve(DResource* resource,float width) {
	DUserClientFreeGlutCurve* c=new DUserClientFreeGlutCurve();
	c->type=TYPE_CURVE;
	c->size=dvectone;
	c->texture=(DUserClientFreeGlutResource*)resource;
//	c->angle=0;
//	c->pos=dvectzero;

	c->init(curve_max_segments,width);


	if(curve_vbos.size()>0) {
		c->vbo_pos=curve_vbos.back();
		curve_vbos.pop_back();

		//printf("delta gl got vbo %d, free vbos %d\n",c->vbo_pos,curve_vbos.size());
	}
	else {

		printf("gl out of free curve vbos, creating new\n");

		c->vbo_pos=vbo_create_dynamic(curve_max_segments*2);
	}


	return c;
}
void DUserClientFreeGlut::releaseCurve(DUserClientObject* object) {
	DUserClientFreeGlutCurve* curve=(DUserClientFreeGlutCurve*)object;

	//printf("gl releasing vbo %d\n",curve->vbo_pos);

	curve_vbos.push_back(curve->vbo_pos);
	delete(curve);
}

void DUserClientFreeGlut::saveMatrix() {
	//glPushMatrix();
	matrix_stack.push_back(matrix_stack.back());
}
void DUserClientFreeGlut::restoreMatrix() {
	//glPopMatrix();
	if(matrix_stack.size()<=1) {
		printf("WARNING: matrix stack underflow\n");
	}
	else {
		matrix_stack.pop_back();
	}

	shader_text->useSetViewMatrix(glm::value_ptr(matrix_stack.back()));
}

void DUserClientFreeGlut::transform(dvect pos,dvect scale,double rot) {
	/*
	glTranslatef(pos.x,pos.y,0);
	glRotatef(rot,0,0,1);
	glScalef(scale.x,scale.y,1);
	*/
	matrix_stack.back()=glm::scale(
			glm::rotate(
					glm::translate(
							matrix_stack.back(),
							glm::vec3(pos.x,pos.y,0)),
					(float)rot,glm::vec3(0,0,1)),
			glm::vec3(scale.x,scale.y,1));

	shader_text->useSetViewMatrix(glm::value_ptr(matrix_stack.back()));
}
void DUserClientFreeGlut::transform(dvect3 pos,dvect3 scale,double rot) {
	transform(dvect(pos.x,pos.y),dvect(scale.x,scale.y),rot);
}
void DUserClientFreeGlut::transformCam(dvect pos,dvect scale,double rot) {
	/*
	glRotatef(rot,0,0,1);
	glTranslatef(pos.x,pos.y,0);
	glScalef(scale.x,scale.y,1);
	*/

	/*
	matrix=glm::scale(
			glm::translate(
					glm::rotate<float>(matrix,rot,glm::vec3(0,0,1)),
			glm::vec3(pos.x,pos.y,0)),
			glm::vec3(scale.x,scale.y,1));
			*/

	//matrix=glm::rotate<float>(matrix,rot,glm::vec3(0,0,1));
	matrix=glm::translate(matrix,glm::vec3(pos.x,pos.y,0));
	//matrix=glm::scale(matrix,glm::vec3(scale.x,scale.y,1));

	//glLoadMatrixf(glm::value_ptr(matrix));
}

void DUserClientFreeGlut::render_init_matrix() {
	//glLoadIdentity();
	//glTranslatef(0,0,-3.5*4);
}

void DUserClientFreeGlut::prerender() {
	glutMainLoopEvent();

	//glClear(GL_COLOR_BUFFER_BIT);
	render_init_matrix();

}
void DUserClientFreeGlut::postrender() {
	glutSwapBuffers();
}
/*
void DUserClientFreeGlut::renderObject(DUserClientObject* object) {
	((DUserClientFreeGlutObject*)object)->render();
}
*/
void DUserClientFreeGlut::renderParticle(DParticle* particle) {
}


//input
/*
bool DUserClientFreeGlut::isKeyDown(int key) {
	return false;
}
dvect DUserClientFreeGlut::getMousePos() {
	return dvect(0,0);
}
bool DUserClientFreeGlut::isMouseDown(int key) {
	return false;
}
*/


//enviroment
void DUserClientFreeGlut::_setAmbient() {
	float v[]={ambient_color.x,ambient_color.y,ambient_color.z,1};
	shader_lighting->use();
	shader_lighting->setVec4(shader_lighting_uni_ambient,v);
}
void DUserClientFreeGlut::_setSunDir() {
	static float v[]={sun_dir.x,sun_dir.y,sun_dir.z,1};
	shader_lighting->use();
	shader_lighting->setVec4(shader_lighting_uni_sun_dir,v);

}
void DUserClientFreeGlut::_setSunColor() {
	static float v[]={sun_color.x,sun_color.y,sun_color.z,1};
	shader_lighting->use();
	shader_lighting->setVec4(shader_lighting_uni_sun_color,v);
}


//drawing
void DUserClientFreeGlut::lineTo(dvect to) {

	shader_base->useSetViewMatrix(glm::value_ptr(matrix_stack.back()));
	shader_base->setModelMatrix(glm::value_ptr(matrix_identity));
	shader_base->setVec4(shader_base_uni_color,curr_color);

	glBegin(GL_LINES);
	glVertex2f(cursor.x,cursor.y);
	glVertex2f(to.x,to.y);
	glEnd();
	/*
	glColor4fv(curr_color);
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glVertex2f(cursor.x,cursor.y);
	glVertex2f(to.x,to.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	*/
	cursor=to;
}
void DUserClientFreeGlut::quad(dvect pos,dvect size,bool fill) {
	/*
	glColor4fv(curr_color);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	dvect end=pos+size;
	glBegin(fill ? GL_QUADS : GL_LINE_LOOP);
	glVertex2f(pos.x,pos.y);
	glVertex2f(pos.x,end.y);
	glVertex2f(end.x,end.y);
	glVertex2f(end.x,pos.y);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	*/
}

void DUserClientFreeGlut::renderTexture(GLuint tex,dvect size) {
	glBindTexture(GL_TEXTURE_2D,tex);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

}

//returns true to render node, false to discard it
#define NODE_DO_RENDER_CHECK(node) \
	((node->force_render || node->in_frustrum) && node->visible)

void DUserClientFreeGlut::renderColor(DRenderNode* node) {
	if(!NODE_DO_RENDER_CHECK(node)) return;

	//render bounding boxes
	if(false) {
		glPushMatrix();
		glLoadMatrixf(glm::value_ptr(matrix));

		glDisable(GL_TEXTURE_2D);
		color(1,1,1,1);
		moveTo(node->bb.start);
		lineTo(dvect(node->bb.start.x,node->bb.end.y));
		lineTo(node->bb.end);
		lineTo(dvect(node->bb.end.x,node->bb.start.y));
		lineTo(node->bb.start);

		dvect center=(node->bb.start+node->bb.end)/2;
		color(1,0,0,1);
		moveTo(dvect(center.x+1,center.y+1));
		lineTo(dvect(center.x-1,center.y-1));
		moveTo(dvect(center.x+1,center.y-1));
		lineTo(dvect(center.x-1,center.y+1));

		glColor3f(1,1,1);

		glEnable(GL_TEXTURE_2D);

		glPopMatrix();
	}

	if(node->object && (node->object->render_flags & RENDER_FLAG_COLOR)) {

		shader_texture->use();
		shader_texture->setModelMatrix((float*)glm::value_ptr(node->matrix));
		shader_texture->setVec4(shader_texture_uni_color,color_white);

		switch(node->object->type) {
			case TYPE_CAR:
				glBindTexture(GL_TEXTURE_2D,
						((DUserClientFreeGlutObject*)node->object)->textures[0]->texture_id);
				glDrawArrays(GL_TRIANGLE_STRIP,0,4);
				break;
			case TYPE_TERRAIN: {
				DUserClientFreeGlutTerrain* t=(DUserClientFreeGlutTerrain*)node->object;

				for(int i=4;i>=0;i--) {
					if(t->textures[i]) {
						glActiveTexture(GL_TEXTURE0+i+1);
						glBindTexture(GL_TEXTURE_2D,t->textures[i]->texture_id);
					}
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D,t->alphas->texture_id);
				shader_splatting->use();
				shader_splatting->setModelMatrix((float*)glm::value_ptr(node->matrix));
				glDrawArrays(GL_TRIANGLE_STRIP,0,4);
				break;
			}
			case TYPE_ROAD: {
				DUserClientFreeGlutRoad* r=(DUserClientFreeGlutRoad*)node->object;

				glBindTexture(GL_TEXTURE_2D,r->texture->texture_id);

				//VBO currently holds whole mesh
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, r->vbo_index);
				glIndexPointer(GL_INT,0,(char*)NULL);

				glBindBufferARB(GL_ARRAY_BUFFER_ARB, r->vbo_pos);
				glVertexPointer(2,GL_FLOAT,0,(char *)NULL);

				glBindBufferARB(GL_ARRAY_BUFFER_ARB, r->vbo_coord);
				glTexCoordPointer(2,GL_FLOAT,0,(char*)NULL);

				glDrawElements(GL_TRIANGLE_STRIP,
						r->geometry->chunks[0]->num_indices,GL_UNSIGNED_INT,0);

				glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_quad_pos);
				glVertexPointer(2,GL_FLOAT,0,(char *)NULL);
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo_quad_coord);
				glTexCoordPointer(2,GL_FLOAT,0,(char*)NULL);

				/*
				for(int c=0;c<r->geom->chunks.size();c++) {
					DUserClientGeometryChunk* ch=r->geom->chunks[c];

					glEnable(GL_TEXTURE_2D);
					//glDisable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D,r->texture->texture_id);


					glBegin(GL_TRIANGLES);
					//glBegin(GL_POINTS);

					for(int i=0;i<ch->num_indices;i++) {

						glTexCoord2f(
							ch->tex_coords[ch->tex_indices[i]],
							ch->tex_coords[ch->tex_indices[i]+1]);
						glVertex3f(
							ch->vertices[ch->indices[i]],
							ch->vertices[ch->indices[i]+1],
							ch->vertices[ch->indices[i]+2]);
					}
					glEnd();

				}
				*/
				break;
			}
			case TYPE_CURVE: {
				DUserClientFreeGlutCurve* curve=(DUserClientFreeGlutCurve*)node->object;

				glBindTexture(GL_TEXTURE_2D,curve->texture->texture_id);

				vbo_use_pos(curve->vbo_pos);
				vbo_use_coord(curve_vbo_tex);

				vbo_draw((curve->current_size)/2);

				vbo_use_pos(vbo_quad_pos);
				vbo_use_coord(vbo_quad_coord);

				break;
			}
			case TYPE_OBJECT: {
				DUserClientFreeGlutObject* gl_object=(DUserClientFreeGlutObject*)node->object;

				if(gl_object->shader) {
					//gl_object->shader->use();
					gl_object->shader->useSetViewMatrix(glm::value_ptr(matrix));
					gl_object->shader->setModelMatrix(glm::value_ptr(node->matrix));

					for(int i=0;i<gl_object->float_unifs.size();i++) {
						gl_object->shader->setFloat(gl_object->float_unifs[i].id,
								gl_object->float_unifs[i].value);
					}
					for(int i=0;i<gl_object->float4_unifs.size();i++) {
						float value[]={
								gl_object->float4_unifs[i].value[0],
								gl_object->float4_unifs[i].value[1],
								gl_object->float4_unifs[i].value[2],
								gl_object->float4_unifs[i].value[3]
						};
						gl_object->shader->setVec4(gl_object->float4_unifs[i].id,
								value);
					}

				}

				for(int i=0;i<gl_object->textures.size();i++) {
					glActiveTexture(GL_TEXTURE0 + i);
					glBindTexture(GL_TEXTURE_2D,gl_object->textures[i]->texture_id);
				}

				glDrawArrays(GL_TRIANGLE_STRIP,0,4);
				glActiveTexture(GL_TEXTURE0);

				break;
			}
		}
	}

	if(node->post_draw) {
		if(!node->object) {
			//glLoadMatrixf(glm::value_ptr(matrix*node->matrix));
		}
		node->post_draw(node,this,node->post_draw_data);
		//glColor3f(1,1,1);
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderColor(*it);
	}
}

void DUserClientFreeGlut::renderNormal(DRenderNode* node,float angle) {
	//if(!node->visible || !node->in_frustrum) return;
	if(!NODE_DO_RENDER_CHECK(node)) return;

	//saveMatrix();
	//transform(node->pos,node->scale,node->angle);

	angle+=node->angle;
	if(node->object && (node->object->render_flags & RENDER_FLAG_NORMAL)) {

		float new_angle=0; //node->object->angle;
		dvect size=node->object->size;

		//angle+=new_angle;

		switch(node->object->type) {
			case TYPE_CAR: {

				glBindTexture(GL_TEXTURE_2D,((DUserClientFreeGlutObject*)node->object)->textures[1]->texture_id);

				float radx=degToRad(angle);

				shader_bumps->use();
				shader_bumps->setModelMatrix(glm::value_ptr(node->matrix));
				shader_bumps->setVec2(shader_bumps_uni_dir,cos(radx),sin(radx));

				vbo_draw(4);

				break;
			}
			case TYPE_TERRAIN: {
				DUserClientFreeGlutTerrain* t=(DUserClientFreeGlutTerrain*)node->object;

				shader_base->use();
				shader_base->setModelMatrix((float*)glm::value_ptr(node->matrix));

				static float color[]={0.5,0.5,1,1};
				shader_base->setVec4(shader_base_uni_color,color);
				vbo_draw(4);

				break;
			}
			case TYPE_OBJECT:
				break;
				/*
				glDisable(GL_TEXTURE_2D);
				glColor3f(0.5,0.5,1);
				vbo_draw(4);

				glEnable(GL_TEXTURE_2D);
				glColor3f(1,1,1);
				break;
				*/

		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderNormal(*it,angle);
	}
	//restoreMatrix();
}

void DUserClientFreeGlut::renderLights(DRenderNode* node,float angle,
		vector<DRenderNode*>* nodes) {
	if(!NODE_DO_RENDER_CHECK(node)) return;

	angle+=node->angle;
	if(node->object) {

		float new_angle=0;

		dvect size=node->object->size;

		angle+=new_angle;

		switch(node->object->type) {
			case TYPE_LIGHT: {

				DUserClientFreeGlutLight* light=(DUserClientFreeGlutLight*)node->object;

				shader_use(0);
				glm::vec4 my_pos=node->matrix*glm::vec4(
						(0.5-light->origin.x)*light->size.x,
						(0.5-light->origin.y)*light->size.y,
						0,1);

				if(config.flat_shadows && light->cast_shadows) {
					//rt_height->activate();
					rt_normal->activate();
					glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
					glClear(GL_COLOR_BUFFER_BIT);
					glDisable(GL_BLEND);

					shader_flat_shadow->use();
					shader_flat_shadow->setMat4(shader_flat_shadow_uni_screen_mat,
							glm::value_ptr(matrix));

					dvect3 my_pos_v=dvect3(my_pos.x,my_pos.y,my_pos.z);
					if(nodes) for(int i=0;i<nodes->size();i++) {
						renderFlatShadow2((*nodes)[i],0,my_pos_v,&node->bb);
					}
					glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
					glEnable(GL_BLEND);

					vbo_use_pos(vbo_quad_centered_pos);

					shader_use(shader_lighting_textured_PHandle);
				}
				glLoadMatrixf(glm::value_ptr(matrix*node->matrix));

				if(config.aa_enabled) {
					rt_final->activate();
				}
				else {
					rt_normal->deactivate();
				}

				shader_use(shader_lighting_textured_PHandle);

				float radx=degToRad(180-angle);

				shader_uniform_vec3(shader_lighting_textured_uni_dir,cos(radx),sin(radx),0);
				shader_uniform_vec2(shader_lighting_textured_uni_origin,light->origin);
				glBindTexture(GL_TEXTURE_2D,light->color->texture_id);

				//TODO: get rid of this
				glScalef(size.x,size.y,1);
				vbo_draw(4);

				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderLights(*it,angle,nodes);
	}
}



void DUserClientFreeGlut::renderLights2(DRenderNode* node,float angle,
		vector<DRenderNode*>* nodes) {
	if(!NODE_DO_RENDER_CHECK(node)) return;

	angle+=node->angle;
	if(node->object) {

		float new_angle=0;

		dvect size=node->object->size;

		angle+=new_angle;

		switch(node->object->type) {
			case TYPE_LIGHT: {

				DUserClientFreeGlutLight* light=(DUserClientFreeGlutLight*)node->object;

				shader_use(0);
				glm::vec4 my_pos=node->matrix*glm::vec4(
						(0.5-light->origin.x)*light->size.x,
						(0.5-light->origin.y)*light->size.y,
						0,1);

				if(config.flat_shadows && light->cast_shadows) {
					rt_normal->activate();
					glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
					glClear(GL_COLOR_BUFFER_BIT);
					glDisable(GL_BLEND);

					shader_flat_shadow->use();
					shader_flat_shadow->setMat4(shader_flat_shadow_uni_screen_mat,
							glm::value_ptr(matrix));

					dvect3 my_pos_v=dvect3(my_pos.x,my_pos.y,my_pos.z);
					if(nodes) for(int i=0;i<nodes->size();i++) {
						renderFlatShadow2((*nodes)[i],0,my_pos_v,&node->bb);
					}
					glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
					glEnable(GL_BLEND);

					vbo_use_pos(vbo_quad_centered_pos);

					shader_use(shader_lighting_textured_PHandle);
				}
				glLoadMatrixf(glm::value_ptr(matrix*node->matrix));

				if(config.aa_enabled) {
					rt_final->activate();
				}
				else {
					rt_normal->deactivate();
				}

				shader_use(shader_lighting_textured_PHandle);

				float radx=degToRad(180-angle);

				shader_uniform_vec3(shader_lighting_textured_uni_dir,cos(radx),sin(radx),0);
				shader_uniform_vec2(shader_lighting_textured_uni_origin,light->origin);
				glBindTexture(GL_TEXTURE_2D,light->color->texture_id);

				//TODO: get rid of this
				glScalef(size.x,size.y,1);
				vbo_draw(4);

				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderLights2(*it,angle,nodes);
	}
}



void DUserClientFreeGlut::renderShadow(DRenderNode* node,float angle,float height) {
	//if(!node->visible || !node->in_frustrum) return;
	if(!NODE_DO_RENDER_CHECK(node)) return;

	dvect sun_dir;

	height+=node->pos.z;

	angle+=node->angle;
	if(node->object && (node->object->render_flags & RENDER_FLAG_HEIGHT)) {

		dvect size=node->object->size;

		switch(node->object->type) {
			case TYPE_CAR: {

				float dangle=-degToRad(angle+180)+atan2(sun_dir.x,sun_dir.y);
				sun_dir=dvect(cos(dangle),sin(dangle)).normalize();
				float sun_tan=4;

				glBindTexture(GL_TEXTURE_2D,
						((DUserClientFreeGlutObject*)node->object)->textures[2]->texture_id);

				int layers=50*shadow_quality;

				float l1=((float)0+1)/(layers+1);
				dvect delta=sun_dir*sun_tan*(1-l1)/(size*layers);

				dvect offset1=delta*height;
				float offset[2]={delta.x,delta.y};

				shader_shadow->setModelMatrix(glm::value_ptr(node->matrix));
				shader_shadow->setVec2(shader_shadow_uni_offset,offset);
				glDrawArraysInstancedARB(GL_TRIANGLE_STRIP,0,4,layers);
				/*
				for(int i=0;i<layers;i++) {
					float l=((float)i+1)/(layers+1);

					glUniform1fARB(shader_shadow_uni_sub,l);

					offset[0]=offset1.x+delta.x*(float)i;
					offset[1]=offset1.y+delta.y*(float)i;

					shader_shadow->setVec2(shader_shadow_uni_offset,offset);
					vbo_draw(4);
				}
				*/

				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderShadow(*it,angle,height);
	}
}

//screen-space shadows
//less render calls, but slower because of overdraw
void DUserClientFreeGlut::renderShadow2() {
	//float dangle=atan2(sun_dir.x,sun_dir.y);
	//dvect sun_dir=dvect(cos(dangle),sin(dangle)).normalize();
	float sun_tan=0.1;

	float w=config.width;
	float h=config.height;
	matrix_init_ortho(w,h);

	glBindTexture(GL_TEXTURE_2D,rt_height->get_texture());

	dvect sun_dir2d=dvect(sun_dir.x,sun_dir.y);

	int layers=50;
	for(int i=0;i<layers;i++) {
		float l=((float)i+1)/(layers+1);

		dvect offset=sun_dir2d*sun_tan *(1-l) *dvect(w,h);

		glUniform1fARB(shader_shadow_uni_sub,1-l);

		glPushMatrix();
		glTranslatef(offset.x,offset.y,0);

//		glScalef(config.width,config.height,1);
		//glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		glBegin(GL_QUADS);
		glTexCoord2f(0,0); glVertex2f(0,0);
		glTexCoord2f(1,0); glVertex2f(w,0);
		glTexCoord2f(1,1); glVertex2f(w,h);
		glTexCoord2f(0,1); glVertex2f(0,h);
		glEnd();

		glPopMatrix();
	}

	matrix_init_perspective(config.width,config.height);
	//prerender();
	render_init_matrix();
	transformCam(-cam_pos,dvect(1,1),-cam_angle);
}

/*
//raycasted screen-space shadows
//no overdraw
void DUserClientFreeGlut::renderShadow3() {
	//float dangle=atan2(sun_dir.x,sun_dir.y);
	//dvect sun_dir=dvect(cos(dangle),sin(dangle)).normalize();
	float sun_tan=0.1;

	float w=config.width;
	float h=config.height;
	matrix_init_ortho(w,h);

	glBindTexture(GL_TEXTURE_2D,rt_height->get_texture());

	dvect sun_dir2d=dvect(sun_dir.x,sun_dir.y);

	glUseProgramObjectARB(shader_shadow3_PHandle);

	glBegin(GL_QUADS);
	glTexCoord2f(0,1); glVertex2f(0,0);
	glTexCoord2f(1,1); glVertex2f(w,0);
	glTexCoord2f(1,0); glVertex2f(w,h);
	glTexCoord2f(0,0); glVertex2f(0,h);
	glEnd();

	glUseProgramObjectARB(0);

	matrix_init_perspective(config.width,config.height);
	//prerender();
	render_init_matrix();
	transformCam(-cam_pos,dvect(1,1),-cam_angle);
}
*/

void DUserClientFreeGlut::renderHeight(DRenderNode* node,float angle,float height) {
	if(!NODE_DO_RENDER_CHECK(node)) return;

	height+=node->pos.z;

	angle+=node->angle;
	if(node->object && (node->object->render_flags & RENDER_FLAG_HEIGHT)) {

		float new_angle=0; //node->object->angle;
		dvect size=node->object->size;

		shader_texture->setModelMatrix((float*)glm::value_ptr(node->matrix));

		angle+=new_angle;

		switch(node->object->type) {
			case TYPE_CAR: {
				glBindTexture(GL_TEXTURE_2D,((DUserClientFreeGlutObject*)node->object)->textures[2]->texture_id);
				vbo_draw(4);
				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderHeight(*it,angle,height);
	}
}
void DUserClientFreeGlut::renderClouds() {
	if(!clouds_resource) return;
	glBindTexture(GL_TEXTURE_2D,((DUserClientFreeGlutResource*)clouds_resource)->texture_id);

	dvect s=clouds_offset;
	dvect e=s+clouds_scale;

	float coord_data[]={
			s.x,s.y,
			s.x,e.y,
			e.x,s.y,
			e.x,e.y
	};

	vbo_use_coord_own(coord_data);
	vbo_use_pos(vbo_clouds_pos);

	vbo_draw(4);

	vbo_use_pos(vbo_quad_pos);
	vbo_use_coord(vbo_quad_coord);
}

//deprecated
void DUserClientFreeGlut::renderFlatShadow(DRenderNode* node,float height,dvect3 origin,
		DBoundingBox* target_bb) {
	if(!node->visible || !node->bb.intersects(target_bb)) return;

	height+=node->pos.z;

	if(node->object) {

		glLoadMatrixf(glm::value_ptr(matrix));

		dvect size=node->object->size;

		switch(node->object->type) {
			case TYPE_CAR: {
				if(node->object->geometry && node->object->geometry->chunks.size()>0
						&& node->object->geometry->chunks[0]->num_vertices>4) {
					DUserClientGeometryChunk* c=node->object->geometry->chunks[0];

					//TODO: migrate to vertex shader

					glm::vec4 my_pos=node->matrix*glm::vec4(
							c->vertices[0],c->vertices[1],0,1);
					dvect prev_p=dvect(my_pos.x,my_pos.y);
					dvect prev_offset=(prev_p-dvect(origin.x,origin.y))*20000;

					dvect first_p=prev_p;
					dvect first_offset=prev_offset;

					glColor3f(1,1,1);
					for(int i=2;i<c->num_vertices;i+=2) {

						my_pos=node->matrix*glm::vec4(
								c->vertices[i],c->vertices[i+1],0,1);
						dvect p=dvect(my_pos.x,my_pos.y);
						dvect offset=(p-dvect(origin.x,origin.y))*20000;

						glBegin(GL_TRIANGLE_STRIP);
						glVertex2f(prev_p.x,prev_p.y);
						glVertex2f(prev_offset.x,prev_offset.y);
						glVertex2f(p.x,p.y);
						glVertex2f(offset.x,offset.y);
						glEnd();

						prev_p=p;
						prev_offset=offset;
					}

					glBegin(GL_TRIANGLE_STRIP);
					glVertex2f(prev_p.x,prev_p.y);
					glVertex2f(prev_offset.x,prev_offset.y);
					glVertex2f(first_p.x,first_p.y);
					glVertex2f(first_offset.x,first_offset.y);
					glEnd();

					glColor3f(0,0,0);
					glBegin(GL_TRIANGLE_FAN);
					for(int i=0;i<c->num_vertices;i+=2) {
						my_pos=node->matrix*glm::vec4(c->vertices[i],c->vertices[i+1],0,1);
						glVertex2f(my_pos.x,my_pos.y);
					}
					glEnd();
				}

				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderFlatShadow(*it,height,origin,target_bb);
	}
}


void DUserClientFreeGlut::renderFlatShadow2(DRenderNode* node,float height,dvect3 origin,
		DBoundingBox* target_bb) {
	if(!node->visible || !node->bb.intersects(target_bb)) return;

	height+=node->pos.z;

	if(node->object) {

		switch(node->object->type) {
			case TYPE_CAR: {

				if(node->object->geometry && node->object->geometry->chunks.size()>0
						&& node->object->geometry->chunks[0]->num_vertices>4) {
					DUserClientGeometryChunk* c=node->object->geometry->chunks[0];

					DUserClientFreeGlutObject* obj=(DUserClientFreeGlutObject*)node->object;

					if(c->gl_vbo_flat_shadow_pos==0) {
						int num_vbo_vertices=c->num_vertices+2;
						float* positions=new float[num_vbo_vertices*2];

						for(int i=0;i<c->num_vertices;i+=2) {
							positions[i*2+0]=c->vertices[i];
							positions[i*2+1]=c->vertices[i+1];
							positions[i*2+2]=c->vertices[i];
							positions[i*2+3]=c->vertices[i+1];
						}
						positions[c->num_vertices*2+0]=c->vertices[0];
						positions[c->num_vertices*2+1]=c->vertices[1];
						positions[c->num_vertices*2+2]=c->vertices[0];
						positions[c->num_vertices*2+3]=c->vertices[1];

						c->gl_vbo_flat_shadow_pos=vbo_create(num_vbo_vertices,positions);

						delete[] positions;
					}

					vbo_use_pos(c->gl_vbo_flat_shadow_pos);

					float vorigin[]={origin.x,origin.y,0,1};

					shader_flat_shadow->setMat4(shader_flat_shadow_uni_world_mat,
							glm::value_ptr(node->matrix));
					shader_flat_shadow->setVec4(shader_flat_shadow_uni_origin,
							vorigin);

					vbo_draw(c->num_vertices+2);
				}

				break;
			}
		}
	}

	for(vector<DRenderNode*>::iterator it=node->childs.begin();it<node->childs.end();it++) {
		renderFlatShadow2(*it,height,origin,target_bb);
	}
}




//recursively sorts node children by pos.z
//also calculate node matrices
//and bounding boxes
//not very fast
void DUserClientFreeGlut::sortChildren(DRenderNode* node) {

	int childs=node->childs.size();

	//sort
	for(int i=0;i<childs;i++) {
		int lowest=i;
		for(int n=i+1;n<childs;n++) {
			if(node->childs[n]->pos.z<node->childs[lowest]->pos.z) lowest=n;
		}
		//swap lowest with i
		if(lowest!=i) {
			DRenderNode* temp=node->childs[i];
			node->childs[i]=node->childs[lowest];
			node->childs[lowest]=temp;
		}
	}
	for(int i=0;i<childs;i++) {
		DRenderNode* c=node->childs[i];
		c->matrix=
				glm::rotate<float>(
				glm::translate(node->matrix,glm::vec3(c->pos.x,c->pos.y,c->pos.z)),
				c->angle,glm::vec3(0,0,1));

		sortChildren(c);
	}

	int merge_start=0;

	//apply object transformation to matrix
	if(node->object) {

		//calc BB

		glm::vec4 p1,p2,p3,p4;

		//exception for meshes
		if(node->object->type==TYPE_ROAD) {
			DBoundingBox* g_bb=&((DUserClientFreeGlutRoad*)node->object)->geometry->bb;
			p1=node->matrix*glm::vec4(g_bb->start.x,g_bb->start.y,0,1);
			p2=node->matrix*glm::vec4(g_bb->start.x,g_bb->end.y,0,1);
			p3=node->matrix*glm::vec4(g_bb->end.x,g_bb->start.y,0,1);
			p4=node->matrix*glm::vec4(g_bb->end.x,g_bb->end.y,0,1);
		}
		else {
			dvect size2=node->object->size/2;
			p1=node->matrix*glm::vec4(size2.x,size2.y,0,1);
			p2=node->matrix*glm::vec4(size2.x,-size2.y,0,1);
			p3=node->matrix*glm::vec4(-size2.x,-size2.y,0,1);
			p4=node->matrix*glm::vec4(-size2.x,size2.y,0,1);
		}

		node->bb.start.x=fmin(p1.x,fmin(p2.x,fmin(p3.x,p4.x)));
		node->bb.start.y=fmin(p1.y,fmin(p2.y,fmin(p3.y,p4.y)));

		node->bb.end.x=fmax(p1.x,fmax(p2.x,fmax(p3.x,p4.x)));
		node->bb.end.y=fmax(p1.y,fmax(p2.y,fmax(p3.y,p4.y)));

		glm::vec4 p=node->matrix*glm::vec4(0,0,0,1);

		//ignore on road meshes
		//TODO: get rid of this
		if(node->object->type!=TYPE_ROAD && node->object->type!=TYPE_LIGHT) {
			node->matrix=

					glm::scale(

					glm::translate(
					glm::rotate<float>(node->matrix,
							180/*+node->object->angle*/,glm::vec3(0,0,1)),

					glm::vec3(-node->object->size.x/2,
							-node->object->size.y/2,0)),

					glm::vec3(node->object->size.x,node->object->size.y,1));
		}
	}
	else if(node->childs.size()>0) {
		node->bb=node->childs[0]->bb;
		merge_start=1;
	}

	//nested BB
	for(int i=merge_start;i<node->childs.size();i++) {
		if(node->childs[i]->visible) {
			node->bb.merge(&node->childs[i]->bb);
		}
	}

	node->in_frustrum=(node->force_render || node->bb.intersects(&real_screen_bb));
}

//main render---
void DUserClientFreeGlut::render(long ts,duserclient_get_nodes get_nodes) {
	matrix_init_perspective(config.width,config.height);
	prerender();
	transformCam(-cam_pos,dvectone,-cam_angle);

//	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	root_node->matrix=glm::mat4(1.0f);
	sortChildren(root_node);

	glm::mat4 invmatrix=glm::inverse(matrix);
	float v=1;
	glm::vec4 screen_start=invmatrix*glm::vec4(-v,-v,0,1);
	glm::vec4 screen_end=invmatrix*glm::vec4(v,v,0,1);

	real_screen_bb.start=dvect(screen_start.x,screen_start.y);
	real_screen_bb.end=dvect(screen_end.x,screen_end.y);

	/*
	printf("view bb %f %f %f %f\n",
			real_screen_bb.start.x,real_screen_bb.start.y,
			real_screen_bb.end.x,real_screen_bb.end.y);
	*/

	vector<DRenderNode*>* nodes=NULL;
	if(get_nodes) nodes=get_nodes(&real_screen_bb);

	float* matrix_ptr=glm::value_ptr(matrix);

	shader_texture->useSetViewMatrix(matrix_ptr);
	shader_splatting->useSetViewMatrix(matrix_ptr);
	shader_bumps->useSetViewMatrix(matrix_ptr);
	shader_base->useSetViewMatrix(matrix_ptr);
	shader_shadow->useSetViewMatrix(matrix_ptr);
	shader_text->useSetViewMatrix(matrix_ptr);

	switch(config.render_mode) {
		case RENDER_MODE_SHADED: {

			glClear(GL_COLOR_BUFFER_BIT);

			vbo_use_pos(vbo_quad_pos);
			vbo_use_coord(vbo_quad_coord);

			rt_color->activate();
			glClear(GL_COLOR_BUFFER_BIT);
			renderColor(root_node);

			if(nodes) for(int i=0;i<nodes->size();i++) {
				DRenderNode* node=(*nodes)[i];
				renderColor(node);
			}

			glm::vec4 origin_ss=matrix*glm::vec4(0,0,0,1);

			rt_normal->activate();
			glClear(GL_COLOR_BUFFER_BIT);
			renderNormal(root_node);
			if(nodes) for(int i=0;i<nodes->size();i++)
				renderNormal((*nodes)[i]);

			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			glEnable(GL_BLEND);

			rt_final->activate();
			glClear(GL_COLOR_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,rt_normal->get_texture());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,rt_color->get_texture());

			glActiveTexture(GL_TEXTURE2);

			shader_use(shader_lighting_textured_PHandle);

			vbo_use_pos(vbo_quad_centered_pos);

			bool use_new_lights=false;
			if(use_new_lights) {
				glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_TRUE);
				glClearColor(0.0f,0.0f,0.0f,0.0f);
				rt_color->activate();
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(0.0f,0.0f,0.0f,1.0f);
				rt_final->activate();

				renderLights2(root_node);
				if(nodes) for(int i=0;i<nodes->size();i++)
					renderLights2((*nodes)[i],0,nodes);

				vbo_use_pos(vbo_quad_pos);

				glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			}
			else {
				rt_final->activate();

				renderLights(root_node);
				if(nodes) for(int i=0;i<nodes->size();i++)
					renderLights((*nodes)[i],0,nodes);

				vbo_use_pos(vbo_quad_pos);
			}

			glActiveTexture(GL_TEXTURE0);

			glUseProgramObjectARB(0);

			rt_height->activate();
			glClear(GL_COLOR_BUFFER_BIT);

			glViewport(0,0,config.width*shadow_quality,config.height*shadow_quality);

			renderClouds();

			if(config.shadows_on==1) {
				glClear(GL_COLOR_BUFFER_BIT);

				shader_texture->use();
				shader_texture->setVec4(shader_texture_uni_color,color_red);

				renderHeight(root_node);
				if(nodes) for(int i=0;i<nodes->size();i++)
					renderHeight((*nodes)[i]);

				shader_texture->setVec4(shader_texture_uni_color,color_white);
			}

			if(config.aa_enabled)
				rt_final->activate();
			else {
				rt_height->deactivate();
			}

			if(config.shadows_on==1) {
				rt_height->activate();
				glBlendEquation(GL_MAX);

				shader_shadow->use();

				renderShadow(root_node);
				if(nodes) for(int i=0;i<nodes->size();i++)
					renderShadow((*nodes)[i]);
				//renderShadow3();

				glBlendEquation(GL_FUNC_ADD);
			}

			glViewport(0,0,config.width,config.height);

			//shader_use(shader_lighting_PHandle);
			shader_lighting->use();

			clouds_offset+=clouds_vel*ts;
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D,rt_height->get_texture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D,rt_normal->get_texture());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,rt_color->get_texture());

			if(config.aa_enabled)
				rt_final->activate();
			else {
				rt_height->deactivate();
			}

			matrix_init_ortho(300,200);

			dvect p1=dvectzero;
			dvect p2=dvect(300,200);

			shader_lighting->useSetViewMatrix(glm::value_ptr(matrix));

			//sun pass
			vbo_use_pos(vbo_screen_pos);
			vbo_use_coord(vbo_quad_coord);
			vbo_draw(4);

			if(config.aa_enabled) {
				rt_final->deactivate();

				glViewport(0,0,real_window_width,real_window_height);
				//glViewport(0,0,100,100);

				/*
				matrix=glm::scale(
						glm::translate(
								matrix,
								glm::vec3(50,50,0)),
						glm::vec3(0.5,0.5,1));

				matrix=glm::scale(
						glm::translate(
								matrix,
								glm::vec3(0,0,0)),
						glm::vec3(1,1,1));
				*/

				shader_aa->useSetViewMatrix(glm::value_ptr(matrix));
				shader_aa->setModelMatrix(glm::value_ptr(matrix_identity));
				glBindTexture(GL_TEXTURE_2D,rt_final->get_texture());
				vbo_draw(4);
			}

			vbo_use_pos(vbo_quad_pos);
			vbo_use_coord(vbo_quad_coord);

			glDisable(GL_BLEND);

			root_node_overlay->matrix=glm::mat4(1.0f);
			//matrix=glm::mat4(1.0f);
			matrix_init_ortho(300,200);

			float* matrix_ptr=glm::value_ptr(matrix);

			shader_texture->useSetViewMatrix(matrix_ptr);
			shader_splatting->useSetViewMatrix(matrix_ptr);
			shader_bumps->useSetViewMatrix(matrix_ptr);
			shader_base->useSetViewMatrix(matrix_ptr);
			shader_shadow->useSetViewMatrix(matrix_ptr);
			shader_text->useSetViewMatrix(matrix_ptr);

			sortChildren(root_node_overlay);
			renderColor(root_node_overlay);

			break;
		}
		case RENDER_MODE_SIMPLE:
			glClear(GL_COLOR_BUFFER_BIT);
			renderColor(root_node);
			if(nodes) for(int i=0;i<nodes->size();i++) {
				DRenderNode* node=(*nodes)[i];
				renderColor(node);
			}

			matrix_init_ortho(300,200);

			float* matrix_ptr=glm::value_ptr(matrix);

			shader_texture->useSetViewMatrix(matrix_ptr);
			shader_splatting->useSetViewMatrix(matrix_ptr);
			shader_bumps->useSetViewMatrix(matrix_ptr);
			shader_base->useSetViewMatrix(matrix_ptr);
			shader_shadow->useSetViewMatrix(matrix_ptr);
			shader_text->useSetViewMatrix(matrix_ptr);

			root_node_overlay->matrix=glm::mat4(1.0f);
			sortChildren(root_node_overlay);
			renderColor(root_node_overlay);
			break;
	}

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	/*
	dvect p2=dvect(0,0);
	dvect p3=dvect(150,100);
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D,rt_normal->get_texture());
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(1,0); glVertex2f(p3.x,p3.y);
	glTexCoord2f(0,0); glVertex2f(p2.x,p3.y);
	glTexCoord2f(1,1); glVertex2f(p3.x,p2.y);
	glTexCoord2f(0,1); glVertex2f(p2.x,p2.y);
	glEnd();
	 */

	/*
	matrix_init_ortho(300,200);
	shader_texture->useSetViewMatrix(glm::value_ptr(matrix));
	shader_texture->setModelMatrix(glm::value_ptr(matrix_identity));

	dvect p1=dvect(0,100);
	dvect p4=dvect(150,200);
	glBindTexture(GL_TEXTURE_2D,rt_height->get_texture());
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(p1.x,p1.y);
	glTexCoord2f(1,0); glVertex2f(p4.x,p1.y);
	glTexCoord2f(0,1); glVertex2f(p1.x,p4.y);
	glTexCoord2f(1,1); glVertex2f(p4.x,p4.y);
	glEnd();
	*/

	postrender();


    GLenum error = glGetError();
    if(error!=GL_NO_ERROR) {
        //printf("GL ERROR: %s\n",gluErrorString(error));
    }
}

//private 

void DUserClientFreeGlut::createWindow(string title,int width,int height,bool fullscreen,int x,int y) {
	glutInitWindowSize(width,height);
	glutInitWindowPosition(x,y);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE /*| GLUT_DEPTH*/);
	if(fullscreen) {
		glutFullScreen();
	}
	window=glutCreateWindow(title.c_str());
	glutReshapeFunc(clientResize);
	glutKeyboardFunc(clientKeyboard);
	glutKeyboardUpFunc(clientKeyboardUp);
	glutSpecialFunc(clientKeyboardSpecial);
	glutSpecialUpFunc(clientKeyboardSpecialUp);
	glutMotionFunc(clientPassiveMotion);
	glutPassiveMotionFunc(clientPassiveMotion);
	glutMouseFunc(clientMouse);
}

void DUserClientFreeGlut::matrix_init_perspective(int w,int h) {
	//cout<<"gl resize\n";
	glViewport(0,0,w,h);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

	float z=32 / 2;

//	float w2=w/h*z/2;
//	float h2=z/2;

	/*
	float w2=(w/h)/2 * z;
	float h2= z/2 ;
	*/

//	s1=M*w1;
//	w1=I*s1;



	//200x150

	float w2=(1+((float)w/(float)h-1)/2)*z;
	//float w2=(1+0.33/2)*z;
	float h2=z;

	screen_bb.start=dvect(-w2,-h2);
	screen_bb.end=dvect(w2,h2);

	matrix=glm::ortho<float>(-w2,w2,-h2,h2,-100,100);

	matrix_stack.clear();
	matrix_stack.push_back(matrix);
//	glLoadMatrixf(glm::value_ptr(matrix));

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
}
void DUserClientFreeGlut::matrix_init_ortho(int w,int h) {

	//glViewport(0,0,w,h);
	glViewport(0,0,config.width,config.height);

//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();

	//glOrtho(0,w,h,0,-1,1);
	matrix=glm::ortho<float>(0,w,h,0,-1,1);
//	glLoadMatrixf(glm::value_ptr(matrix));

	matrix_stack.clear();
	matrix_stack.push_back(matrix);

	screen_bb.start=dvectzero;
	screen_bb.end=dvect(w,h);

//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();


}
void DUserClientFreeGlut::_resizeWindow(int w,int h) {
	config.width=w*render_upscale;
	config.height=h*render_upscale;
	matrix_init_perspective(config.width,config.height);

	/*
	init_rtts(w,h);

	shader_lighting->setVec2(shader_lighting->getUniform("screenSizeInv"),
			1/((float)config.width),
			1/((float)config.height));
	*/

	printf("resized to %d %d\n",w,h);
}
void DUserClientFreeGlut::_keyboard(int key,bool down) {
	//cout<<"glut keydown "<<down<<" "<<key<<endl;
	keys[key]=down;
}
void DUserClientFreeGlut::_mouseMove(int x,int y) {
	mouse_pos=dvect(x,y);
}
void DUserClientFreeGlut::_mouseButton(int key,bool down) {
	mouse_keys[key]=down;
}

void DUserClientFreeGlut::closeWindow() {
	cout<<"gl deinit\n";
	glutDestroyWindow(window);
}
void clientResize(int w,int h) {
	client->_resizeWindow(w,h);
}
void clientKeyboard(unsigned char key,int x,int y) {
	client->_keyboard(key,true);
}
void clientKeyboardUp(unsigned char key,int x,int y) {
	client->_keyboard(key,false);
}
void clientKeyboardSpecial(int key,int x,int y) {
	client->_keyboard(key+100,true);
}
void clientKeyboardSpecialUp(int key,int x,int y) {
	client->_keyboard(key+100,false);
}
void clientPassiveMotion(int x,int y) {
	client->_mouseMove(x,y);
}
void clientMouse(int button,int state,int x,int y) {
	client->_mouseButton(button,state==GLUT_DOWN);
}

void DUserClientFreeGlut::init_shader_lighting() {
	shader_lighting=new DGLShader();
	if(!shader_lighting->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/lighting.glsl")) {
		printf("Lighting shader failed to compile.\n");
		return;
	}
	shader_lighting_uni_sun_color=shader_lighting->getUniform("sun_color");
	shader_lighting_uni_sun_dir=shader_lighting->getUniform("sun_dir");
	shader_lighting_uni_ambient=shader_lighting->getUniform("ambient");
//	shader_lighting_uni_clouds_offset=shader_lighting->getUniform("clouds_offset");
//	shader_lighting_uni_clouds_scale=shader_lighting->getUniform("clouds_scale");
	shader_lighting_uni_shadow_quality=shader_lighting->getUniform("shadow_quality");
	shader_lighting->setModelMatrix(glm::value_ptr(matrix_identity));
	shader_lighting->setVec2(shader_lighting->getUniform("screenSizeInv"),1/((float)config.width),1/((float)config.height));
	shader_lighting->setFloat(shader_lighting_uni_shadow_quality,1);

	glUniform1iARB(shader_lighting->getUniform("texColor"),0);
	glUniform1iARB(shader_lighting->getUniform("texNormal"),1);
	glUniform1iARB(shader_lighting->getUniform("texHeight"),2);
	glUniform1iARB(shader_lighting->getUniform("texClouds"),3);

}
void DUserClientFreeGlut::setShadowQuality(float q) {
	shader_lighting->use();
	shader_lighting->setFloat(shader_lighting_uni_shadow_quality,q);
	shadow_quality=q;
}

void DUserClientFreeGlut::init_shader_lighting_textured() {
	char *FProgram=get_file_content("shader/lighting_textured.glsl");
	if(!FProgram) return;	//no file

	int i;
	char* s;

	shader_lighting_textured_PHandle=glCreateProgramObjectARB();
	shader_lighting_textured_FSHandle=glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	glShaderSourceARB(shader_lighting_textured_FSHandle,1,(const char**)&FProgram,NULL);
	glCompileShaderARB(shader_lighting_textured_FSHandle);

	GLint compile_status;
	glGetObjectParameterivARB(shader_lighting_textured_FSHandle,GL_OBJECT_COMPILE_STATUS_ARB,&compile_status);
	if(compile_status==GL_FALSE) {
		// Print the compilation log.
		glGetObjectParameterivARB(shader_lighting_textured_FSHandle,GL_OBJECT_COMPILE_STATUS_ARB,&i);
		s=new char[32768];
		glGetInfoLogARB(shader_lighting_textured_FSHandle,32768,NULL,s);
		printf("Textured lighting shader compilation failed: %s\n", s);
		delete(s);
		delete(FProgram);
		shader_lighting_textured_PHandle=0;
		shader_lighting_textured_FSHandle=0;
		return;
	}

	glAttachObjectARB(shader_lighting_textured_PHandle,shader_lighting_textured_FSHandle);
	glLinkProgramARB(shader_lighting_textured_PHandle);
	glUseProgramObjectARB(shader_lighting_textured_PHandle);

	glUniform1iARB(glGetUniformLocationARB(shader_lighting_textured_PHandle,"texColor"),0);
	glUniform1iARB(glGetUniformLocationARB(shader_lighting_textured_PHandle,"texNormal"),1);
	glUniform1iARB(glGetUniformLocationARB(shader_lighting_textured_PHandle,"texLight"),2);
	//glUniform1iARB(glGetUniformLocationARB(shader_lighting_textured_PHandle,"texHeight"),3);
	//glUniform1iARB(glGetUniformLocationARB(shader_lighting_textured_PHandle,"texShadow"),4);
	shader_uniform_vec2(glGetUniformLocationARB(shader_lighting_textured_PHandle,"screenSizeInv"),1/((float)config.width),1/((float)config.height));
	shader_lighting_textured_uni_dir=glGetUniformLocationARB(shader_lighting_textured_PHandle,"dir");
	shader_lighting_textured_uni_origin=glGetUniformLocationARB(shader_lighting_textured_PHandle,"origin");

	glUseProgramObjectARB(0);

	cout<<"Textured lighting shader compiled OK!\n";
	delete[] FProgram;
}

void DUserClientFreeGlut::init_shader_bumps() {
	shader_bumps=new DGLShader();
	if(!shader_bumps->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/bumps.glsl")) {
		printf("Bumps shader failed to compile.\n");
		return;
	}
	shader_bumps_uni_dir=shader_bumps->getUniform("dir");
}
void DUserClientFreeGlut::init_shader_shadow() {
	shader_shadow=new DGLShader();
	if(!shader_shadow->compileFullFromFile(
			"shader/shadow_vert.glsl",
			"shader/shadow_frag.glsl")) {
		printf("Shadow shader failed to compile.\n");
		return;
	}
	shader_shadow_uni_sub=shader_shadow->getUniform("sub");
	shader_shadow_uni_offset=shader_shadow->getUniform("offset");
}
void DUserClientFreeGlut::init_shader_splatting() {
	shader_splatting=new DGLShader();
	shader_splatting->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/splatting.glsl");
}
void DUserClientFreeGlut::init_shader_aa() {
	shader_aa=new DGLShader();
	shader_aa->compileFullFromFile("shader/base_vert.glsl","shader/fxaa.glsl");
	shader_aa->setFloat(shader_aa->getUniform("rt_w"),config.width);
	shader_aa->setFloat(shader_aa->getUniform("rt_h"),config.height);
}
void DUserClientFreeGlut::init_shader_text() {
	shader_text=new DGLShader();

	//if(!shader_text->compileFromFile("shader/text.glsl")) {
	if(!shader_text->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/text.glsl")) {
		printf("Can't compile text shader.\n");
		return;
	}
	shader_text_uni_color=shader_text->getUniform("color");
	shader_text->setModelMatrix(glm::value_ptr(matrix_identity));
}
void DUserClientFreeGlut::init_shader_flat_shadow() {
	shader_flat_shadow=new DGLShader();
	if(!shader_flat_shadow->compileFullFromFile(
			"shader/flat_shadow_vert.glsl",
			"shader/flat_shadow_frag.glsl")) {
		printf("Can't compile flat shadow shader.\n");
		return;
	}
	shader_flat_shadow_uni_world_mat=shader_flat_shadow->getUniform("world_mat");
	shader_flat_shadow_uni_screen_mat=shader_flat_shadow->getUniform("screen_mat");
	shader_flat_shadow_uni_origin=shader_flat_shadow->getUniform("origin");
}
void DUserClientFreeGlut::init_shader_base() {
	shader_base=new DGLShader();
	if(!shader_base->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/base_frag.glsl")) {
		printf("Can't compile base shader.\n");
		return;
	}
	shader_base_uni_color=shader_base->getUniform("color");
	shader_base->setVec4(shader_base_uni_color,color_white);
}
void DUserClientFreeGlut::init_shader_texture() {
	shader_texture=new DGLShader();
	if(!shader_texture->compileFullFromFile(
			"shader/base_vert.glsl",
			"shader/texture_frag.glsl")) {
		printf("Can't compile base texture shader.\n");
		return;
	}
	shader_texture_uni_color=shader_base->getUniform("color");
	shader_texture->setVec4(shader_texture_uni_color,color_white);
}

void DUserClientFreeGlut::init_vbos() {
	//TODO: get rid of limited clouds range
	float x=3000;

	float pos[]={ 0.0,0.0, 0.0,1.0, 1.0,0.0, 1.0,1.0 };
	float pos_c[]={ 0.5,-0.5, 0.5,0.5, -0.5,-0.5, -0.5,0.5};
	float tex[]={ 0.0,0.0, 0.0,1.0, 1.0,0.0, 1.0,1.0 };
	float screen_pos[]={0,0, 300,0, 0,200, 300,200};
	float clouds_pos[]={ x,x, x,-x, -x,x, -x,-x };

	vbo_quad_pos=vbo_create(4,pos);
	vbo_quad_centered_pos=vbo_create(4,pos_c);
	vbo_quad_coord=vbo_create(4,tex);
	vbo_screen_pos=vbo_create(4,screen_pos);
	vbo_text_coord=vbo_create_text_coord(10,10);
	//vbo_clouds_pos=vbo_create(4,clouds_pos);
	vbo_clouds_pos=vbo_create(4,clouds_pos);
}

void DUserClientFreeGlut::shader_uniform_vec2(GLint uni,float x,float y) {
	GLfloat v[2]={x,y};
	glUniform2fvARB(uni,1,v);
}
void DUserClientFreeGlut::shader_uniform_vec2(GLint uni,dvect v) {
	shader_uniform_vec2(uni,v.x,v.y);
}
void DUserClientFreeGlut::shader_uniform_vec3(GLint uni,float x,float y,float z) {
	GLfloat v[3]={x,y,z};
	glUniform3fvARB(uni,1,v);
}
void DUserClientFreeGlut::shader_uniform_vec3(GLint uni,dvect3 v) {
	shader_uniform_vec3(uni,v.x,v.y,v.z);
}


//gl stuff
void DUserClientFreeGlut::initGL() {
	cout<<"gl init\n";
//	glShadeModel(GL_SMOOTH);
	glClearColor(0.0,0.0,0.0,1.0);
	glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	glEnable(GL_COLOR_MATERIAL);
//	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_INDEX_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void DUserClientFreeGlut::screenshot(char* buffer) {
	glReadPixels(0,0,config.width,config.height,GL_RGB,GL_UNSIGNED_BYTE,buffer);
}

DGLRenderTarget* DUserClientFreeGlut::new_render_target() {
	return new DGLFBO();
}

void DUserClientFreeGlut::text(DResource* res,string text,float size) {
	DUserClientFreeGlutFont* f=(DUserClientFreeGlutFont*)res;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,f->texture_id);
	glDisable(GL_BLEND);

	shader_text->use();
	shader_text->setVec4(shader_text_uni_color,curr_color);

	dvect size_vect=dvect(size,size);

	float x=0;
	int rows=10;

	dvect p1=dvectzero;

	for(int i=0;i<text.length();i++) {
		int chr=text[i]-32;

		if(text[i]==' ') {
			x+=size/3;
			continue;
		}

		float spacing=f->spacings[chr-1] * size;
		float offset=f->offsets[chr-1]* size;

//		printf("char %c i %d sp %f\n",text[i],chr,f->spacings[chr]);

		/*
//		dvect c1=dvect(chr%rows,(chr-1)/rows)/rows;
//		dvect c2=c1+(dvect(-1,1)/rows);
		dvect c1=dvect(chr%rows,(chr-1)/rows+1)/rows;
		dvect c2=c1+(dvect(-1,-1)/rows);

		dvect p1=dvect(x,0);
		dvect p2=p1+dvect(size,size);

		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(c2.x,c2.y); glVertex2f(p1.x,p1.y);
		glTexCoord2f(c1.x,c2.y); glVertex2f(p2.x,p1.y);
		glTexCoord2f(c2.x,c1.y); glVertex2f(p1.x,p2.y);
		glTexCoord2f(c1.x,c1.y); glVertex2f(p2.x,p2.y);
		glEnd();
		*/
		p1.x=x-offset;
		dvect p2=p1+size_vect;

		float pos[]={
				p1.x,p1.y,
				p2.x,p1.y,
				p1.x,p2.y,
				p2.x,p2.y };


		vbo_use_coord(vbo_text_coord[chr]);
		//vbo_use_coord(vbo_quad_coord);

		vbo_use_pos_own(pos);
		vbo_draw(4);

		x+=spacing;
	}

	vbo_use_pos(vbo_quad_pos);
	vbo_use_coord(vbo_quad_coord);

	/*
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	*/
	glEnable(GL_BLEND);
}





