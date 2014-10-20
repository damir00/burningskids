#ifndef _LUAAPI_H_
#define _LUAAPI_H_

#include "luatools.h"

#include "gamecar.h"
//#include "gameobject.h"
#include "gameroad.h"
#include "gametrack.h"
#include "gamelevel.h"
#include "gameresourcemanager.h"
#include "gamewidget.h"
#include "gamemenu.h"
#include "duserclient.h"
#include "duserclientfreeglut.h"
#include "gamecampaign.h"
#include "gamecampaigndefinition.h"

/*
class LuaDVect {
	dvect* parent;
};
*/

template <class objType>
class LuaPropInterface {
public:
	objType parent;
	LuaPropInterface(objType _parent) { parent=_parent; }

	int getPropInt(string prop) { return parent->getPropInt(prop); }
	void setPropInt(string prop,int v) { return parent->setPropInt(prop,v); }

	float getPropFloat(string prop) { return parent->getPropFloat(prop); }
	void setPropFloat(string prop,float v) { return parent->setPropFloat(prop,v); }

	string getPropString(string prop) { return parent->getPropString(prop); }
	void setPropString(string prop,string v) { return parent->setPropString(prop,v); }
};

class LuaGameCar {
	GameCar* parent;
public:
	LuaGameCar(GameCar* _parent) { parent=_parent; }
	void turn(float x) { parent->turn(x); }
	void accelerate(float x) { parent->accelerate(x); }
	void brake(float x) { parent->brake(x); }
	dvect getPos() { return parent->getPos(); }
	dvect getHeadingVect() { return parent->getHeadingVect(); }
	float getSpeed() { return parent->getSpeed(); }
	dvect getSpeedVect() { return parent->getSpeedVect(); }
	void sensorsInit(int num_pixels,float spread_angle,float length) {
		parent->sensorsInit(num_pixels,spread_angle,length);
	}
	float getSensorPixels() { return parent->getSensorPixels(); }
	float getSensorData(int i) { return parent->getSensorData()[i]; }
	float getSensorAngle(int i) { return parent->getSensorAngle(i); }
	bool getSensorEnabled() { return parent->getSensorEnabled(); }
	//scoring
	int getLap() { return parent->getLap(); }
	float getScore() { return parent->getScore(); }
	void setScore(float s) { parent->setScore(s); }
	int getRank() { return parent->getRank(); }

	float getFinalScore() { return parent->getFinalScore(); }
	void setFinalScore(float s) { parent->setFinalScore(s); }
	bool isDone() { return parent->isDone(); }

	GameRoadGeometryMetapoint getRoadClosest() { return parent->road_closest; }
};

class LuaGameRoad {
	GameRoad* parent;
public:
	LuaGameRoad(GameRoad* _parent) { parent=_parent; }
	GameRoadGeometryMetapoint getMeta(float pos) { return parent->geometry.getMeta(pos); }
	GameRoadGeometryMetapoint getClosest(dvect pos) { return parent->geometry.getClosest(pos); }
	float getLength() { return parent->geometry.approxLength(); }
	bool isCyclic() { return parent->cyclic; }
};

class LuaGameTrack {
	GameTrack* parent;
public:
	LuaGameTrack(GameTrack* _parent) { parent=_parent; }
	int getRoadSize() { return parent->getRoadSize(); }
	LuaGameRoad getRoad(int id) { return LuaGameRoad(parent->getRoad(id)); }
	dvect surfaceQuery(dvect pos) { return parent->surfaceQuery2(pos); }
};

class LuaGameLevel : public LuaPropInterface<GameLevel*> {
public:
	LuaGameLevel(GameLevel* _parent) : LuaPropInterface<GameLevel*>(_parent) {}
	LuaGameCar addCar(string id,float x,float y,float angle) { return LuaGameCar(parent->luaAddCar(id,x,y,angle)); }
	void addObject(string id,float x,float y,float angle) { parent->luaAddObject(id,x,y,angle); }
	void addObject(string id,float x,float y,float angle,bool is_static) { parent->luaAddObject(id,x,y,angle,is_static); }
	void addObject(string id,float x,float y,float angle,bool is_static,float scale) {
		parent->luaAddObject(id,x,y,angle,is_static,scale); }
	LuaGameTrack getTrack() { return LuaGameTrack(parent->getTrack()); }

