
#include "gameroad.h"

#include <stdio.h>

class GameRoadTreeNode : public DQuadTreeNode {
public:
//	GameRoad* road;
	int segment_start;
	int segment_end;
};

GameRoadGeometryMetapoint::GameRoadGeometryMetapoint() {
	local_pos=0;
	index=0;
}
GameRoadGeometryMetapoint::GameRoadGeometryMetapoint(dvect _center,dvect _left,
		dvect _right,float _local_pos) {
	center=_center;
	left=_left;
//	right=_right;
	local_pos=_local_pos;
	index=0;
}
GameRoadGeometryMetapoint::GameRoadGeometryMetapoint(dvect _center) {
	center=_center;
	left=dvectzero;
	local_pos=0;
	index=0;
}

GameRoadGeometry::GameRoadGeometry() {
	tree=0;
}

void GameRoadGeometry::addPoint(GameRoadGeometryPoint point) {
	points.push_back(point);
}

void GameRoadGeometry::calcPoints() {
	//calculate metapoints

	if(tree) {
		delete(tree);
		tree=0;
	}

	metapoints.clear();
	if(points.size()<2) return;

	//length query samples per segment
	int len_query_samples=1000;

	float queried_len=0;
	GameRoadGeometryMetapoint prev_pos=getCurveMetaLocal(0,0);
	GameRoadGeometryMetapoint pos=prev_pos;

	int metapoints_num=0;
	//distance between 2 metapoints ^2
	float metapoints_dist2=2;

	DBoundingBox road_bb;
	float max_width=0;

	for(int i=0;i<points.size()-1;i++) {

		for(int q=1;q<len_query_samples;q++) {

			if(queried_len>metapoints_num*metapoints_dist2) {
				pos.local_pos=queried_len;

				if(metapoints.size()==0) {
					road_bb.start=road_bb.end=pos.center;
				}
				else {
					road_bb.merge(pos.center);
				}

				metapoints.push_back(pos);
				metapoints_num++;
			}

			float p=((float)q)/((float)(len_query_samples-1));

			pos=getCurveMetaLocal(i,p);
			//TODO: make this faster by using length_fast
			queried_len+=(prev_pos.center-pos.center).length();
			prev_pos=pos;

			//metapoints.push_back(pos);
		}
		if(points[i].width>max_width) max_width=points[i].width;
		//queried_len=sqrt(queried_len);
	}
	pos.local_pos=queried_len;
	metapoints.push_back(pos);

	road_bb.merge(pos.center);

	app_len=queried_len;
	cout<<"road len: "<<queried_len<<endl;


	//calc quadtree
	road_bb.grow(max_width/2);
	float leaf_size=5;
	dvect bb_size=road_bb.getSize();
	float full_size=max(bb_size.x,bb_size.y);

	int depth=1;

	while(full_size>leaf_size) {
		full_size/=2;
		depth++;
	}

	tree=new DQuadTree(road_bb.start,road_bb.end,depth,0);

	int points_pack=10;

	for(int i=0;i<metapoints.size();i+=points_pack) {
		DBoundingBox bb;
		bb.start=bb.end=metapoints[i].center;

		for(int m=i;m<i+points_pack && m<metapoints.size();m++) {
			int id=m;

			bb.merge(metapoints[id].center);
		}
		bb.grow(max_width/2);

		GameRoadTreeNode* node=new GameRoadTreeNode();
		node->bb=bb;
		node->segment_start=i;
		node->segment_end=i+points_pack;
		tree->putNode(node);
	}

}


void GameRoadGeometry::postInit() {

	//calculate local_pos and app_len

	//TODO: local_pos on points don't matter anymore, remove calculations
	app_len=0;
	if(points.size()==0) return;

	for(int i=0;i<points.size()-1;i++) {
		points[i].local_pos=app_len;
		app_len+=(points[i].pos-points[i+1].pos).length_fast();
	}
	for(int i=0;i<points.size()-1;i++) {
		points[i].local_pos/=app_len;
	}
	points[points.size()-1].local_pos=1;
	app_len=sqrt(app_len);

	calcPoints();

	for(int i=0;i<metapoints.size();i++) {
		metapoints[i].local_pos/=app_len;
	}

}
float GameRoadGeometry::approxLength() {
	return app_len;
}

//get metadata on a segment, pos=[0-1]
GameRoadGeometryMetapoint GameRoadGeometry::getCurveMetaLocal(int segment,float pos) {

	dvect p1=points[segment].pos;
	dvect p2=points[segment+1].pos;
	dvect c1=p1+points[segment].control_1;
	dvect c2=p2+points[segment+1].control_2;

	dvect x=lerpVect(c1,c2,pos);
	dvect t1=lerpVect(p1,x,pos);
	dvect t2=lerpVect(x,p2,pos);

	dvect center=lerpVect(t1,t2,pos);
	dvect n1=t1-t2;
	n1=dvect(-n1.y,n1.x).normalize();	//normal
	n1*=lerpFloat(points[segment].width,points[segment+1].width,pos);
	return GameRoadGeometryMetapoint(center,n1,-n1,pos);
}

