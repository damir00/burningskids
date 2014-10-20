
#include "duserclientgeometry.h"

#include <stdio.h>

DUserClientGeometryChunk::DUserClientGeometryChunk() {
	vertices=NULL;
	indices=NULL;
	tex_coords=NULL;
	tex_indices=NULL;
	gl_vbo_flat_shadow_pos=0;
}
DUserClientGeometryChunk::DUserClientGeometryChunk(
		int _num_vertices,int _num_texcoords,int _num_indices) {
	vertices=new float[_num_vertices];	//Address 0x52af700 is 8 bytes after a block of size 1,824 alloc'd
	tex_coords=new float[_num_texcoords];	//Address 0x52189f0 is 0 bytes after a block of size 1,824 alloc'd
	indices=new unsigned int[_num_indices];
	tex_indices=new unsigned int[_num_indices];
	num_indices=_num_indices;
	num_vertices=_num_vertices;
	gl_vbo_flat_shadow_pos=0;
}
DUserClientGeometryChunk::~DUserClientGeometryChunk() {
	delete(vertices);
	delete(tex_coords);
	delete(indices);
	delete(tex_indices);
}


DUserClientGeometry::DUserClientGeometry() {
}
DUserClientGeometry::~DUserClientGeometry() {
}

void DUserClientGeometry::addChunk(DUserClientGeometryChunk* chunk) {
	if(chunks.size()==0) {
		bb=chunk->bb;
	}
	else {
		bb.merge(&chunk->bb);
	}
	chunks.push_back(chunk);
}

DUserClientGeometry* DUserClientGeometry::generateFromBitmap(
		unsigned char* image,int w,int h,
		bool vertical,int num_samples) {

	if(num_samples<=1) return 0;

	int num_vertices=num_samples*4;

	DUserClientGeometryChunk* c=new DUserClientGeometryChunk(num_vertices,0,0);

	int img_stride=w*4;

	int last_found_sample=-1;
	for(int i=0;i<num_samples;i++) {
		float p=(float)i/(float)(num_samples-1);

		float top=1;
		float bottom=0;

		int pixel=p*w;
		int offset=pixel*4+3;
		bool found=false;
		for(int t=0;t<h;t++) {
			if(image[offset+t*img_stride]>128) {
				top=(float)(w-t)/(float)w;
				found=true;
				break;
			}
		}

		for(int t=h-1;t>=0;t--) {
			if(image[offset+t*img_stride]>128) {
				bottom=(float)(w-t)/(float)w;
				found=true;
				break;
			}
		}

		int id=i*2;
		if(found) {
			c->vertices[id+0]=p;
			c->vertices[id+1]=top;
			c->vertices[num_vertices-(id+1)]=bottom;
			c->vertices[num_vertices-(id+2)]=p;

			for(int l=last_found_sample+1;l<i;l++) {
				int ld=l*2;
				c->vertices[ld+0]=p;
				c->vertices[ld+1]=top;
				c->vertices[num_vertices-(ld+1)]=bottom;
				c->vertices[num_vertices-(ld+2)]=p;
			}

			last_found_sample=i;
		}
	}
	if(last_found_sample!=num_samples-1) {
		//move last samples to last found sample position
		int ld=last_found_sample*2;
		for(int i=num_samples-1;i>=last_found_sample;i--) {
			int id=i*2;
			c->vertices[id+0]=c->vertices[ld+0];
			c->vertices[id+1]=c->vertices[ld+1];
			c->vertices[num_vertices-(id+1)]=c->vertices[num_vertices-(ld+1)];
			c->vertices[num_vertices-(id+2)]=c->vertices[num_vertices-(ld+2)];
		}
	}

	DUserClientGeometry* geom=new DUserClientGeometry();
	geom->addChunk(c);

	return geom;
}




