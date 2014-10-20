#include "xmlparser.h"

using namespace std;

namespace XmlParser {

	GameShapeType shapeTypeFromStr(string str) {
		if(str=="quad") return SHAPE_QUAD;
		if(str=="circle") return SHAPE_CIRCLE;
		if(str=="line") return SHAPE_LINE;
		if(str=="ramp") return SHAPE_RAMP;
		return SHAPE_NONE;
	}
	GameRoadGeneratorType roadGeneratorTypeFromStr(string str) {
		if(str=="rand-points") return GAMEROAD_GENERATOR_RAND_POINTS;
		return GAMEROAD_GENERATOR_MANUAL;
	}

	string nodeName(xmlNodePtr node) {
		return (string)((char*)node->name);
	}

	bool propExists(xmlNodePtr node,string field) {
		xmlChar* v=xmlGetProp(node,(const xmlChar*)field.c_str());
		if(!v) return false;
		xmlFree(v);
		return true;
	}

	string parsePropString(xmlNodePtr node,string field,string def="") {
			xmlChar* value=xmlGetProp(node,(const xmlChar*)field.c_str());
			if(!value) {
					return def;
			}
			string r=(string)((char*)value);
			xmlFree(value);
			return r;
	}
	int parsePropInt(xmlNodePtr node,string field,int def) {
	        xmlChar* value=xmlGetProp(node,(const xmlChar*)field.c_str());
	        if(!value) return def;
	        int r=atoi((const char*)value);
	        xmlFree(value);
	        return r;
	}
	float parsePropFloat(xmlNodePtr node,string field,float def=0) {
		xmlChar* value=xmlGetProp(node,(const xmlChar*)field.c_str());
		if(!value) return def;
		float r=atof((const char*)value);
		xmlFree(value);
		return r;
	}
	bool parsePropBool(xmlNodePtr node,string field,bool def) {
	        xmlChar* value=xmlGetProp(node,(const xmlChar*)field.c_str());
	        if(!value) return def;
	        string v=(char*)value;
	        bool r=(v=="true" || v=="TRUE" || v=="1");
	        xmlFree(value);
	        return r;
	}
	dvect parsePropVect(xmlNodePtr node,string field,dvect def=dvect(0,0)) {
		return dvect(
				parsePropFloat(node,field+".x",def.x),
				parsePropFloat(node,field+".y",def.y));
	}
	dvect3 parsePropVect3(xmlNodePtr node,string field,dvect3 def=dvect3(0,0,0)) {
		return dvect3(
				parsePropFloat(node,field+".x",def.x),
				parsePropFloat(node,field+".y",def.y),
				parsePropFloat(node,field+".z",def.z));
	}
	dvect3 parsePropColor(xmlNodePtr node,string field,dvect3 def=dvect3(0,0,0)) {
		return dvect3(
				parsePropFloat(node,field+".r",def.x),
				parsePropFloat(node,field+".g",def.y),
				parsePropFloat(node,field+".b",def.z));
	}

	GameObjectPart parseObjectPart(xmlNodePtr node) {
		return GameObjectPart(
				parsePropString(node,"color"),
				parsePropString(node,"normal"),
				parsePropString(node,"height"),
				parsePropString(node,"specular"),
				parsePropVect3(node,"size",dvect3(1,1,1)),
				parsePropVect3(node,"pos"),
				parsePropFloat(node,"angle"),
				shapeTypeFromStr(parsePropString(node,"shape","none")),
				parsePropBool(node,"render",true),
				parsePropFloat(node,"bounce",0.5),
				parsePropFloat(node,"friction",0.5));
	}
	GameCarTyre parseTyre(xmlNodePtr node) {
		return GameCarTyre(
				parsePropVect(node,"pos"),
				parsePropFloat(node,"angle"),
				parsePropBool(node,"powered",true),
				parsePropBool(node,"steerable",true),
				parsePropFloat(node,"steer_angle"));
	}
	GameCarSound parseSound(xmlNodePtr node) {
		return GameCarSound(
				parsePropString(node,"engine_start", "media/audio/CarStarter3.wav"),
				parsePropString(node,"engine", "media/audio/RacingEngineLoExt.wav"));
	}
	GameObjectLight parseLight(xmlNodePtr node) {
		return GameObjectLight(
				parsePropString(node,"texture"),
				parsePropVect(node,"size",dvect(1,1)),
				parsePropVect(node,"center",dvect(0.5,0.5)),
				parsePropVect(node,"pos"),
				parsePropFloat(node,"angle"));
	}