GameRoadGeometryMetapoint GameRoadGeometry::getCurveMeta(float pos) {
	if(points.size()==0) return GameRoadGeometryMetapoint();
	if(points.size()==1) return GameRoadGeometryMetapoint(points[0].pos);

	pos/=app_len;

	int i=0;
	for(i=0;i<points.size()-1;i++) {
		if(points[i+1].local_pos>pos) break;
	}
	float n_pos=(pos-points[i].local_pos)/(points[i+1].local_pos-points[i].local_pos);

	return getCurveMetaLocal(i,n_pos);
}

GameRoadGeometryMetapoint GameRoadGeometry::getMetaLocal(int segment,float pos) {
	GameRoadGeometryMetapoint p;
	p.center=lerpVect(metapoints[segment].center,metapoints[segment+1].center,pos);
	p.left=lerpVect(metapoints[segment].left,metapoints[segment+1].left,pos);
//	p.right=-p.left;
	p.local_pos=lerpFloat(metapoints[segment].local_pos,metapoints[segment+1].local_pos,pos);
	return p;
}

GameRoadGeometryMetapoint GameRoadGeometry::getMeta(float pos) {
	//return GameRoadGeometryMetapoint(dvect(0,0));

	pos/=app_len;

	/*
	int i=0;
	for(i=0;i<metapoints.size()-1;i++) {
		if(metapoints[i+1].local_pos>pos) break;
	}
	*/

	int i=min((int)metapoints.size()-1,(int)(pos*metapoints.size())); //approx

	if(metapoints[i].local_pos>pos) {
		//need to go down
		i--;
		for(;i>=0;i--) {
			if(metapoints[i].local_pos<pos) break;
		}
	}
	else {
		for(;i<metapoints.size()-1;i++) {
			if(metapoints[i+1].local_pos>pos) break;
		}
	}

	i=max(0,i);

	float n_pos=(pos-metapoints[i].local_pos)/(metapoints[i+1].local_pos-metapoints[i].local_pos);

	return getMetaLocal(i,n_pos);
}

GameRoadGeometryMetapoint GameRoadGeometry::getClosest(dvect pos) {
	return getClosest(pos,0,metapoints.size());
}
GameRoadGeometryMetapoint GameRoadGeometry::getClosest(dvect pos,int start,int end) {

	if(metapoints.size()==0) return GameRoadGeometryMetapoint();
	if(metapoints.size()==1) return GameRoadGeometryMetapoint(metapoints[0].center);

	if(end==0) end=metapoints.size();

	float closest_x=GameMath::lineClosestX(metapoints[start].center,
			metapoints[start+1].center,pos);
	dvect closest=GameMath::dvectlerp(metapoints[start].center,
			metapoints[start+1].center,closest_x);

	float dist=(pos-closest).length_fast();
	float lpos=lerpFloat(metapoints[start].local_pos,metapoints[start+1].local_pos,closest_x);
	dvect left=lerpVect(metapoints[start].left,metapoints[start+1].left,closest_x);
	int index=start;

	for(int i=start+1;i<end-1;i++) {
		int in=i%metapoints.size();
		int in2=(i+1)%metapoints.size();

		closest_x=GameMath::lineClosestX(metapoints[in].center,metapoints[in2].center,pos);
		dvect next_closest=GameMath::dvectlerp(metapoints[in].center,metapoints[in2].center,closest_x);
		float next_dist=(pos-next_closest).length_fast();

		if(next_dist<dist) {
			dist=next_dist;
			closest=next_closest;
			lpos=lerpFloat(metapoints[in].local_pos,metapoints[in2].local_pos,closest_x);
			left=lerpVect(metapoints[in].left,metapoints[in2].left,closest_x);
			index=i;
		}
	}
	GameRoadGeometryMetapoint m;
	m.center=closest;
	m.local_pos=lpos*app_len;
	m.left=left;
	m.index=index % metapoints.size();

	return m;

	/*
	if(metapoints.size()==0) return GameRoadGeometryMetapoint();
	if(metapoints.size()==1) return GameRoadGeometryMetapoint(metapoints[0].center);

	float closest_x=GameMath::lineClosestX(metapoints[0].center,metapoints[1].center,pos);
	dvect closest=GameMath::dvectlerp(metapoints[0].center,metapoints[1].center,closest_x);
	float dist=(pos-closest).length_fast();
	float lpos=lerpFloat(metapoints[0].local_pos,metapoints[1].local_pos,closest_x);
	dvect left=lerpVect(metapoints[0].left,metapoints[1].left,closest_x);

	for(int i=1;i<metapoints.size()-1;i++) {
		closest_x=GameMath::lineClosestX(metapoints[i].center,metapoints[i+1].center,pos);
		dvect next_closest=GameMath::dvectlerp(metapoints[i].center,metapoints[i+1].center,closest_x);
		float next_dist=(pos-next_closest).length_fast();

		if(next_dist<dist) {
			dist=next_dist;
			closest=next_closest;
			lpos=lerpFloat(metapoints[i].local_pos,metapoints[i+1].local_pos,closest_x);
			left=lerpVect(metapoints[i].left,metapoints[i+1].left,closest_x);
		}
	}
	GameRoadGeometryMetapoint m;
	m.center=closest;
	m.local_pos=lpos*app_len;
	m.left=left;
	return m;
	*/
}


