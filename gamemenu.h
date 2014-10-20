
#ifndef _GAMEMENU_H_
#define _GAMEMENU_H_

#include "duserclient.h"
#include "gamewidget.h"
#include "luatools.h"
#include "propinterface.h"
#include "gamescript.h"

#include <vector>
#include <string>
using namespace std;

class GameMenu : public PropInterface {
	vector<GameWidget*> widgets;

	DUserClient* client;
	GameResourceManager* manager;
	DRenderNode* node;

	GameScript script;

public:
	string script_file;
	bool is_closed;
	GameMenu(DRenderNode* root,GameResourceManager* manager,string script_filename);
	~GameMenu();

	DRenderNode* getNode();
	void addWidget(GameWidget* widget);

	void init();
	void update(long time,bool mouse_clicked,bool mouse_down,dvect mouse_pos);
	void render();
	lua_State* getLua();
	void close();

	void show(bool on);
	bool isVisible();
};

#endif

