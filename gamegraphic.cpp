
#include "gamegraphic.h"

GameGraphic::GameGraphic(GameResourceManager* manager,DRenderNode* root) {

	client=manager->client;

	font_arial=manager->getFont("media/font/arial.png");

	node_ground=root->createNode();
	node_shadows=0; //root->createNode();
	node_objects=root->createNode();
//	node_lights=root->createNode();
	node_hud=client->getOverlayNode()->createNode(); //root->createNode();
	node_menu=client->getOverlayNode()->createNode();

}
