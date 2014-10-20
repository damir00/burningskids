
#include "r2dlib.h"
#include "duserclientfreeglut.h"

#include "gametrack.h"
#include "gamelevel.h"
#include "gamecar.h"
#include "dtimelimiter.h"
#include "gameutils.h"
#include "gamegraphic.h"
#include "gameresourcemanager.h"
#include "gameaudio.h"

#include "luacarcontroller.h"
#include "xmlparser.h"
#include "propinterface.h"

#include "gamemenu.h"
#include "gamegraph.h"

#include <iostream>
#include <string>
#include <vector>

#include "il.h"
#include "ilu.h"
#include "ilut.h"

#include <stdio.h>

#include "luatools.h"
#include "luaapi.h"

using namespace std;

int r2d_lua_pcall(lua_State* l) {
	cout<<"LUA WRONG!\n";
	lua_Debug deb;

	lua_getstack(l,0,&deb);
	lua_getinfo(l,"nSl",&deb);

	cout<<endl;
	cout<<"Namewhat:\t"<<deb.namewhat<<endl;
	cout<<"What:\t"<<deb.what<<endl;
	cout<<"Line:\t"<<deb.currentline<<endl;
	cout<<"Line def:\t"<<deb.linedefined<<endl;
	printf("Name:\t%s\n",deb.name);
	cout<<endl;

	return 0;
}

void r2d_post_draw(DRenderNode* node,void* client,void* data);
void r2d_hud_draw(DRenderNode* node,void* client,void* data);
void r2d_menu_draw(DRenderNode* node,void* client,void* data);

//set to true to ignore time delat at next frame
//set after loading resources that takes a lot of time
bool ignore_frame_delta=false;

int current_fps=0;
int current_frametime=0;

GameCar *player_car=NULL;
LuaCarController** car_controllers=NULL;

dvect cam_pos;
GameLevel* level=NULL;

DUserClient *_client;
GameGraphic* graphic;
GameResourceManager* manager;

int cam_car=0;

GameMenu* hack_menu;		//last created menu
GameWidget* hack_widget;	//last created widget

vector<GameMenu*> menu_stack;
vector<GameMenu*> menu_cache;

PropInterface* global_props;


#define LUA_PROP_INTERFACE_DEFS_GLOBAL \
	    luabind::def("getPropInt",&lua_getPropInt), \
	    luabind::def("setPropInt",&lua_setPropInt), \
	    luabind::def("getPropFloat",&lua_getPropFloat), \
	    luabind::def("setPropFloat",&lua_setPropFloat), \
	    luabind::def("getPropString",&lua_getPropString), \
	    luabind::def("setPropString",&lua_setPropString)


//lua api
void lua_text(string text,float size);
LuaGameCar lua_getPlayer();
LuaGameLevel lua_getLevel();
LuaUserClient lua_getUserClient() { return LuaUserClient(_client); }
LuaGameResourceManager lua_getManager() { return LuaGameResourceManager(manager); }
LuaGameMenu lua_getMenu() { return LuaGameMenu(hack_menu); }
LuaGameWidget lua_getWidget() { return LuaGameWidget(hack_widget); }
LuaGameWidget lua_addWidget(LuaGameMenu menu,string id,dvect pos,dvect size);
LuaRenderNode lua_getHudNode() { return LuaRenderNode(graphic->node_hud); }
LuaGameMenu lua_openMenu(string filename);
bool lua_loadLevel(string level);
void lua_unloadLevel();

int lua_getPropInt(string id) { return global_props->getPropInt(id); }
void lua_setPropInt(string id,int value) { global_props->setPropInt(id,value); }
float lua_getPropFloat(string id) { return global_props->getPropFloat(id); }
void lua_setPropFloat(string id,float value) { global_props->setPropFloat(id,value); }
string lua_getPropString(string id) { return global_props->getPropString(id); }
void lua_setPropString(string id,string value) { global_props->setPropString(id,value); }

void r2d_bind_lua(lua_State* l) {

	luaapi_bind(l);

	luabind::module(l) [
	    luabind::def("text", &lua_text),
	    luabind::def("getPlayer",&lua_getPlayer),
		luabind::def("getLevel",&lua_getLevel),
		luabind::def("getUserClient",&lua_getUserClient),
		luabind::def("getRenderer",&lua_getUserClient),
		luabind::def("getManager",&lua_getManager),
		luabind::def("getMenu",&lua_getMenu),
		luabind::def("getWidget",&lua_getWidget),
		luabind::def("addWidget",&lua_addWidget),
		luabind::def("getHudNode",&lua_getHudNode),
		luabind::def("openMenu",&lua_openMenu),
		luabind::def("loadLevel",&lua_loadLevel),
		luabind::def("unloadLevel",&lua_unloadLevel),
		LUA_PROP_INTERFACE_DEFS_GLOBAL
	];
}

