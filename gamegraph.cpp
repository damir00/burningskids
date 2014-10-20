#include "gamegraph.h"

GameGraph::GameGraph(int _width,int num_samples) {
	width=_width;
	samples=num_samples;
	data=new float[width];
	for(int i=0;i<width;i++) {
		data[i]=0;
	}
	temp_val=0;
	temp_samples=0;
	offset=0;
}
GameGraph::~GameGraph() {
	delete(data);
}

void GameGraph::put(float value) {
	temp_val+=value;
	temp_samples++;
	if(temp_samples>=samples) {
		data[offset]=temp_val/((float)temp_samples);
		offset=(offset+1)%width;
		temp_val=0;
		temp_samples=0;
	}
}

float GameGraph::getData(int i) {
	return data[(i+offset)%width];
}
int GameGraph::getWidth() {
	return width;
}
