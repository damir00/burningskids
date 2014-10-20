
#ifndef _GAMELEVEL_H_
#define _GAMELEVEL_H_

#include "gametrack.h"
#include "gameaudio.h"
#include "gamestructs.h"
#include "gameresourcemanager.h"
#include "duserclient.h"
#include "luatools.h"
#include "propinterface.h"

struct GameLevelScript {
	lua_State* lua;
	luabind::object lua_init;
	luabind::object lua_update;
	luabind::object lua_render;
	luabind::object lua_rendergui;
};
class GameLevel : public PropInterface {
	GameTrack* track;
	GameResourceManager* manager;
	GameGraphic* graphic;
	DRenderNode* root_node;
	DRenderNode* child_node;

	vector<GameLevelScript> scripts;

	bool initLua(string filename);

public:
	GameLevel(GameLevelDesc* desc,GameResourceManager* manager,
			GameGraphic* graphic,DRenderNode* node);
	~GameLevel();
	void init();
	GameTrack* getTrack();
	void update(long ts);
	void renderGui();

	void luaAddObject(string object_id,float x,float y,float angle,bool is_static=false,float scale=1);
	GameCar* luaAddCar(string car_id,float x,float y,float angle);

	lua_State* getLua(int index);
	int getScriptSize();

	void lockCars();
	void releaseCars();	//start race

	void _rootPostDraw();
};

#endif