LuaCarController* load_car_controller(string filename) {
	LuaCarController* c=new LuaCarController();
	c->load(filename);
	r2d_bind_lua(c->getLua());
	return c;
}

char* screenshot_buffer=0;
int screenshot_buffer_size=0;
void saveScreenshot() {
	int buffer_size=_client->config.width*_client->config.height*3;

	if(!screenshot_buffer || screenshot_buffer_size!=buffer_size) {
		delete(screenshot_buffer);
		screenshot_buffer=new char[buffer_size];
		screenshot_buffer_size=buffer_size;
	}
	_client->screenshot(screenshot_buffer);

	ILuint image=ilGenImage();
	ilBindImage(image);
	ilTexImage(_client->config.width,_client->config.height,1,3,
			IL_RGB,IL_UNSIGNED_BYTE,screenshot_buffer);

	char filename[100];
	sprintf(filename,"screenshot/scr_%d%d.png",(int)time(0),rand()%10);
	ilSave(IL_PNG,filename);
	ilDeleteImage(image);
}

void unloadLevel() {
	//cleanup first
	if(level) {
		delete(level);
		level=NULL;
	}
	//controllers
	if(car_controllers) {
		//TODO: cleanup
	}

	player_car=NULL;
}

GameLevel* loadLevel(GameGraphic* graphic,GameResourceManager* manager,string filename) {

	unloadLevel();

	GameLevelDesc* level_desc=manager->getLevel(filename);
	if(!level_desc) return NULL;
	level=new GameLevel(level_desc,manager,graphic,graphic->node_ground);

	for(int i=0;i<level->getScriptSize();i++) {
		r2d_bind_lua(level->getLua(i));
	}

	if(!level) {
		printf("Can't load level %s.\n",filename.c_str());
		return NULL;
	}

	//global level needs to be set at this point
	level->init();

	int num_cars=level->getTrack()->cars.size();

	player_car=num_cars>0 ? level->getTrack()->cars[0] : NULL;

	if(num_cars>1) {
		car_controllers=new LuaCarController*[level->getTrack()->cars.size()-1];
		for(int i=0;i<num_cars-1;i++) {
			car_controllers[i]=load_car_controller("script/ai0.lua");
		}
	}

	{
		DResource* decal_res=manager->getImg("media/light/spot01.png");

		dvect range=level->getTrack()->getSize()/10;
		for(int i=0;i<30000;i++) {

			dvect pos=cpVectToDvect(rand_vect(-range.x,range.x,-range.y,range.y));
			float size_f=rand_float(1,10);
			dvect size=dvect(size_f,size_f);
			float rot=rand_float(0,360);

			/*
			level->getTrack()->addSimpleDecal(decal_res,
					RENDER_FLAG_COLOR,
					pos,size,rot);
			*/
			//level->getTrack()->addLight(decal_res,pos,size*3,rot);
		}
	}

	ignore_frame_delta=true;

	return level;
}

GameMenu* loadMenu(GameResourceManager* manager,DRenderNode* root,string filename) {
	GameMenu* menu=new GameMenu(root,manager,filename);
	r2d_bind_lua(menu->getLua());
	hack_menu=menu;
	menu->init();
	return menu;
}
GameWidget* loadWidget(GameResourceManager* manager,DRenderNode* root,GameWidgetDesc *desc) {
	GameWidget* widget=new GameWidget(root,manager,desc);
	r2d_bind_lua(widget->getLua());
	hack_widget=widget;
	widget->init();
	return widget;
}

LuaGameWidget lua_addWidget(LuaGameMenu menu,string id,dvect pos,dvect size) {

	GameWidgetDesc desc;
	desc.script="menu/"+id+".lua";
	desc.pos=pos;
	desc.size=size;

	GameWidget* w=loadWidget(manager,menu.parent->getNode(),&desc);
	menu.parent->addWidget(w);
	return LuaGameWidget(w);
}


void unloadTrack(DUserClient* client,GameTrack* track) {
	DResource* ground=track->getResource();
	if(ground) {
		client->unloadResource(ground);
		delete(ground);
	}
	delete(track);
}


