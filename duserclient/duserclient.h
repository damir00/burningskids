
#ifndef _R2DUSERCLIENT_H_
#define _R2DUSERCLIENT_H_

#include "dresource.h"
#include "dvect.h"
#include "dvect3.h"
#include "duserclientobject.h"
#include "duserclientgeometry.h"
#include "dparticle.h"
#include "drendernode.h"
#include "dutils.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

enum DUserClientRenderMode { RENDER_MODE_SIMPLE, RENDER_MODE_SHADED };


class DUserClientConfig {
	void init(string _title,int _width, int _height, bool _fullscreen=false,int x=0,int y=0) {
		title=_title;
		width=_width;
		height=_height;
		fullscreen=_fullscreen;
		render_mode=RENDER_MODE_SHADED;
		shadows_on=true;
		shadow_quality=1;
		flat_shadows=true;
		aa_enabled=1;
		window_x=x;
		window_y=y;
	}
public:
	string title;
	int width;
	int height;
	bool fullscreen;
	DUserClientRenderMode render_mode;
	bool shadows_on;
	float shadow_quality;	//0-1
	bool flat_shadows;
	bool aa_enabled;
	int window_x;
	int window_y;

	DUserClientConfig() {
		init("Hello user",800,600,false);
	}
	DUserClientConfig(string _title,int _width, int _height, bool _fullscreen=false,
			int x=0,int y=0) {
		init(_title,_width,_height,_fullscreen,x,y);
	}
};

/*
enum DUserClientEventType {
	KEYDOWN,
	KEYUP,
	CLOSE
};
class DUserClientEvent {
	DUserClientEventType type;
};
*/

typedef vector<DRenderNode*> *(*duserclient_get_nodes)(DBoundingBox* bb);

class DUserClient {
protected:

	vector<DUserClientObject*> objects;
	vector<DParticle*> particles;
	DRenderNode* root_node;
	DRenderNode* root_node_overlay;

	bool prev_keys[256];
	bool keys[256];

	bool mouse_keys[16];
	bool mouse_prev_keys[16];
	dvect mouse_pos;

	//drawing
	dvect cursor;
	float curr_color[4];

	dvect cam_pos;
	float cam_angle;

	dvect3 ambient_color;
	dvect3 sun_color;
	dvect3 sun_dir;
	DResource* clouds_resource;
	dvect clouds_scale;
	dvect clouds_vel;
	dvect clouds_offset;

	virtual void _setAmbient() {}
	virtual void _setSunDir() {}
	virtual void _setSunColor() {}

public:
	DUserClientConfig config;
	//DRenderOctree* octree;

	DUserClient(DUserClientConfig config);
	~DUserClient();

	//calcs world-space BB for node and all it's children
	//void nodeCalcBB(DRenderNode* node);

	virtual bool supportsResourceType(DResourceType type);
	virtual DResource* loadResource(DResourceType type,string file);
	virtual DResource* loadResourceImage(char* data,int width,int height,int depth) {
		return 0;
	}
	virtual void unloadResource(DResource* resource);

	DUserClientObject* loadObject(DResource* resource);
	virtual DUserClientObject* createObject(dvect size=dvect(1,1));	//doesn't queue the object for render
	virtual DUserClientObject* createCar(DResource* res_color,DResource* res_normal,DResource* res_height,dvect size=dvect(1,1)) { return NULL; }
	virtual DUserClientObject* createTerrain(DResource* colors[4],DResource* alphas,dvect size=dvect(1,1)) { return NULL; }

	//origin: local light origin (0,0)-(1,1)
	virtual DUserClientObject* createLight(DResource* resource,dvect size=dvect(1,1),
			dvect origin=dvect(0.5,0.5)) { return NULL; }

	virtual DUserClientObject* createRoad(DUserClientGeometry* geom,DResource* texture) { return NULL; }
	virtual DUserClientObject* createCurve(DResource* resource,float width) { return NULL; }

	virtual void releaseCurve(DUserClientObject* curve) {}


	DRenderNode * getRootNode();
	DRenderNode * getOverlayNode();

	/*
	void nodeSetDynamic(DRenderNode* node);	//call only once
	void nodeRehash(DRenderNode* node);		//call everytime static node is moved

	void quadtreeBuild(dvect size,int depth);
	void quadtreeReset();
	*/

	//handle input and call render
	void update(long ts,duserclient_get_nodes get_nodes=NULL);

	void renderNode(DRenderNode* node);
	virtual void saveMatrix();
	virtual void restoreMatrix();
	virtual void transform(dvect pos,dvect scale,double rot);
	virtual void transform(dvect3 pos,dvect3 scale,double rot);
	virtual void transformCam(dvect pos,dvect scale,double rot);
	void camera(dvect pos,double angle=0);

	//render handles prerender,postrender and calls object and particle rendering.
	//override if you want to handle this yourself
	virtual void render(long ts,duserclient_get_nodes get_nodes=NULL);

	virtual void prerender();
	virtual void postrender();
	virtual void renderObject(DUserClientObject* obj);
	virtual void renderParticle(DParticle* particle);

	void setAmbient(dvect3 ambient);
	void setSunDir(dvect3 dir);
	void setSunColor(dvect3 color);
	void setCloudsResource(DResource* r);
	void setCloudsScale(dvect scale);
	void setCloudsVel(dvect vel);
	virtual void setShadowQuality(float q) {};

	//input
	virtual bool isKeyDown(int key);
	virtual bool isKeyPressed(int key);
	virtual dvect getMousePos();
	virtual bool isMouseDown(int key);
	virtual bool isMousePressed(int key);

	//drawing api
	virtual void color(float r,float g,float b,float a=1);
	virtual void moveTo(dvect pos);
	virtual void lineTo(dvect to);
	virtual void text(DResource* res,string text,float size) {};
	virtual void quad(dvect pos,dvect size,bool fill) {};

	virtual void screenshot(char* buffer) {};

	//particles
	void addParticle(dvect pos,DParticles* generator);
};

#endif

