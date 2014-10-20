
#ifndef _GAMECAR_H_
#define _GAMECAR_H_

#include "r2dlib.h"
#include "duserclient.h"
#include "dutils.h"
#include "gameutils.h"
#include "gamegraphic.h"
#include "gameaudio.h"
#include "gamestructs.h"
#include "gameresourcemanager.h"
#include "dquadtree.h"
#include "gameroad.h"
#include "gamescript.h"

#include <vector>

class GameCar : public DQuadTreeNode {

	bool is_brake;
	int lap;
	float score;
	float final_score;
	bool is_done;	//finished race?
	int rank;	//starts at 0
	bool is_car;

	void init(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
		float scale);

	void brake_on(bool on);
	void reverse_on(bool on);
	void addLights(std::vector<DRenderNode*> *dst,std::vector<GameObjectLight> *src,
			GameResourceManager* m,bool visible,float scale);
	void sensorsDataInit();
	void sensorsUpdate();

	std::vector<DRenderNode*> parts;
	dvect3 total_size;
	CarSounds sounds;
	GameResourceManager* resource_manager;

public:
	bool controls_locked;
	float last_acc;
	float last_turn;

	//closest point to road
	GameRoadGeometryMetapoint road_closest_prev;
	GameRoadGeometryMetapoint road_closest;

//	std::vector<DUserClientObject*> tyre_skids;
	std::vector<DRenderNode*> tyre_skids;

	DRenderNode* render_node;
	//DRenderNode* lights_node;

	std::vector<DRenderNode*> lights_front;
	std::vector<DRenderNode*> lights_brake;
	std::vector<DRenderNode*> lights_reverse;
	std::vector<DRenderNode*> lights_aux;

	std::vector<GameShape> shapes;
	R2DCar* physic_object;
	GameCarBody* carBody;

	string name;

	float* sensor_data;
	int sensor_pixels;
	bool sensor_enabled;
	float sensor_spread;
	float sensor_length;

	GameScript script;

//	GameCar();
	GameCar(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
			float scale=1);
	//will make it static!
	GameCar(GameCarBody* body,GameGraphic* graphic,GameResourceManager* manager,
			dvect3 pos,float angle,float scale=1);
	~GameCar();
	void set(cpVect pos,float angle);
	//void setSize(dvect size);
	void update(GameResourceManager* m, long time);
	void initEngine(GameResourceManager* manager);

	//control
	//all control functions must be called before update
	void accelerate(float power);	//- 1
	void acc_idle();
	void brake(float power);		//0-1
	void turn(float x);	//-1-1
	void handbrake();

	dvect getPos();
	dvect getPrevPos();
	dvect getHeadingVect();
	float getAngleDeg();
	float getSpeed();
	dvect getSpeedVect();
	dvect3 getTotalSize();

	void sensorsInit(int num_pixels,float spread_angle,float length);
	void sensorsEnable(bool on);
	float *getSensorData(); //distances
	int getSensorPixels();
	bool getSensorEnabled();
	float getSensorSpread();
	float getSensorLength();
	float getSensorAngle(int index);
	cpLayers getLayers();	//collision layers

	void setControlsLocked(bool locked);

	//scoring
	int getLap();
	void setLap(int lap);
	float getScore();
	void setScore(float s);
	int getRank();
	void setRank(int rank);

	void setFinalScore(float s);	//sets isDone to true
	float getFinalScore();
	bool isDone();
};

#endif