	int getCarSize() { return parent->getTrack()->cars.size(); }
	LuaGameCar getCar(int i) { return LuaGameCar(parent->getTrack()->cars[i]); }

	//game mode api
	void lockCars() { parent->lockCars(); }
	void releaseCars() { parent->releaseCars(); }
};

class LuaUserClientShader {
public:
	DGLShader* parent;

	LuaUserClientShader() { parent=new DGLShader(); }
	LuaUserClientShader(DGLShader* _parent) { parent=_parent; }
	bool compile(string source) { return parent->compile(source); }
	bool compileFromFile(string file) {
		return parent->compileFullFromFile("shader/base_vert.glsl",file);
	}
	void free() { parent->free(); }
	int getUniform(string name) { return parent->getUniform(name); }
};

class LuaUserClientObject {
public:
	DUserClientObject* parent;
	LuaUserClientObject(DUserClientObject* _parent) { parent=_parent; }
	//boo
	void setShader(LuaUserClientShader shader) {
		((DUserClientFreeGlutObject*)parent)->shader=shader.parent;
	}
	void setFloat(int id,float value) {
		((DUserClientFreeGlutObject*)parent)->setFloat(id,value);
	}
	void addTexture(DResource* resource) {
		((DUserClientFreeGlutObject*)parent)->addTexture(resource);
	}
	void setTexture(int id,DResource* resource) {
		((DUserClientFreeGlutObject*)parent)->setTexture(id,resource);
	}
};

class LuaRenderNode {
	DRenderNode* parent;
public:
	LuaRenderNode(DRenderNode* _parent) { parent=_parent; }
//	void addNode(DRenderNode* node);
//	void removeNode(DRenderNode* node);
	LuaRenderNode createNode(dvect pos,double angle) { return LuaRenderNode(parent->createNode(pos,angle)); }

	void setPos(dvect pos) { parent->pos=dvect3(pos.x,pos.y,0); }
	void setAngle(float angle) { parent->angle=angle; }
	void setScale(dvect scale) { parent->scale=dvect3(scale.x,scale.y,1); }
	void setVisible(bool visible) { parent->visible=visible; }
	void setObject(LuaUserClientObject object) { parent->object=object.parent; }
};


class LuaUserClient {
	DUserClient* parent;
public:
	LuaUserClient(DUserClient* _parent) { parent=_parent; }
	void setAmbient(dvect3 ambient) { parent->setAmbient(ambient); }
	void setSunDir(dvect3 dir) { parent->setSunDir(dir); }
	void setSunColor(dvect3 color) { parent->setSunColor(color); }
	void color(float r,float g,float b,float a) { parent->color(r,g,b,a); }
	void moveTo(dvect pos) { parent->moveTo(pos); }
	void lineTo(dvect to) { parent->lineTo(to); }
	void saveMatrix() { parent->saveMatrix(); }
	void restoreMatrix() { parent->restoreMatrix(); }
	void transform(dvect pos,dvect scale,double rot) { parent->transform(pos,scale,rot); }
	void text(DResource* res,string text,float size) { parent->text(res,text,size); }
	dvect getMousePos() { return parent->getMousePos(); }
	bool getMousePressed(int key) { return parent->isMousePressed(key); }
	bool getMouseDown(int key) { return parent->isMouseDown(key); }
	LuaUserClientObject createObject(dvect size) { return LuaUserClientObject(parent->createObject(size)); }
};
class LuaGameResourceManager {
	GameResourceManager* parent;
public:
	LuaGameResourceManager(GameResourceManager* _parent) { parent=_parent; }
	DResource* getFont(string id) { return parent->getFont("media/font/"+id+".png"); }
	DResource* getImg(string id) { return parent->getImg(id); }
	//LuaGameLevel getLevel(string id) { return LuaGameLevel(parent->getLevel(id)); }
};

class LuaGameMenu : public LuaPropInterface<GameMenu*> {
public:
	LuaGameMenu(GameMenu* _parent) : LuaPropInterface<GameMenu*>(_parent) { }
	void close() { parent->close(); }
	void show(bool on) { parent->show(on); }
	LuaRenderNode getRootNode() { return parent->getNode(); }
};

