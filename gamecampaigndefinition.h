/*
 * gamecampaigndefinition.h
 *
 *  Created on: Apr 18, 2012
 *      Author: dejan
 */

#ifndef GAMECAMPAIGNDEFINITION_H_
#define GAMECAMPAIGNDEFINITION_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class GameEntityChild {
public:
	int weight;
	string id;
};

class GameEntity {
public:
	string id;
	int score;
	map<string, GameEntityChild*> childs;
	map<string, string> cars;
	GameEntityChild* getLightestChild();
	GameEntityChild* getHeaviestChild();
	void addChild(GameEntityChild*);
	GameEntityChild* removeChild(string id);
	void addCar(string id);
	map<string, string> getCars();
};

class GameEntityTree {
public:
	string start;
	map<string, GameEntity*> entities;
	GameEntity* getEntity(string id);
	void addEntity(GameEntity * ent);
	GameEntity* getFirst();
};

class GameCampaignDefinition {
	GameEntityTree * levels;

public:
	GameCampaignDefinition();
	virtual ~GameCampaignDefinition();

	void addLevel(GameEntity * level);
	GameEntityTree * getLevels();
};

#endif /* GAMECAMPAIGNDEFINITION_H_ */
