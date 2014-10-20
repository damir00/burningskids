
#ifndef _GAMERESOURCEMANAGER_H_
#define _GAMERESOURCEMANAGER_H_

#include <string>
#include <vector>

#include "dresource.h"
#include "duserclient.h"
#include "gamestructs.h"
#include "xmlparser.h"
#include "gameaudio.h"

using namespace std;

template <class itemType>
struct GameResourceManagerItem {
	string id;
	itemType item;
	GameResourceManagerItem(string _id,itemType _item) {
		id=_id;
		item=_item;
	}
};
template <class itemType>
class GameResourceManagerPool {
	//TODO: replace with hashmap
	vector<GameResourceManagerItem<itemType> > items;

	DUserClient* client;
public:
	void init(DUserClient* _client,itemType (*_load_func) (DUserClient*,string) ) {
		client=_client;
		load_func=_load_func;
	}
	void add(string id,itemType item) {
		items.push_back(GameResourceManagerItem<itemType>(id,item));
	}
	itemType get(string id) {
		for(int i=0;i<items.size();i++) {
			if(items[i].id==id) {
				return items[i].item;
			}
		}
		itemType t=load_func(client,id);
		if(t) add(id,t);
		return t;
	}
	itemType (*load_func)(DUserClient* client,string id);
};

class GameResourceManager {

	static GameResourceManagerPool<GameImage*> images_data;
	GameResourceManagerPool<DResource*> images;
	GameResourceManagerPool<DUserClientGeometry*> geometry;

	GameResourceManagerPool<DResource*> fonts;
	GameResourceManagerPool<GameTrackDesc*> tracks;
	GameResourceManagerPool<GameCarBody*> bodies;
	GameResourceManagerPool<GameLevelDesc*> levels;
	SoundManager* soundManager;

	static DResource* loadFont(DUserClient*,string id);
	static GameImage* loadImgData(DUserClient*,string filename);
	static DResource* loadImg(DUserClient*,string id);
	static DUserClientGeometry* loadGeometry(DUserClient*,string id);
	static GameTrackDesc* loadTrack(DUserClient*,string id);
	static GameCarBody* loadBody(DUserClient*,string id);
	static GameLevelDesc* loadLevel(DUserClient*,string id);

public:
	DUserClient* client;
	GameResourceManager(DUserClient* client);

	DResource* getFont(string filename);
	static GameImage* getImgData(string filename);
	DResource* getImg(string filename);
	DUserClientGeometry* getGeometry(string filename);
	GameCarBody* getBody(string filename);
	GameTrackDesc* getTrack(string filename);
	GameLevelDesc* getLevel(string filename);

	SoundManager* getSoundManager();
};

#endif

