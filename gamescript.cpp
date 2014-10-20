#include "gamescript.h"

void GameScript::load(std::string filename) {
	lua=luatools_open();
	luatools_load_script(lua,filename,true);
	lua_init=luatools_get_func(lua,"init");
	lua_update=luatools_get_func(lua,"update");
	lua_clicked=luatools_get_func(lua,"clicked");
	lua_render=luatools_get_func(lua,"render");

	lua_get_prop_int=luatools_get_func(lua,"getPropInt");
	lua_set_prop_int=luatools_get_func(lua,"setPropInt");
	lua_get_prop_float=luatools_get_func(lua,"getPropFloat");
	lua_set_prop_float=luatools_get_func(lua,"setPropFloat");
	lua_get_prop_string=luatools_get_func(lua,"getPropString");
	lua_set_prop_string=luatools_get_func(lua,"setPropString");


	//luaapi_bind(lua);
}
void GameScript::update(long ts) { if(lua_update) luabind::call_function<void>(lua_update,ts); }
void GameScript::render() { if(lua_render) luabind::call_function<void>(lua_render); }
void GameScript::init() { if(lua_init) luabind::call_function<void>(lua_init); }