class LuaGameWidget : public LuaPropInterface<GameWidget*> {
public:
//	GameWidget* parent;
	LuaGameWidget(GameWidget* _parent) : LuaPropInterface<GameWidget*>(_parent) {}
	bool mouseClicked() { return parent->mouseClicked(); }
	bool mouseDown() { return parent->mouseDown(); }
	bool mouseInside() { return parent->mouseInside(); }
	dvect mousePos() { return parent->mousePos(); }
	dvect getSize() { return parent->getSize(); }
	dvect getPos() { return parent->getPos(); }
	void show(bool on) { parent->show(on); }
};

class LuaGameCampaign {
	GameCampaign* parent;
public:
	LuaGameCampaign(GameCampaign* _parent) { parent = _parent; }
	string getPlayerName() { return parent->getPlayerName(); }
	void setPlayerName(string name) { parent->setPlayerName(name); }
	int getBudget() { return parent->getBudget(); }
	void setBudget(int budgetValue) { return parent->setBudget(budgetValue); }
	int subtractBudget(int bill) { return parent->subtractBudget(bill); }
	void addCar(string carId) { parent->addCar(carId); }
	vector<string> getCars() { return parent->getCars(); }
	int countCars() { return parent->countCars(); }
	bool isLevelUnlocked(string levelId) { return parent->isLevelUnlocked(levelId); }
	void setLevelUnlocked(string levelId) { parent->setLevelUnlocked(levelId); }
};

class LuaGameEntityChild {
	GameEntityChild * parent;
public:
	LuaGameEntityChild(GameEntityChild * _parent) {parent = _parent; }
	GameEntityChild * getParent() {return this->parent;}
	string getId() {return this->parent->id; }
	void setId(string i) {this->parent->id = i; }
	int getWeight() { return this->parent->weight; }
	void setWeight(int w) { this->parent->weight = w; }
};

class LuaGameEntity {
	GameEntity * parent;
public:
	LuaGameEntity(GameEntity * _parent) {parent = _parent;}
	GameEntity * getParent() {return this->parent;}
	LuaGameEntityChild getLightestChild() {return LuaGameEntityChild(parent->getLightestChild());}
	LuaGameEntityChild getHeaviestChild() {return LuaGameEntityChild(parent->getHeaviestChild());}
	void addChild(LuaGameEntityChild* c)  {parent->addChild(c->getParent());}
	LuaGameEntityChild removeChild(string id) {return LuaGameEntityChild(parent->removeChild(id));}
	int getScore() { return this->parent->score; }
	void setScore(int s) { this->parent->score = s; }
	map<string, string> getCars() {return this->parent->cars;}
	void addCar(string id) {this->parent->addCar(id);}
};

class LuaGameEntityTree {
	GameEntityTree * parent;
public:
	LuaGameEntityTree(GameEntityTree * _parent) { parent = _parent; }
	LuaGameEntity getEntity(string id) {return LuaGameEntity(parent->getEntity(id));}
	void addEntity(LuaGameEntity * ent) {parent->addEntity(ent->getParent()); }
	LuaGameEntity getFirst() {return LuaGameEntity(parent->getFirst()); }
};

class LuaGameCampaignDefinition {
	GameCampaignDefinition * parent;
public:
	LuaGameCampaignDefinition(GameCampaignDefinition * _parent) { parent = _parent; }
//	void addCar(LuaGameEntity * car) { parent->addCar(car->getParent());}
//	LuaGameEntityTree getCars() { return LuaGameEntityTree(parent->getCars()); }
	void addLevel(LuaGameEntity * level) { parent->addLevel(level->getParent());}
	LuaGameEntityTree getLevels() {return LuaGameEntityTree(parent->getLevels());}
};

class LuaGameTweaker {
public:
	void setTyreOutForceMult(float x) { R2DTyre::factor_out_force_mult=x; }
	void setTyreGripForceMult(float x) { R2DTyre::factor_grip_force_mult=x; }
	void setTyreSlideTreshold(float x) { R2DTyre::factor_slide_treshold=x; }
};

void luaapi_bind(lua_State* lua);

#endif
