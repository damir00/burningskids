#ifndef _GAMEGRAPH_H_
#define _GAMEGRAPH_H_

class GameGraph {
	int width;
	int samples;
	float *data;
	int offset;

	float temp_val;
	int temp_samples;
public:

	GameGraph(int width,int num_samples);
	~GameGraph();

	void put(float value);

	float getData(int i);
	int getWidth();
};

#endif