void carControl(GameCar* car,LuaCarController* controller,long delta) {
	controller->update(car,delta);
}


GameGraph* graph_speed;
GameGraph* graph_input_turn;
GameGraph* graph_input_acc;
GameGraph* graph_steer;
GameGraph* graph_steer_level;

float plot_function(int num_points,float* points,float x) {
	if(x<0) return points[0];
	if(x>=1) return points[num_points-1];
	int index=x*num_points;

	if(index>=num_points-1) return points[num_points-1];

	float p1=(float)index/(float)num_points;
	float p2=(float)(index+1)/(float)num_points;

	x-=p1;
	float rel_x=x/(p2-p1);

	//float rel_x=lerpFloat(((float)index)/((float)num_points),
	//		((float)(index+1)/((float)num_points)),x);
	return lerpFloat(points[index],points[index+1],rel_x);
}

float get_player_steer(float vel) {
	//top speed ~0.09
	//max return 1

	return 1;

	float val;

	return 0.05*4/(vel*15+0.02);
	//val=atan2(0.02,vel);

	float min_output=0.05;
	float top_limit=0.05;
	/*
	//linear fade
	//1 at 0
	//min_output at top_limit
	if(vel>top_limit) val=min_output;
	else val= lerpFloat(1,min_output,vel/top_limit);
	*/

	//plotted
	static float points[]={1,0.75,0.5,0.25,0.15};
	val=plot_function(5,points,vel/top_limit);

//	printf("steer val at %f: %f\n",vel,val);
	return val;
}

void playerControl(DUserClient* client,GameCar* car) {
	float steer=get_player_steer(car->getSpeed());

	float acc=1; //0.0002;

	if(client->isKeyDown(201) || client->isKeyDown('w')) {
		car->accelerate(acc);
	}
	else {
		car->acc_idle();
	}
	if(client->isKeyDown(203) || client->isKeyDown('s')) {
		car->brake(acc);
	}
	bool dsteer=false;
	if(client->isKeyDown(202) || client->isKeyDown('d')) {
		car->turn(-steer);
		dsteer=true;
	}
	if(client->isKeyDown(200) || client->isKeyDown('a')) {
		car->turn(steer);
		dsteer=true;
	}
	if(client->isKeyDown(' ')) {
		car->handbrake();
	}
	if(!dsteer) car->turn(0);
}
void userInput(DUserClient* client,GameGraphic* graphic) {
	if(client->isKeyPressed('t')) {
		saveScreenshot();
		cout<<"Saved screenshot"<<endl;
	}
	if(client->isKeyPressed('1')) {
		client->config.render_mode= (client->config.render_mode==RENDER_MODE_SHADED ?
				RENDER_MODE_SIMPLE : RENDER_MODE_SHADED);
	}
	if(client->isKeyPressed('2')) {
		client->config.shadows_on=!client->config.shadows_on;
	}
	if(client->isKeyPressed('3')) {
		graphic->node_ground->visible=!graphic->node_ground->visible;
	}
	if(client->isKeyPressed('4')) client->config.aa_enabled=!client->config.aa_enabled;
	if(client->isKeyPressed('5')) client->config.flat_shadows=!client->config.flat_shadows;

	if(client->isKeyPressed('c')) {
		if(level) {
			cam_car++;
			if(cam_car>=level->getTrack()->cars.size()) cam_car=0;
		}
	}
}

//gets GameMenu from cache or loads from disk
GameMenu* getMenu(GameResourceManager* manager,DRenderNode* root,string filename) {
	for(int i=0;i<menu_cache.size();i++) {
		if(menu_cache[i]->script_file==filename) {
			return menu_cache[i];
		}
	}
	GameMenu* m=loadMenu(manager,root,filename);
	if(m) {
		menu_cache.push_back(m);
	}
	return m;
}
//loads GameMenu and places it on top of menu stack
bool openMenu(GameResourceManager* manager,DRenderNode* root,string filename) {
	GameMenu* m=getMenu(manager,root,filename);
	if(!m) {
		printf("Failed to open menu %s\n",filename.c_str());
		return false;
	}
	m->is_closed=false;
	m->show(true);

	//hide current menu
	if(menu_stack.size()>0) {
		GameMenu* m=menu_stack.back();
		m->show(false);
	}

	menu_stack.push_back(m);

	printf("menu %s opened, stack size %d\n",filename.c_str(),menu_stack.size());

	for(int i=0;i<menu_stack.size();i++)
		printf(" %s\n",menu_stack[i]->script_file.c_str());

	return true;
}

