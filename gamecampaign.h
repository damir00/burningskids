/*
 * gamecampaign.h
 *
 *  Created on: Apr 17, 2012
 *      Author: dejan
 */

#ifndef GAMECAMPAIGN_H_
#define GAMECAMPAIGN_H_

#include <string>
#include <vector>

using namespace std;

class GameCampaign {
public:
	GameCampaign();
	virtual ~GameCampaign();

	string getPlayerName();
	void setPlayerName(string name);

	int getBudget();
	void setBudget(int budgetValue);
	int subtractBudget(int bill);

	void addCar(string carId);
	vector<string> getCars();
	int countCars();

	bool isLevelUnlocked(string levelId);
	void setLevelUnlocked(string levelId);
	vector<string> getUnlockedLevels();

private:
	string playerName;
	int budget;
	vector<string> cars;
	vector<string> levels;


};

#endif /* GAMECAMPAIGN_H_ */
