
#include "dresource.h"

#ifndef _GAMESTRUCTS_H_
#define _GAMESTRUCTS_H_

#include "duserclient.h"

#include <string>
#include "dvect.h"

using namespace std;

/*
struct GameLight {
	DResource* color;
	dvect size;
	dvect center;
};
*/

enum GameShapeType {
	SHAPE_NONE,
	SHAPE_QUAD,
	SHAPE_CIRCLE,
	SHAPE_LINE,
	SHAPE_RAMP
};
struct GameShape {
	GameShapeType type;
	//applies to all types
	dvect start;
	dvect end;
	float bounce;
	float friction;

	GameShape(GameShapeType _type,dvect _start,dvect _end,float _bounce,float _friction) {
		type=_type;
		start=_start;
		end=_end;
		bounce=_bounce;
		friction=_friction;
	}
};

/*
struct GameTyre {
	dvect size;
	float grip;
};


enum GameShadowType { SHADOW_TYPE_BITMAP, SHADOW_TYPE_GEOMETRY };
struct GameStaticObject {
	dvect pos;
	dvect scale;
	GameShadowType shadow_type;
};
*/



struct GameObjectPart {
	/*
	DResource* color;
	DResource* normal;
	DResource* height;
	DResource* specular;
	*/
	string color_filename;
	string normal_filename;
	string height_filename;
	string specular_filename;

	dvect3 size;
	dvect3 pos;
	float angle;

	bool render;

	float bounce;
	float friction;

	GameShapeType shape_type;

	GameObjectPart(string color,string normal,string height,string specular,
			dvect3 _size,dvect3 _pos,float _angle,GameShapeType _shape_type,
			bool _render,float _bounce,float _friction) {
		color_filename=color;
		normal_filename=normal;
		height_filename=height;
		specular_filename=specular;
		size=_size;
		pos=_pos;
		angle=_angle;
		shape_type=_shape_type;
		render=_render;
		bounce=_bounce;
		friction=_friction;
	}
};

struct GameObjectLight {
	string color_filename;
	dvect size;
	dvect center;
	dvect pos;
	float angle;

	GameObjectLight(string color,dvect _size,dvect _center,dvect _pos,float _angle) {
		color_filename=color;
		size=_size;
		center=_center;
		pos=_pos;
		angle=_angle;
	}
};

struct GameCarTyre {
	dvect pos;
	float angle;
	bool powered;
	bool steerable;
	float steer_angle;

	GameCarTyre(dvect _pos,float _angle,bool _powered,bool _steerable,float _steer_angle=0) {
		pos=_pos;
		angle=_angle;
		powered=_powered;
		steerable=_steerable;
		steer_angle=_steer_angle;
	}
};

struct GameCarSound {
	string engine_start;
	string engine;

	GameCarSound(string _engine_start, string _engine) {
		engine_start=_engine_start;
		engine=_engine;
	}
};

struct GameCarBody {

	vector<GameObjectPart> parts;
	vector<GameCarSound> sounds;
	vector<GameCarTyre> tyres;
	vector<GameObjectLight> lights_front;
	vector<GameObjectLight> lights_reverse;
	vector<GameObjectLight> lights_brake;
	vector<GameObjectLight> lights_aux;

	dvect hook_pos;
	GameCarBody* hook_car;	//move this

	//dvect size;
	float mass;
	float mass_interia;
	dvect center_of_mass;

	float durability;

	float engine_power;

	string script;
};

/*
struct GameCarDesc {
	vector<GameCarBody*> bodies;
};
*/


//---roads---

struct GameRoadGeometryPoint {
	dvect pos;
	dvect control_1;	//beziers, relative to pos
	dvect control_2;
	float width;
	float local_pos;
	GameRoadGeometryPoint() {}
	GameRoadGeometryPoint(dvect _pos,dvect _control_1,dvect _control_2,float _width,
			float _local_pos=0) {
		pos=_pos;
		control_1=_control_1;
		control_2=_control_2;
		width=_width;
		local_pos=_local_pos;
	}
};

enum  GameRoadGeneratorType { GAMEROAD_GENERATOR_MANUAL, GAMEROAD_GENERATOR_RAND_POINTS };

struct GameRoadDesc {
	string color;

	dvect scale;
	dvect offset;	//not used
	float angle;	//not used

	float grip;
	float roughness;
	bool cyclic;

	GameRoadGeneratorType generator;
	int rand_points;

	vector<GameRoadGeometryPoint> points;

	GameRoadDesc() {
		generator=GAMEROAD_GENERATOR_MANUAL;
		cyclic=false;
	}
};

struct GameTrackObject {
	dvect pos;
	dvect size_scale;	//size or scale
	bool use_size;	//whether to use size or use scale
	float angle;
	string id;
	bool is_static;
};

struct GameTrackDesc {
	string colors[4];
	string alpha;
	dvect texture_mult;
	dvect size;

	float grips[4];
	float roughnesses[4];

	vector<GameRoadDesc> roads;
	vector<GameTrackObject> objects;
};

struct GameLevelDesc {
	string track;
	string script;

	string clouds;
	dvect clouds_vel;
	dvect clouds_scale;

	dvect3 sun_dir;
	dvect3 sun_color;
	dvect3 ambient_color;

	string game_type;	//ie. race

	string soundtrack;
//	vector<GameLevel>
};

struct GameImage {
	char* data;
	int width;
	int height;
	int depth;	//bytes per pixel, 1(alpha), 3(RGB), or 4(RGBA)
};


#endif


