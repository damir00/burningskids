
#include "luacarcontroller.h"

LuaCarController::LuaCarController() {

}
LuaCarController::~LuaCarController() {
	lua_close(lua);
}
void LuaCarController::update(GameCar* car,long delta) {
	if(func_update)
		luabind::call_function<void>(func_update,LuaGameCar(car),delta);
}

void LuaCarController::load(string filename) {

	lua=luatools_open();

/*
	//register classes
	luabind::module(lua) [
		luabind::class_<GameCar>("GameCar")
		.def("accelerate",&GameCar::accelerate)
		.def("turn",&GameCar::turn)
	];
*/

	luatools_load_script(lua,filename,true);

	func_update=luatools_get_func(lua,"update");
	if(!func_update) {
		cout<<"Can't load car controller "<<filename<<endl;
	}

	cout<<"controller inited\n";
}
lua_State* LuaCarController::getLua() {
	return lua;
}