DUserClientGeometry* GameRoadGeometry::getGeometryOld() {
	DUserClientGeometry* geom=new DUserClientGeometry();

	static float points_per_unit=0.2;

	float len=approxLength();
	int points=len/points_per_unit;
	if(points<2) points=2;

	/*
		2 vertices per point = 2*points *3
		6 indices per 4 vertices = ((points-1)*2)*3
	*/
	unsigned int vertices=2*points *3;
	unsigned int texcoords=2*points *2;
	unsigned int indices=(points-1)*2*3;

	DUserClientGeometryChunk* chunk=new DUserClientGeometryChunk(vertices,texcoords,indices);

	dvect prev_center;
	float real_pos=0;
	for(int i=0;i<points;i++) {

		float pos=((float)i)/((float)points)*len;

		GameRoadGeometryMetapoint mp=getMeta(pos);

		if(i!=0) {
			real_pos+=(mp.center-prev_center).length();
		}
		float width=(mp.left).length()*2;

		prev_center=mp.center;

		int v_i=i*6;
		int v_tex=i*4;
		dvect pos_left=-(mp.center+mp.left);
		dvect pos_right=-(mp.center-mp.left);

		chunk->vertices[v_i]=pos_left.x;
		chunk->vertices[v_i+1]=pos_left.y;
		chunk->vertices[v_i+2]=0;
		chunk->tex_coords[v_tex]=1;
		chunk->tex_coords[v_tex+1]=real_pos/width;

		chunk->vertices[v_i+3]=pos_right.x;
		chunk->vertices[v_i+4]=pos_right.y;
		chunk->vertices[v_i+5]=0;
		chunk->tex_coords[v_tex+2]=0;
		chunk->tex_coords[v_tex+3]=real_pos/width;
	}

	/*
		0 2 4 6 8
		1 3 5 7 9
		triangle 1: 0 1 2
		triangle 2: 1 2 3
		triangle 3: 2 3 4
		triangle 4: 3 4 5
		etc.
	*/

	for(int i=0;i<points-1;i++) {
		int i2=i*2;
		int i6=i*6;

		chunk->indices[i6]=i2*3;
		chunk->indices[i6+1]=(i2+1)*3;
		chunk->indices[i6+2]=(i2+2)*3;

		chunk->indices[i6+3]=(i2+1)*3;
		chunk->indices[i6+4]=(i2+2)*3;
		chunk->indices[i6+5]=(i2+3)*3;

		chunk->tex_indices[i6]=i2*2;
		chunk->tex_indices[i6+1]=(i2+1)*2;
		chunk->tex_indices[i6+2]=(i2+2)*2;

		chunk->tex_indices[i6+3]=(i2+1)*2;
		chunk->tex_indices[i6+4]=(i2+2)*2;
		chunk->tex_indices[i6+5]=(i2+3)*2;
	}
	geom->addChunk(chunk);

	return geom;
}

