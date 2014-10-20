#include "iostream"
#include "duserclientfreeglut.h"

#include <vector>
#include <math.h>

using namespace std;

double randDouble(double low,double high) {
	double r=((double)rand())/RAND_MAX;
	return r*(high-low)+low;
}

DUserClientObject* new_object(DUserClient* client,DResource* resource) {
	DUserClientObject* obj=client->loadObject(resource);
	obj->pos=dvect(randDouble(-1,1),randDouble(-1,1));
	obj->size=dvect(randDouble(3,4),randDouble(1,2));
	obj->angle=randDouble(0,3.14*2);
	return obj;
}

void update_object(DUserClientObject* obj) {
	obj->pos+=dvect(cos(obj->angle),sin(obj->angle))*0.005;
	obj->angle+=0.001;
}

void node_debug(DRenderNode* node,void* client_p) {
	DUserClient * client=(DUserClient*)client_p;

	client->color(0,0,1);
	client->moveTo(dvect(0.5,0.25));
	client->lineTo(dvect(-0.5,0.25));
	client->lineTo(dvect(-0.5,-0.25));
	client->lineTo(dvect(0.5,-0.25));
	client->lineTo(dvect(0.5,0.25));

}

DRenderNode* new_node(DUserClient* client,DResource* resource) {
	DRenderNode* node=new DRenderNode();
	DUserClientObject* obj=client->createObject(resource);
	obj->size=dvect(1,0.5);
	node->object=obj;
	node->scale=dvect(2,2);
	node->angle=randDouble(0,3.14*2);

	DRenderNode* left=new DRenderNode(dvect(-0.25,0.35),0);
	DRenderNode* right=new DRenderNode(dvect(-0.25,-0.35),0);

	left->object=obj;
	right->object=obj;

	left->scale=dvect(0.5,0.5);
	right->scale=dvect(0.5,0.5);

	node->addNode(left);
	node->addNode(right);

	node->post_draw=node_debug;
	left->post_draw=node_debug;
	right->post_draw=node_debug;

	return node;
}
void update_node(DRenderNode* node) {
	float speed=10;
	node->pos+=dvect(cos(node->angle),sin(node->angle))*0.005*speed;
	node->angle+=0.002*speed;
}

int main() {

	DUserClientConfig conf;
	conf.title="hello test";
	conf.fullscreen=false;
	conf.width=300;
	conf.height=200;

	DUserClientFreeGlut *client=new DUserClientFreeGlut(conf);

	DResource* resource=client->loadResource(IMAGE_TRANSPARENT,"car.png");
/*
	vector<DUserClientObject*> objects;

	for(int i=0;i<2000;i++) {
		objects.push_back(new_object(client,resource));
	}

	for(int i=0;i<5000;i++) {
		client->render(1000);
		usleep(1000);

		for(int o=0;o<objects.size();o++) {
			update_object(objects[o]);
		}
	}
*/

	DRenderNode* root=client->getRootNode();
	vector<DRenderNode*> nodes;
	for(int i=0;i<1000;i++) {
		DRenderNode* node=new_node(client,resource);
		nodes.push_back(node);
		root->addNode(node);
	}

	for(int i=0;i<5000;i++) {
		client->render(1000);
		usleep(1000);
		for(int o=0;o<nodes.size();o++) {
			update_node(nodes[o]);
		}
	}

	delete(client);
	return 0;
}

