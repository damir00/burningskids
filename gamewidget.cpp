#include "gamewidget.h"

using namespace std;

GameWidget::GameWidget(DRenderNode* parent,GameResourceManager* _manager,GameWidgetDesc* desc) {
	manager=_manager;
	node=parent->createNode();
	pos=desc->pos;
	size=desc->size;
	//node->object=manager->client->createObject(manager->getImg(desc->image),size);
	node->pos=dvect3(pos,0);
	script.load(desc->script);

	is_mouse_down=false;
	is_clicked=false;
}
void GameWidget::update(long time,bool mouse_clicked,bool mouse_down,dvect mouse_pos) {
	rel_mouse_pos=mouse_pos-pos;

	if(node->visible) {
		if(mouse_pos.x>=pos.x && mouse_pos.x<=pos.x+size.x &&
			mouse_pos.y>=pos.y && mouse_pos.y<=pos.y+size.y) {
			mouse_inside=true;
			is_clicked=mouse_clicked;
			is_mouse_down=mouse_down;
		}
		else {
			mouse_inside=false;
			is_clicked=false;
			is_mouse_down=false;
		}
	}
	else {
		mouse_inside=false;
		is_clicked=false;
	}

	script.update(time);
}
void GameWidget::render() {
	if(node->visible) {
		script.render();
	}
}
bool GameWidget::mouseClicked() {
	return is_clicked;
}
bool GameWidget::mouseDown() {
	return is_mouse_down;
}
bool GameWidget::mouseInside() {
	return mouse_inside;
}
dvect GameWidget::mousePos() {
	return rel_mouse_pos;
}
void GameWidget::initLua(string filename) {
	script.load(filename);
}
void GameWidget::init() {
	script.init();
}
lua_State* GameWidget::getLua() {
	return script.lua;
}
dvect GameWidget::getPos() {
	return pos;
}
dvect GameWidget::getSize() {
	return size;
}
void GameWidget::show(bool on) {
	node->visible=on;
}

int GameWidget::getPropInt(string prop) {
	if(script.lua_get_prop_int) {
		return luabind::call_function<int>(script.lua_get_prop_int,prop);
	}
	return 0;
}
void GameWidget::setPropInt(string prop,int v) {
	if(script.lua_set_prop_int) {
		luabind::call_function<void>(script.lua_set_prop_int,prop,v);
	}
}
float GameWidget::getPropFloat(string prop) {
	if(script.lua_get_prop_float) {
		return luabind::call_function<float>(script.lua_get_prop_float,prop);
	}
	return 0;
}
void GameWidget::setPropFloat(string prop,float v) {
	if(script.lua_set_prop_int) {
		luabind::call_function<void>(script.lua_set_prop_float,prop,v);
	}
}
string GameWidget::getPropString(string prop) {
	if(script.lua_get_prop_string) {
		return luabind::call_function<string>(script.lua_get_prop_string,prop);
	}
	return 0;

}
void GameWidget::setPropString(string prop,string v) {
	if(script.lua_set_prop_string) {
		luabind::call_function<void>(script.lua_set_prop_string,prop,v);
	}
}



