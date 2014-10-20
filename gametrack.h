
#ifndef _GAMETRACK_H_
#define _GAMETRACK_H_

#include "chipmunk.h"
#include "duserclient.h"
#include "gamecar.h"
#include "gameaudio.h"
//#include "gameobject.h"
#include "dutils.h"
#include "gameutils.h"
#include "gameroad.h"
#include "gamestructs.h"
#include "gamegraphic.h"
#include "dquadtree.h"

#include "duserclientfreeglut.h"

#include <vector>
using namespace std;

struct GameTrackPosition {
	cpVect pos;
	float angle;
	GameTrackPosition();
	GameTrackPosition(cpVect _pos,float _angle);
};

enum GameTrackDecalType { DECAL_TYPE_SIMPLE, DECAL_TYPE_CURVE };

class GameTrackDecal : public DQuadTreeNode {
public:
	DRenderNode* render_node;
	long end_ts;	//end timestamp in ms
	GameTrackDecalType type;

	GameTrackDecal() {
		end_ts=-1;
	}
};


class GameTrack {

	DResource* ground_image;
	GameTrackPosition start_positions[100];
	int num_start_positions;

	R2DSim* sim;
	DQuadTree* quadtree;

	DRenderNode* parent_node;
	DRenderNode* render_node;	//holds terrain & skids
	vector<DUserClientObject*> road_objects;
	vector<GameCar*> objects;	//dynamic active objects
	vector<GameRoad*> roads;

	//vector<DRenderNode*> skids;

	DResource* resource_skid;

	float grips[4];
	float roughnesses[4];

	dvect size;

	int num_cars;

	long runtime;	//miliseconds

	void updateCarSensors(GameCar* car);

	void setupObject(GameCar* obj,bool is_static);

	DRenderNode* getNewTyreSkid();

	std::vector<GameTrackDecal*> skids;

	void updateSkids();

public:
	GameResourceManager* manager;
	DUserClientObject * ground;
	vector<GameCar*> cars;
	vector<DRenderNode*> iterated_nodes;

	float update_radius;
	dvect update_rad2;	//cached for update_radius
	dvect update_position;

	long lifetime_skids;

	GameTrack(GameTrackDesc* desc,GameGraphic* graphics,
			GameResourceManager* manager,DRenderNode* parent);
	~GameTrack();
	void setResource(DResource* r);
	DResource* getResource();

	void addStartPosition(cpVect pos,float angle);
	int getNumStartPositions();
	GameTrackPosition getStartPosition(int i);

	void addCar(GameCar* car);
	void addObject(GameCar* obj,bool is_static=false);
	void addRoad(DUserClient* client,GameRoad* road,DResource* res);

	void initGround(DUserClientObject* obj,dvect size);

	void update(long delta);

	void _surfaceQuery(R2DSurfaceQuery* query);
	//used by lua
	dvect surfaceQuery2(dvect pos);	//v.x=grip, v.y=roughness

	dvect getSize();
	unsigned int getRoadSize();
	GameRoad* getRoad(unsigned int i);

	void iterateTreeNodes(DBoundingBox* box);

	void _tree_wake_objects(GameCar* obj);

	void addDecal(DUserClientObject* object,dvect pos,float rot);
	void addSimpleDecal(DResource* resource,DUserClientObjectRenderFlags render_flag,
			dvect pos,dvect size,float rot);
	void addLight(DResource* resource,dvect pos,dvect size,float rot);

	long getRuntime();	//miliseconds
};

#endif