vector<DRenderNode*>* client_get_nodes(DBoundingBox* bb) {
	if(!level) return NULL;

	level->getTrack()->iterateTreeNodes(bb);
	return &level->getTrack()->iterated_nodes;
}

int main(int argc,char** argv) {

	srand(time(0));

	luabind::set_pcall_callback(r2d_lua_pcall);

	R2DInit();

	printf("Chipmunk version %s\n",cpVersionString);

	//TODO remove
//	XmlParser::parseCampaign("config/campaign.xml");

	DUserClientConfig client_config=XmlParser::parseConfig("config/settings.xml");
	client_config.title="Burning Skids";
	_client=new DUserClientFreeGlut(client_config);
	DUserClient* client=_client;
	manager=new GameResourceManager(client);
	graphic=new GameGraphic(manager,client->getRootNode());

	global_props=new PropInterface();

	openMenu(manager,graphic->node_menu,"menu/main.lua");

	level=NULL;
	if(argc>1) {
		string level_filename=(string)"media/"+argv[1]+"/level.xml";
		level=loadLevel(graphic,manager,level_filename);
		openMenu(manager,graphic->node_menu,"menu/tweaker.lua");
	}
	else {
		//level=loadLevel(graphic,manager,"media/hello_level/level.xml");
	}
	if(level) {
		//menu_stack.back()->show(false);
	}

	DTimeLimiter *limiter=new DTimeLimiter(1000/6000);

	limiter->firstMark();

	int steps=1;

	long agregator=0;	//nanoseconds
	int tick_step=16;	//fixed timestep
	long elapsed_time=0;	//milisecnds
	long elapsed_time_ns=0;	//nanoseconds
	long last_fps_timestamp=0;
	int last_fps_frames=0;

	//DRenderNode* test_node=client->getRootNode();
	DRenderNode* test_node=graphic->node_ground;
	//test_node->post_draw=r2d_post_draw;
	test_node->post_draw_data=graphic;

	graphic->node_hud->force_render=true;
	graphic->node_hud->post_draw=r2d_hud_draw;
	graphic->node_hud->post_draw_data=graphic;
	graphic->node_menu->post_draw=r2d_menu_draw;
	//graphic->node_menu->post_draw_data=menu;

	luatools_init();

	graph_speed=new GameGraph(500,10);
	graph_input_acc=new GameGraph(500,10);
	graph_input_turn=new GameGraph(500,10);
	graph_steer=new GameGraph(50,1);
	graph_steer_level=new GameGraph(500,10);

	for(int i=0;i<50;i++) {
		graph_steer->put( get_player_steer(0.09 * ((float)i)/50) );
	}

	while(true) {

		/*
		if(player_car) {
			if(player_car->isDone()) {
				unloadLevel();
				openMenu(manager,graphic->node_menu,"menu/finish.lua");
				//loadLevel(graphic,manager,"media/hello_level/level.xml");
			}
		}
		*/

		if(ignore_frame_delta) {
			ignore_frame_delta=false;
		}
		else {
			agregator+=limiter->getDelta();
		}

		long delta_ns=limiter->getDelta();
		long delta_ms=delta_ns/1000;

		//slow down sim if computer too slow
		if(agregator>200000) agregator=200000;

		while(agregator>=tick_step*1000) {
			if(level) {
				for(int i=0;i<level->getTrack()->cars.size();i++) {
					if(level->getTrack()->cars[i]==player_car)
						playerControl(client,player_car);
					else if(i<level->getTrack()->cars.size())
						carControl(level->getTrack()->cars[i],car_controllers[i-1],tick_step);
				}

				level->getTrack()->update_position=level->getTrack()->cars[cam_car]->getPos();

				level->update(tick_step);
				//updateCars(cars,tick_step);
			}
			agregator-=tick_step*1000;
		}

		userInput(client,graphic);

		elapsed_time_ns+=delta_ns;
		elapsed_time=elapsed_time_ns/1000;

		if(level) {
			cam_pos=level->getTrack()->cars[cam_car]->getPos()+
					level->getTrack()->cars[cam_car]->getSpeedVect()*125*2.8;

			GameCar* car=level->getTrack()->cars[cam_car];
			R2DEngine* engine=car->physic_object->get_engine();

			graph_speed->put(car->getSpeed());
			graph_input_acc->put(car->last_acc);
			graph_input_turn->put(car->last_turn);
			graph_steer_level->put(get_player_steer(car->getSpeed()));

			/*
			printf("Car: locked %d mass %f\t Engine: gear %d, power %f, ratio %f, gas %f, output power %f\n",
					car->controls_locked,
					cpBodyGetMass(car->physic_object->get_body()),
					engine->get_gear(),
					engine->get_power(),
					engine->get_ratio(),
					engine->get_gas(),
					engine->get_output_power());
			*/

		}

		/*
		for(int i=0;i<speed_graph->getWidth();i++) {
			printf("%f ",speed_graph->getData(i));
		}
		printf("\n");
		*/

		if(menu_stack.size()>0) {
			GameMenu* m=menu_stack.back();

			if(m->isVisible()) {
				//TODO: fix this
				m->update(10,client->isMousePressed(0),client->isMouseDown(0),
						client->getMousePos()*
							dvect(
								300.0f/(float)client->config.width,
								200.0f/(float)client->config.height
								));
				//if(m->is_closed) {
				if(menu_stack.back()->is_closed) {
					m->show(false);

					/*
					for(int i=0;i<menu_stack.size();i++) {
						if(menu_stack[i]==m) {
							menu_stack.erase(menu_stack.begin()+i);
							break;
						}
					}
					*/
					menu_stack.pop_back();

					if(menu_stack.size()>0) {
						menu_stack.back()->show(true);
					}
					printf("Menu %s closed, stack size %d\n",
							m->script_file.c_str(),menu_stack.size());

					for(int i=0;i<menu_stack.size();i++)
						printf(" %s\n",menu_stack[i]->script_file.c_str());

				}
			}
		}

		client->camera(cam_pos,0);

		client->update(delta_ms,client_get_nodes);

		/* Update audio listener (aka. player) position */
		//manager->getAudio()->setListenerPosition(cam_pos);
		if(level && level->getTrack())
			manager->getSoundManager()->setListenerPosition(level->getTrack()->cars[cam_car]->getPos());

		if(delta_ms>0) {
			current_fps=1000.0/(float)delta_ms;
			current_frametime=delta_ms;
		}

		/*
		printf("%d frames, elapsed %d, timestamp %d, dif %d\n",
				last_fps_frames,
				elapsed_time,
				last_fps_timestamp,
				elapsed_time-last_fps_timestamp);
		*/

		printf("frame delta %ld\n",delta_ms);

		last_fps_frames++;
		if(elapsed_time-last_fps_timestamp>=1000) {
			int time_delta=elapsed_time-last_fps_timestamp;
			float mspf=((float)time_delta)/((float)last_fps_frames);
			printf("%d frames in %d ms (%f mspf)\n",
					last_fps_frames,time_delta,mspf);
			last_fps_timestamp=elapsed_time;
			last_fps_frames=0;
		}

		limiter->mark();
		limiter->wait();
	}

	return 0;
}

