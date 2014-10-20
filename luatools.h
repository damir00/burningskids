#ifndef _LUATOOLS_H_
#define _LUATOOLS_H_

#include <string>

extern "C" {
	#include <lualib.h>
}
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

void luatools_init();
bool luatools_load_script(lua_State* L,std::string filename,bool execute=false);

lua_State* luatools_open();
void luatools_close(lua_State* l);
luabind::object luatools_get_func(lua_State* l,std::string func_name);

#endif

