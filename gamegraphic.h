#ifndef _GAMEGRAPHIC_H_
#define _GAMEGRAPHIC_H_

#include "duserclient.h"
#include "gameresourcemanager.h"

class GameGraphic {
public:

	DUserClient* client;

	//nodes
	DRenderNode* node_ground;
	DRenderNode* node_shadows;	//not used
	DRenderNode* node_objects;
//	DRenderNode* node_lights;
	DRenderNode* node_hud;
	DRenderNode* node_menu;

	/*
	//resources
	//DResource* resource_car;
	DResource* resource_brake_light;
	DResource* resource_reverse_light;
	//DResource* resource_car_shadow;
	DResource* resource_car_height;

	//maps
	DResource* resource_car_color;
	DResource* resource_car_normal;
	*/

	//fonts
	DResource* font_arial;

	GameGraphic(GameResourceManager* manager,DRenderNode* root);
	~GameGraphic();
};

#endif