void lua_text(string text,float size) {
	_client->text(graphic->font_arial,text,size);
}
LuaGameCar lua_getPlayer() {
	return LuaGameCar(player_car);
}
LuaGameLevel lua_getLevel() {
	return LuaGameLevel(level);
}
LuaGameMenu lua_openMenu(string filename) {
	if(openMenu(manager,graphic->node_menu,"menu/"+filename+".lua")) {
		return LuaGameMenu(hack_menu);
	}
	return LuaGameMenu(0);
}
bool lua_loadLevel(string level) {
	return (loadLevel(graphic,manager,"media/"+level+"/level.xml")!=NULL);
}
void lua_unloadLevel() {
	unloadLevel();
}

//terrain test text
void r2d_post_draw(DRenderNode* node,void* client,void* data) {
	DUserClient *c=(DUserClient*)client;
	GameGraphic *g=(GameGraphic*)data;

	c->saveMatrix();
	c->text(g->font_arial,"hello",4);
	c->transform(dvect(0,-1),dvect(1,1),-15);

	char b[100];
	sprintf(b,"FPS:%03d",current_fps);
	c->text(g->font_arial,b,2);
	c->restoreMatrix();

	if(player_car) {
		dvect t=player_car->getPos();

		GameRoadGeometryMetapoint meta=level->getTrack()->getRoad(0)->geometry.getClosest(t);
		dvect p=meta.center;
		dvect p2=level->getTrack()->getRoad(0)->geometry.getMeta(meta.local_pos+2).center;

		c->saveMatrix();
		c->transform(p,dvect(1,1),0);
		c->text(g->font_arial,"x <-here",2);
		c->restoreMatrix();

		c->saveMatrix();
		c->transform(p2,dvect(1,1),0);
		c->text(g->font_arial,"x <-and here",1);
		c->restoreMatrix();
	}

	//printf("%s\n",b);
}