	DUserClientConfig parseConfig(string filename) {
		DUserClientConfig conf("DRIVABLE",300,200,false);
		xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
		if (doc == NULL) {
			printf("Failed to parse config file %s\n", filename.c_str());
			return conf;
		}
		xmlNode* root=xmlDocGetRootElement(doc);
		conf.width=parsePropInt(root,"width",conf.width);
		conf.height=parsePropInt(root,"height",conf.height);
		conf.fullscreen=parsePropBool(root,"fullscreen",conf.fullscreen);
		conf.aa_enabled=parsePropBool(root,"enable-antialias",conf.aa_enabled);
		conf.shadow_quality=parsePropFloat(root,"shadow-quality",0.5);
		conf.flat_shadows=parsePropBool(root,"shadow-casting",true);
		conf.window_x=parsePropInt(root,"x",0);
		conf.window_y=parsePropInt(root,"y",0);
		return conf;
	}

	GameCarBody* parseCar(string filename) {

		xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
		if (doc == NULL) {
			printf("Failed to parse car %s\n", filename.c_str());
			return NULL;
		}

		xmlNode* root=xmlDocGetRootElement(doc);

		GameCarBody* body=new GameCarBody();
		string basedir=baseDir(filename);

		body->center_of_mass=parsePropVect(root,"mass_center");
		body->hook_pos=parsePropVect(root,"hook_pos");
		//body->size=parsePropVect(root,"size",dvect(1,1));
		body->mass=parsePropFloat(root,"mass",1);
		body->mass_interia=parsePropFloat(root,"rot_mass",1);
		body->engine_power=parsePropFloat(root,"engine_power",1);

		for(xmlNodePtr child=root->children;child!=NULL;child=child->next) {
			if (child->type != XML_ELEMENT_NODE) continue;
			string name=nodeName(child);
			if(name=="part") {
				GameObjectPart part=parseObjectPart(child);
				part.color_filename=basedir+part.color_filename;
				part.height_filename=basedir+part.height_filename;
				part.normal_filename=basedir+part.normal_filename;
				part.specular_filename=basedir+part.specular_filename;
				body->parts.push_back(part);
			}
			else if(name=="tyre") {
				body->tyres.push_back(parseTyre(child));
			}
			else if(name=="light_front") {
				body->lights_front.push_back(parseLight(child));
			}
			else if(name=="light_brake") {
				body->lights_brake.push_back(parseLight(child));
			}
			else if(name=="light_reverse") {
				body->lights_reverse.push_back(parseLight(child));
			}
			else if(name=="sound") {
				body->sounds.push_back(parseSound(child));
			}
		}

		xmlFreeDoc(doc);
		return body;
	}

	GameRoadDesc parseRoad(xmlNodePtr node) {
		GameRoadDesc desc;

		desc.angle=parsePropFloat(node,"angle",0);
		desc.offset=parsePropVect(node,"offset",dvect(0,0));
		desc.scale=parsePropVect(node,"scale",dvect(1,1));
		desc.color=parsePropString(node,"color","");
		desc.grip=parsePropFloat(node,"grip",1);
		desc.roughness=parsePropFloat(node,"roughness",0);
		desc.cyclic=parsePropBool(node,"cyclic",false);
		desc.generator=roadGeneratorTypeFromStr(parsePropString(node,"generator"));

		desc.rand_points=parsePropInt(node,"rand.points",10);

		for(xmlNodePtr child=node->children;child!=NULL;child=child->next) {
			if (child->type != XML_ELEMENT_NODE) continue;
			string name=nodeName(child);

			if(name=="point") {
				desc.points.push_back(GameRoadGeometryPoint(
						parsePropVect(child,"pos"),
						parsePropVect(child,"c1"),
						parsePropVect(child,"c2"),
						parsePropFloat(child,"width",1)));
			}
		}

		return desc;
	}
	GameTrackObject parseTrackObject(xmlNodePtr node) {
		GameTrackObject obj;

		obj.pos=parsePropVect(node,"pos");
		obj.angle=parsePropFloat(node,"angle");
		obj.is_static=parsePropBool(node,"static",false);
		obj.id=parsePropString(node,"id","<none>");
		if(propExists(node,"size")) {
			obj.use_size=true;
			obj.size_scale=parsePropVect(node,"size",dvect(1,1));
		}
		else {
			obj.use_size=false;
			obj.size_scale.x=parsePropFloat(node,"scale",1);
			//if no param specified, scale is set to 1,1
		}

		return obj;
	}

