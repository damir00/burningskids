#include "gamemenu.h"

GameMenu::GameMenu(DRenderNode* root,GameResourceManager* _manager,string _script_file) {
	manager=_manager;
	client=manager->client;
	node=root->createNode();

	script_file=_script_file;
	script.load(script_file);
	is_closed=false;
}
GameMenu::~GameMenu() {

}

void GameMenu::close() {
	is_closed=true;
}
void GameMenu::show(bool on) {
	node->visible=on;
}
bool GameMenu::isVisible() {
	return node->visible;
}

DRenderNode* GameMenu::getNode() {
	return node;
}

void GameMenu::init() {
	script.init();
}
void GameMenu::update(long time,bool mouse_clicked,bool mouse_down,dvect mouse_pos) {
	for(vector<GameWidget*>::iterator it=widgets.begin();it<widgets.end();it++) {
		(*it)->update(time,mouse_clicked,mouse_down,mouse_pos);
	}
	script.update(time);
}
void GameMenu::render() {
	script.render();
	for(vector<GameWidget*>::iterator it=widgets.begin();it<widgets.end();it++) {
		(*it)->render();
	}
}
void GameMenu::addWidget(GameWidget* widget) {
	widgets.push_back(widget);
}

lua_State* GameMenu::getLua() {
	return script.lua;
}