void renderGraph(DUserClient* c,GameGraph* graph,dvect pos,dvect size,float scale_y,bool center) {
	float scale_x=graph->getWidth();

//	c->color(0,0,0,1);
//	c->quad(pos,size,true);
	c->color(1,0,0,1);

	pos.y+=size.y;
	if(center) pos.y-=size.y/2;

	dvect last=dvect(0,-graph->getData(0)/scale_y) *size;
	c->moveTo(pos+last);
	for(int i=1;i<graph->getWidth();i++) {
		dvect npos=dvect(((float)i)/scale_x,-graph->getData(i)/scale_y)*size;
		c->lineTo(pos+npos);
	}
}


void r2d_hud_draw(DRenderNode* node,void* client,void* data) {
	DUserClient *c=(DUserClient*)client;
	GameGraphic *g=(GameGraphic*)data;

	if(!level) return;

	char b[100];

	c->saveMatrix();

	sprintf(b,"FPS:%03d",current_fps);
	c->transform(dvect(2,2),dvect(1,1),0);
	c->text(g->font_arial,b,10);

	sprintf(b,"%03dms",current_frametime);
	c->transform(dvect(0,10),dvect(1,1),0);
	c->text(g->font_arial,b,10);

	if(level->getTrack()->cars.size()==0 || !player_car) return;

	GameCar* car=level->getTrack()->cars[cam_car];
	sprintf(b,"Speed:%03d",(int)(car->getSpeed()*1000*3.6));
	c->transform(dvect(0,10),dvect(1,1),0);
	c->text(g->font_arial,b,10);

	c->restoreMatrix();

	dvect graph_s=dvect(100,50);
	//renderGraph(c,graph_steer,dvect(0,50),graph_s,1,false);
	float vel=player_car->getSpeed()/0.09 *100;
//	printf("speed %f val %f\n",player_car->getSpeed(),vel);
	//c->moveTo(dvect(vel,50));
	//c->lineTo(dvect(vel,50+graph_s.y));

	//renderGraph(c,graph_steer_level,dvect(0,100),graph_s,1,false);

	renderGraph(c,graph_speed,dvect(0,50),graph_s,0.1,false);
	//renderGraph(c,graph_input_acc,dvect(0,50),graph_s,5,true);
	//renderGraph(c,graph_input_turn,dvect(0,100),graph_s,5,true);

	c->color(1,0,0,1);

	float* s_data=car->getSensorData();
	if(s_data) {
		float spread=car->getSensorSpread();
		int pix=car->getSensorPixels();
		for(int i=0;i<pix;i++) {

			c->saveMatrix();
			c->transform(dvect(100,10),dvect(1,1),car->getSensorAngle(i));
			c->moveTo(dvect(0,0));
			c->lineTo(dvect(0,s_data[i])*100);
			c->restoreMatrix();
		}
	}

	c->saveMatrix();
	c->transform(dvect(-1,-1),dvect(1,1),
			cpBodyGetAngle(car->physic_object->get_engine()->engine)*57.325);
	float s=0.5;
	c->color(1,0,0,1);
	c->moveTo(dvect(-s,0));
	c->lineTo(dvect(s,0));
	c->moveTo(dvect(0,-s));
	c->lineTo(dvect(0,s));
	c->restoreMatrix();

	c->saveMatrix();
	c->transform(dvect(-1.5,-1),dvect(1,1),
			cpBodyGetAngle(player_car->physic_object->get_engine()->beam)*57.325);
	s=0.25;
	c->color(1,0,0,1);
	c->moveTo(dvect(-s,0));
	c->lineTo(dvect(s,0));
	c->moveTo(dvect(0,-s));
	c->lineTo(dvect(0,s));
	c->restoreMatrix();

	level->renderGui();
}

void r2d_menu_draw(DRenderNode* node,void* client,void* data) {
	if(menu_stack.size()==0) return;
	if(menu_stack.back()->isVisible())
		menu_stack.back()->render();
}
