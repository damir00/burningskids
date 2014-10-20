#include "luatools.h"

#include "gametrack.h"
#include "gamecar.h"
#include "gameutils.h"
#include "gamegraphic.h"

#include <iostream>
#include <stdio.h>

using namespace std;

void test(const char* message) {
	cout<<message<<endl;
}

bool luatools_load_script(lua_State* L,string filename,bool execute)  {
	printf("Loading script %s\n",filename.c_str());
	int s = luaL_loadfile(L, filename.c_str());
	if(s==0 && execute) {
		s=lua_pcall(L, 0, LUA_MULTRET, 0);
		if(s!=0) {
			cout<<"script "<<filename<<" failed to run:\n"<<lua_tostring(L, -1)<<endl;
			return false;
		}
	}
	else {
		cout<<"script "<<filename<<" failed to compile:\n"<<lua_tostring(L, -1)<<endl;
		return false;
	}
	return true;
}

class luatest {
public:
	char* id;

	char* get_id() { return id; }
	void set_id(const char* _id) { id=(char*)_id; }
};


void luaapi_test() {
	lua_State* L;
	L=lua_open();
	luabind::open(L);

	//test
	luabind::module(L) [
		luabind::def("test", &test)
	];
	luabind::module(L) [
		luabind::class_<luatest>("luatest")
		.property("id",&luatest::get_id,&luatest::set_id)
	];
	luatools_load_script(L,"script/test.lua");
	luabind::call_function<void>(L,"lua_f");

	luatest lt;
	lt.id="Lua Test Object";

	luabind::call_function<void>(L,"lua_f2",&lt);
/*
	luabind::object obj(luabind::from_stack(L, -1));
	luabind::call_function<void>(obj);
*/
}

void luatools_init() {

	//luaapi_test();


	/*
	//GameCar
	luabind::module(L) [ 
		luabind::class_<GameCar>("GameCar")
			.def(luabind::constructor<>())
	];
	*/

}



static void _luaOpenLib(lua_State* L, const char* name, lua_CFunction fn) {
    lua_pushcfunction(L, fn) ;
    lua_pushstring(L, name) ;
    lua_call(L, 1, 0) ;
}
static void _luaOpenLibs(lua_State* L) {
    _luaOpenLib(L, "", luaopen_base);
    _luaOpenLib(L, LUA_LOADLIBNAME, luaopen_package);
    _luaOpenLib(L, LUA_TABLIBNAME, luaopen_table);
    _luaOpenLib(L, LUA_STRLIBNAME, luaopen_string);
    _luaOpenLib(L, LUA_MATHLIBNAME, luaopen_math);
}

lua_State* luatools_open() {
	lua_State* lua=lua_open();
	luabind::open(lua);
	_luaOpenLibs(lua);

	return lua;
}
//cleanup
void luatools_close(lua_State* l) {

}

luabind::object luatools_get_func(lua_State* l,std::string func_name) {
	return luabind::globals(l)[func_name];
}



