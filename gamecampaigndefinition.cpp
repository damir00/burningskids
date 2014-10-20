/*
 * gamecampaigndefinition.cpp
 *
 *  Created on: Apr 18, 2012
 *      Author: dejan
 */

#include "gamecampaigndefinition.h"

GameCampaignDefinition::GameCampaignDefinition() {
	this->levels = new GameEntityTree();
}

GameCampaignDefinition::~GameCampaignDefinition() {
	delete this->levels;
}

void GameCampaignDefinition::addLevel(GameEntity* level) {
	this->levels->entities.insert(pair<string, GameEntity*>(level->id, level));
}

GameEntityTree* GameCampaignDefinition::getLevels() {
	return this->levels;
}


GameEntityChild* GameEntity::getLightestChild() {
	GameEntityChild * gec = NULL;

	if(this->childs.size() == 0)
		return NULL;

	for(std::map<string, GameEntityChild*>::iterator it = this->childs.begin(); it != this->childs.end(); ++it) {
		if(!gec) {
			gec = (*it).second;
		} else {
			if(gec->weight > (*it).second->weight)
				gec = (*it).second;
		}
	}

	return gec;
}

GameEntityChild* GameEntity::getHeaviestChild() {
	GameEntityChild * gec = NULL;

	if(this->childs.size() == 0)
		return NULL;

	for(std::map<string, GameEntityChild*>::iterator it = this->childs.begin(); it != this->childs.end(); ++it) {
		if(!gec) {
			gec = (*it).second;
		} else {
			if(gec->weight > (*it).second->weight)
				gec = (*it).second;
		}
	}

	return gec;
}

void GameEntity::addChild(GameEntityChild* c) {
	this->childs.insert(pair<string, GameEntityChild*>(c->id, c));
}

GameEntityChild* GameEntity::removeChild(string id) {
	map<string, GameEntityChild*>::iterator it = this->childs.find(id);
	this->childs.erase(it);

	return (*it).second;
}

void GameEntity::addCar(string id) {
	this->cars.insert(pair<string, string>(id, id));
}

map<string, string> GameEntity::getCars() {
	return this->cars;
}

void GameEntityTree::addEntity(GameEntity * e) {
	if(this->entities.size() < 1)
		this->start = e->id;
	this->entities.insert(pair<string, GameEntity*>(e->id, e));
}

GameEntity* GameEntityTree::getEntity(string id) {
	map<string, GameEntity*>::iterator it = this->entities.find(id);

	return (*it).second;
}
GameEntity* GameEntityTree::getFirst() {

	map<string, GameEntity*>::iterator it;
	it = this->entities.find(this->start);

	return (*it).second;
}