	GameTrackDesc* parseTrack(string filename) {
		xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
		if (doc == NULL) {
			printf("Failed to parse track %s\n", filename.c_str());
			return NULL;
		}

		xmlNode* root=xmlDocGetRootElement(doc);
		GameTrackDesc* track=new GameTrackDesc();

		track->alpha=parsePropString(root,"alpha","");
		track->colors[0]=parsePropString(root,"color1","");
		track->colors[1]=parsePropString(root,"color2","");
		track->colors[2]=parsePropString(root,"color3","");
		track->colors[3]=parsePropString(root,"color4","");
		track->texture_mult=parsePropVect(root,"texture-mult",dvect(1,1));
		track->size=parsePropVect(root,"size",dvect(100,100));

		track->grips[0]=parsePropFloat(root,"grip1",1);
		track->grips[1]=parsePropFloat(root,"grip2",1);
		track->grips[2]=parsePropFloat(root,"grip3",1);
		track->grips[3]=parsePropFloat(root,"grip4",1);
		track->roughnesses[0]=parsePropFloat(root,"roughness1",1);
		track->roughnesses[1]=parsePropFloat(root,"roughness2",1);
		track->roughnesses[2]=parsePropFloat(root,"roughness3",1);
		track->roughnesses[3]=parsePropFloat(root,"roughness4",1);


		for(xmlNodePtr child=root->children;child!=NULL;child=child->next) {
			if (child->type != XML_ELEMENT_NODE) continue;
			string name=nodeName(child);
			if(name=="road") {
				GameRoadDesc d=parseRoad(child);
				track->roads.push_back(d);
			}
			else if(name=="object") {
				GameTrackObject o=parseTrackObject(child);
				track->objects.push_back(o);
			}
		}


		return track;
	}

	GameLevelDesc* parseLevel(string filename) {
		xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
		if (doc == NULL) {
			printf("Failed to parse level %s\n", filename.c_str());
			return NULL;
		}

		xmlNode* root=xmlDocGetRootElement(doc);
		GameLevelDesc* level=new GameLevelDesc();

		level->clouds=parsePropString(root,"clouds","");
		level->clouds_scale=parsePropVect(root,"clouds-scale",dvect(1,1));
		level->clouds_vel=parsePropVect(root,"clouds-vel");
		level->track=parsePropString(root,"track");
		level->ambient_color=parsePropColor(root,"ambient-color");
		level->sun_color=parsePropColor(root,"sun-color");
		level->sun_dir=parsePropVect3(root,"sun-dir");
		level->sun_dir.normalize();

		level->script=baseDir(filename)+"/level.lua";
		level->game_type=parsePropString(root,"game-type","race");

		level->soundtrack=parsePropString(root, "soundtrack");

		return level;
	}

	GameCampaignDefinition* parseCampaign(string filename) {
		xmlDocPtr doc = xmlReadFile(filename.c_str(), NULL, 0);
		if (doc == NULL) {
			printf("Failed to parse campaign %s\n", filename.c_str());
			return NULL;
		}

		xmlNode* root=xmlDocGetRootElement(doc);
		GameCampaignDefinition * camp = new GameCampaignDefinition();
		camp->getLevels()->start = parsePropString(root, "start");
		printf("Start level: %s\n", camp->getLevels()->start.c_str());

		int i = 0;
		for(xmlNodePtr lev=root->children;lev!=NULL;lev=lev->next) {
			if (lev->type != XML_ELEMENT_NODE) continue;
			printf("%d: level ", i);
			GameEntity * level = new GameEntity();
			level->id = parsePropString(lev, "id");
			level->score = parsePropInt(lev, "score", 0);
			printf("id:%s score:%d\n", level->id.c_str(), level->score);

			for(xmlNodePtr levelchilds=lev->children;levelchilds!=NULL;levelchilds=levelchilds->next) {
				if (levelchilds->type != XML_ELEMENT_NODE) continue;
				if(nodeName(levelchilds) == string("levels")) {
					printf("\tLevels: ");
					for(xmlNodePtr nextlevel=levelchilds->children;nextlevel!=NULL;nextlevel=nextlevel->next) {
						if (nextlevel->type != XML_ELEMENT_NODE) continue;
						GameEntityChild * gec = new GameEntityChild();
						gec->id = parsePropString(nextlevel, "id");
						gec->weight = parsePropInt(nextlevel, "weight", 0);
						level->addChild(gec);

						printf("id:%s ", gec->id.c_str());
					}
					printf("\n");
				} else if(nodeName(levelchilds) == string("cars")) {
					printf("\tCars: ");
					for(xmlNodePtr car=levelchilds->children;car!=NULL;car=car->next) {
						if (car->type != XML_ELEMENT_NODE) continue;
						string carId = parsePropString(car, "id");
						level->addCar(carId);
						printf("id:%s ", carId.c_str());
					}
					printf("\n");
				}
			}
			camp->addLevel(level);

			i++;
		}

		return camp;
	}

}


