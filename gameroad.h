
#ifndef _GAMEROAD_H_
#define _GAMEROAD_H_

#include "duserclient.h"
#include "duserclientgeometry.h"
#include "gamestructs.h"
#include "gameutils.h"
#include "dquadtree.h"

#include <vector>


struct GameRoadGeometryMetapoint {
	dvect center;
	dvect left;	//relative
	//dvect right;
	float local_pos;
	int index;
	GameRoadGeometryMetapoint();
	GameRoadGeometryMetapoint(dvect center);
	GameRoadGeometryMetapoint(dvect center,dvect left,dvect right,float pos);
};

class GameRoadGeometry {
public:
	DQuadTree* tree;
	vector<GameRoadGeometryPoint> points;
	float app_len;	//approx-linear
	vector<GameRoadGeometryMetapoint> metapoints;	//for physics, ai

	GameRoadGeometry();

	void addPoint(GameRoadGeometryPoint point);
	float approxLength();
	//get from backend curve
	GameRoadGeometryMetapoint getCurveMetaLocal(int segment,float pos);	//0-1
	GameRoadGeometryMetapoint getCurveMeta(float pos);	//0-length

	//get from calculated metapoints(faster & more accurate)
	GameRoadGeometryMetapoint getMetaLocal(int segment,float pos);
	GameRoadGeometryMetapoint getMeta(float pos);	//0-length
	GameRoadGeometryMetapoint getClosest(dvect pos);
	GameRoadGeometryMetapoint getClosest(dvect pos,int start,int end);

	DUserClientGeometry* getGeometryOld();	//deprecated
	DUserClientGeometry* getGeometry();
	void postInit();
	void calcPoints();
};

class GameRoad {
public:
	GameRoadGeometry geometry;
	float surface_grip;
	float surface_roughness;
	bool cyclic;

	GameRoad() {}
	GameRoad(GameRoadDesc* desc);

	bool pointInside(dvect point);
};

#endif

