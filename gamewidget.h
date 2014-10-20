#ifndef _GAMEWIDGET_H_
#define _GAMEWIDGET_H_

#include "duserclient.h"
#include "gameresourcemanager.h"
#include "luatools.h"
#include "propinterface.h"
#include "gamescript.h"

#include <string>

struct GameWidgetDesc {
	dvect pos,size;
	string script;
	string image;
};

class GameWidget : public PropInterface {
	dvect pos;
	dvect size;
	bool is_clicked;
	bool is_mouse_down;
	bool mouse_inside;
	dvect rel_mouse_pos;

	GameScript script;

	DRenderNode* node;
	GameResourceManager* manager;
public:
	GameWidget(DRenderNode* parent,GameResourceManager* _manager,GameWidgetDesc* desc);
	void update(long time,bool mouse_clicked,bool mouse_down,dvect mouse_pos);
	void render();
	bool mouseClicked();
	bool mouseDown();
	bool mouseInside();
	dvect mousePos();
	void initLua(std::string filename);
	void init();
	lua_State* getLua();
	dvect getPos();
	dvect getSize();
	void show(bool on);

	int getPropInt(std::string prop);
	void setPropInt(std::string prop,int v);
	float getPropFloat(std::string prop);
	void setPropFloat(std::string prop,float v);
	std::string getPropString(std::string prop);
	void setPropString(std::string prop,std::string v);
};


/*
class GameWidgetButton : public GameWidget {
public:
	GameWidgetButton(GameResourceManager* manager,DRenderNode* parent,
			GameWidgetDesc* desc) : GameWidget(parent,manager,desc) {

	}
};
*/



#endif


