#ifndef _LUACARCONTROLLER_H_
#define _LUACARCONTROLLER_H_

#include "gamecar.h"
#include "gametrack.h"
#include "luatools.h"
#include "luaapi.h"

class LuaCarController {
	lua_State* lua;
	luabind::object func_update;

public:
	LuaCarController();
	~LuaCarController();
	void update(GameCar* car,long delta);
	void load(string filename);
	lua_State* getLua();
};

#endif

