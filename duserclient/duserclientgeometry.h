
#ifndef _DUSERCLIENTGEOMETRY_H_
#define _DUSERCLIENTGEOMETRY_H_

#include "dresource.h"
#include "dvect.h"
#include "dboundingbox.h"

#include <vector>

using namespace std;

struct DUserClientGeometryChunk {
	float *vertices;
	float *tex_coords;
	unsigned int *indices;
	unsigned int *tex_indices;

	unsigned int num_indices;
	unsigned int num_vertices;		//number of floats stored

	//TODO: is this used?
	dvect pos;
	dvect size;

	DBoundingBox bb;

	//hack for GL
	unsigned int gl_vbo_flat_shadow_pos;

	DUserClientGeometryChunk();
	DUserClientGeometryChunk(int num_vertices,int num_texcoords,int num_indices);
	~DUserClientGeometryChunk();
};

class DUserClientGeometry : public DResource {

public:
	vector<DUserClientGeometryChunk*> chunks;
	DBoundingBox bb;

	//generates approximate mesh from RGBA image
	//vertical: if true, shape is calculated vertically, else horizontally
	//num_samples: number of points to calculate
	//mesh geometry is (0,0)-(1,1)
	static DUserClientGeometry* generateFromBitmap(unsigned char* image,int w,int h,
			bool vertical,int num_samples);

	void addChunk(DUserClientGeometryChunk* chunk);

	DUserClientGeometry();
	~DUserClientGeometry();
};

#endif


