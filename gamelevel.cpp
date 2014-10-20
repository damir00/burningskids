#include "gamelevel.h"

void root_post_draw(DRenderNode* node,void* pclient,void *plevel) {
	GameLevel* l=(GameLevel*)plevel;
	l->_rootPostDraw();
}

GameLevel::GameLevel(GameLevelDesc* desc,GameResourceManager* _manager,
		GameGraphic* _graphic,DRenderNode* node) {
	graphic=_graphic;
	manager=_manager;
	root_node=node;

	track=new GameTrack(manager->getTrack(desc->track),graphic,manager,node);

	child_node=root_node->createNode();
	child_node->setPostDraw(root_post_draw,this);

	manager->client->setCloudsResource(manager->getImg(desc->clouds));
	manager->client->setCloudsScale(desc->clouds_scale);
	manager->client->setCloudsVel(desc->clouds_vel);

	manager->client->setAmbient(desc->ambient_color);
	manager->client->setSunColor(desc->sun_color);
	manager->client->setSunDir(desc->sun_dir);

	if(!initLua(desc->script)) {
		printf("Failed to load level script.\n");
	}
	if(!initLua("script/"+desc->game_type+".lua")) {
		printf("ERROR: Failed to load game type script!\n");
	}

	manager->getSoundManager()->soundtrackInit(desc->soundtrack);
}
GameLevel::~GameLevel() {
	delete(track);
	track=NULL;
	root_node->removeNode(child_node);
	delete(child_node);
	child_node=NULL;

	for(int i=0;i<scripts.size();i++) {
		luatools_close(scripts[i].lua);
	}
	manager->getSoundManager()->soundtrackStop();
}

void GameLevel::init() {
	for(vector<GameLevelScript>::iterator i=scripts.begin();i!=scripts.end();i++) {
		if(i->lua_init) luabind::call_function<void>(i->lua_init);
		//if(lua_init) luabind::call_function<void>(lua_init);
	}
	manager->getSoundManager()->soundtrackPlay();
}

GameTrack* GameLevel::getTrack() {
	return track;
}

int GameLevel::getScriptSize() {
	return scripts.size();
}
lua_State* GameLevel::getLua(int index) {
	return scripts[index].lua;
}

void GameLevel::update(long ts) {

	for(vector<GameLevelScript>::iterator i=scripts.begin();i!=scripts.end();i++) {
		if(i->lua_update) luabind::call_function<void>(i->lua_update,ts);
		//if(lua_update) luabind::call_function<void>(lua_update,ts);
	}

	track->update(ts);
}

void GameLevel::_rootPostDraw() {
	for(vector<GameLevelScript>::iterator i=scripts.begin();i!=scripts.end();i++) {
		if(i->lua_render) luabind::call_function<void>(i->lua_render);
	}
}

bool GameLevel::initLua(string filename) {
	GameLevelScript s;
	s.lua=luatools_open();
	if(!luatools_load_script(s.lua,filename,true)) {
		luatools_close(s.lua);
		return false;
	}
	s.lua_init=luatools_get_func(s.lua,"init");
	s.lua_update=luatools_get_func(s.lua,"update");
	s.lua_render=luatools_get_func(s.lua,"render");
	s.lua_rendergui=luatools_get_func(s.lua,"gui");
	scripts.push_back(s);
	return true;
}

void GameLevel::renderGui() {
	for(vector<GameLevelScript>::iterator i=scripts.begin();i!=scripts.end();i++) {
		if(i->lua_rendergui) luabind::call_function<void>(i->lua_rendergui);
		//if(lua_rendergui) luabind::call_function<void>(lua_rendergui);
	}
}

void GameLevel::luaAddObject(string object_id,float x,float y,float angle,bool is_static,
		float scale) {
	GameCarBody* body_desc=manager->getBody(object_id);
	if(!body_desc) {
		return;
	}
	GameCar* obj;
	if(is_static) {
		obj=new GameCar(body_desc,graphic,manager,dvect3(x,y,0),angle,scale);
	}
	else {
		obj=new GameCar(body_desc,graphic,manager,scale);
		obj->set(cpv(x,y),angle);
	}

	track->addObject(obj,is_static);
}
GameCar* GameLevel::luaAddCar(string car_id,float x,float y,float angle) {
	GameCarBody* body=manager->getBody(car_id);
	if(!body) {
		printf("Can't load car %s!\n",car_id.c_str());
		return NULL;
	}
	GameCar* car=new GameCar(body,graphic,manager);
	car->set(cpv(x,y),angle);
	track->addCar(car);
	return car;
}

void GameLevel::releaseCars() {
	for(int i=0;i<track->cars.size();i++) {
		track->cars[i]->setControlsLocked(false);
	}
}
void GameLevel::lockCars() {
	for(int i=0;i<track->cars.size();i++) {
		track->cars[i]->setControlsLocked(true);
	}
}
