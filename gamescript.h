#ifndef _GAMESCRIPT_H_
#define _GAMESCRIPT_H_

#include "luatools.h"

class GameScript {
public:
	lua_State* lua;
	luabind::object lua_init;
	luabind::object lua_update;
	luabind::object lua_clicked;
	luabind::object lua_render;
	//luabind::object lua_activate;

	luabind::object lua_get_prop_int;
	luabind::object lua_set_prop_int;
	luabind::object lua_get_prop_float;
	luabind::object lua_set_prop_float;
	luabind::object lua_get_prop_string;
	luabind::object lua_set_prop_string;

	void load(std::string filename);
	void update(long ts);
	void render();
	void init();
};

#endif

