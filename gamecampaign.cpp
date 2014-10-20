/*
 * gamecampaign.cpp
 *
 *  Created on: Apr 17, 2012
 *      Author: dejan
 */

#include "gamecampaign.h"

GameCampaign::GameCampaign() {

}

GameCampaign::~GameCampaign() {

}

string GameCampaign::getPlayerName() {
	return this->playerName;
}
void GameCampaign::setPlayerName(string name) {
	this->playerName = name;
}

int GameCampaign::getBudget() {
	return this->budget;
}
void GameCampaign::setBudget(int budgetValue) {
	this->budget = budgetValue;
}
int GameCampaign::subtractBudget(int bill) {
	this->budget -= bill;
	return this->budget;
}

void GameCampaign::addCar(string carId) {
	this->cars.push_back(carId);
}
vector<string> GameCampaign::getCars() {
	return this->cars;
}

int GameCampaign::countCars() {
	return this->cars.size();
}

bool GameCampaign::isLevelUnlocked(string levelId) {
	bool state = false;
	for(std::vector<string>::iterator it = this->levels.begin(); it != this->levels.end(); ++it) {
		string& level = *it;
		if(level == levelId)
			state = true;
	}

	return state;
}
void GameCampaign::setLevelUnlocked(string levelId) {
	this->levels.push_back(levelId);
}
vector<string> GameCampaign::getUnlockedLevels() {
	return this->levels;
}