DUserClientGeometry* GameRoadGeometry::getGeometry() {
	DUserClientGeometry* geom=new DUserClientGeometry();

	float len=approxLength();
	int points=metapoints.size();

	/*
		2 vertices per point = 2*points *3
		6 indices per 4 vertices = ((points-1)*2)*3
	*/
	unsigned int vertices=2*points *2;
	unsigned int texcoords=2*points *2;
	unsigned int indices=(points-1)*2*3;

	DUserClientGeometryChunk* chunk=new DUserClientGeometryChunk(vertices,texcoords,indices);

	dvect prev_center;
	float real_pos=0;
	for(int i=0;i<points;i++) {

		float pos=((float)i)/((float)points)*len;

		//GameRoadGeometryMetapoint mp=getMeta(pos);
		GameRoadGeometryMetapoint mp=metapoints[i];

		if(i!=0) {
			real_pos+=(mp.center-prev_center).length();
		}
		float width=(mp.left).length()*2;

		prev_center=mp.center;

		int v_i=i*4;
		int v_tex=i*4;
		dvect pos_left=-(mp.center+mp.left);
		dvect pos_right=-(mp.center-mp.left);

		chunk->vertices[v_i]=pos_left.x;
		chunk->vertices[v_i+1]=pos_left.y;
		//chunk->vertices[v_i+2]=0;
		chunk->tex_coords[v_tex]=1;
		chunk->tex_coords[v_tex+1]=real_pos/width;

		chunk->vertices[v_i+2]=pos_right.x;
		chunk->vertices[v_i+3]=pos_right.y;
		//chunk->vertices[v_i+5]=0;
		chunk->tex_coords[v_tex+2]=0;
		chunk->tex_coords[v_tex+3]=real_pos/width;

		if(i==0) {
			chunk->bb.start=chunk->bb.end=pos_left;
			chunk->bb.merge(pos_right);
		}
		else {
			chunk->bb.merge(pos_left);
			chunk->bb.merge(pos_right);
		}
	}

	/*
		0 2 4 6 8
		1 3 5 7 9
		triangle 1: 0 1 2
		triangle 2: 1 2 3
		triangle 3: 2 3 4
		triangle 4: 3 4 5
		etc.
	*/

	for(int i=0;i<points-1;i++) {
		int i2=i*2;
		int i6=i*6;

		/*
		chunk->indices[i6]=i2*3;
		chunk->indices[i6+1]=(i2+1)*3;
		chunk->indices[i6+2]=(i2+2)*3;

		chunk->indices[i6+3]=(i2+1)*3;
		chunk->indices[i6+4]=(i2+2)*3;
		chunk->indices[i6+5]=(i2+3)*3;
		*/

		chunk->indices[i6]=i2;
		chunk->indices[i6+1]=(i2+1);
		chunk->indices[i6+2]=(i2+2);

		chunk->indices[i6+3]=(i2+1);
		chunk->indices[i6+4]=(i2+2);
		chunk->indices[i6+5]=(i2+3);

		chunk->tex_indices[i6]=i2*2;
		chunk->tex_indices[i6+1]=(i2+1)*2;
		chunk->tex_indices[i6+2]=(i2+2)*2;

		chunk->tex_indices[i6+3]=(i2+1)*2;
		chunk->tex_indices[i6+4]=(i2+2)*2;
		chunk->tex_indices[i6+5]=(i2+3)*2;
	}
	geom->addChunk(chunk);

	return geom;
}


GameRoad::GameRoad(GameRoadDesc* desc) {
	if(desc->generator==GAMEROAD_GENERATOR_RAND_POINTS) {
		float road_angle=rand_float(0,6.24);
		dvect pos=dvect(0,0);
		for(int i=0;i<desc->rand_points;i++) {
			dvect dir=dvect(cos(road_angle),sin(road_angle));
			GameRoadGeometryPoint p(pos,dir*20,-dir*20,4);
			geometry.addPoint(p);
			road_angle+=rand_float(-1,1);
			pos+=dir*40;
		}
	}
	else {
		for(vector<GameRoadGeometryPoint>::iterator i=desc->points.begin();i!=desc->points.end();i++) {
			geometry.addPoint(*i);
		}
	}
	geometry.postInit();
	surface_grip=desc->grip;
	surface_roughness=desc->roughness;
	cyclic=desc->cyclic;
}

bool query_inside;
dvect query_point;
int segments_queried;
bool pointInsideIterate(DQuadTreeNode* node,void* data) {

	if(query_inside) {
		return false;
	}


	GameRoad* road=(GameRoad*)data;
	GameRoadTreeNode* road_node=(GameRoadTreeNode*)node;

	GameRoadGeometryMetapoint p;
	p=road->geometry.getClosest(query_point,road_node->segment_start,road_node->segment_end);
	segments_queried++;

	float width=road->geometry.points[0].width;
	//dvect c=road->geometry.getMeta(p.local_pos).center;
	dvect c=road->geometry.metapoints[p.index].center;
	if((c-query_point).length_fast()<width*width) {
		query_inside=true;
	}

	return false;
}

//TODO: optimize this (GameRoad::pointInside and getClosest) !!!
bool GameRoad::pointInside(dvect point) {
	if(geometry.points.size()==0) return false;

	segments_queried=0;

	query_inside=false;
	query_point=point;
	geometry.tree->iterateNodes(point,pointInsideIterate,this);

	return query_inside;

	/*
	dvect c=geometry.getMeta(geometry.getClosest(point).local_pos).center;
	float width=geometry.points[0].width;

	if((c-point).length_fast()<width*width)
		return true;
	return false;
	*/
}






