
#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "duserclient.h"
#include "r2dlib.h"
#include "gameutils.h"
#include "gamestructs.h"
#include "gamegraphic.h"
#include "gameresourcemanager.h"
#include <vector>

using namespace std;

class GameObject {
public:
	DRenderNode* node;
	R2DObject* object;
	vector<GameShape> shapes;
	GameObject(DUserClientObject* obj,DRenderNode* parent,dvect pos=dvect(0,0),float angle=0);
	GameObject(GameCarBody* body,DRenderNode* node,GameResourceManager* manager);
	~GameObject();
	void update(long delta);

	void setAngle(float a);
	float getAngle();
	void setPos(dvect pos);
	dvect getPos();

};

#endif

