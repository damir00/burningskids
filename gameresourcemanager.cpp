
#include "gameresourcemanager.h"

#include "il.h"
#include "ilu.h"
#include "ilut.h"

GameResourceManagerPool<GameImage*> GameResourceManager::images_data;

GameResourceManager::GameResourceManager(DUserClient* _client) {
	client=_client;

	images_data.init(client,&loadImgData);
	images.init(client,&loadImg);
	geometry.init(client,&loadGeometry);
	fonts.init(client,&loadFont);
	tracks.init(client,&loadTrack);
	bodies.init(client,&loadBody);
	levels.init(client,&loadLevel);
	soundManager = new SoundManager();
}

GameTrackDesc* GameResourceManager::loadTrack(DUserClient*,string id) {
	return XmlParser::parseTrack(id);
}
GameTrackDesc* GameResourceManager::getTrack(string filename) {
	return tracks.get(filename);
}

DUserClientGeometry* GameResourceManager::loadGeometry(DUserClient*,string filename) {
	GameImage* img=getImgData(filename);
	if(img && img->depth==4) {
		return DUserClientGeometry::generateFromBitmap(
				(unsigned char*)img->data,
				img->width,
				img->height,
				false,10);
	}
	return 0;
}
DUserClientGeometry* GameResourceManager::getGeometry(string filename) {
	return geometry.get(filename);
}

GameImage* GameResourceManager::loadImgData(DUserClient*,string filename) {
	ILuint image_id=0;
	ilGenImages(1,&image_id);
	ilBindImage(image_id);

	ILenum type=ilDetermineType(filename.c_str());
	if(!ilLoad(type,filename.c_str())) {
		printf("Failed to load image %s\n",filename.c_str());
		ilDeleteImage(image_id);
		return 0;
	}

	int w=ilGetInteger(IL_IMAGE_WIDTH);
	int h=ilGetInteger(IL_IMAGE_HEIGHT);
	int d=ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	if(d!=1 && d!=2 && d!=3 && d!=4) {
		printf("Unupported image depth %d, image %s\n",d,filename.c_str());
		ilDeleteImage(image_id);
		return 0;
	}

	if(d==2) d=1;

	char *data=new char[w*h*d];

	ILenum formats[]={0,IL_LUMINANCE,IL_LUMINANCE,IL_RGB,IL_RGBA};
	ilCopyPixels(0,0,0,w,h,1,formats[d],IL_UNSIGNED_BYTE,data);

	ilDeleteImage(image_id);

	GameImage* img=new GameImage();
	img->data=data;
	img->width=w;
	img->height=h;
	img->depth=d;
	return img;
}
GameImage* GameResourceManager::getImgData(string filename) {
	return images_data.get(filename);
}

DResource* GameResourceManager::loadImg(DUserClient* client,string filename) {
	/*
	DResource* r=client->loadResource(IMAGE_TRANSPARENT,filename);
	if(!r) cout<<"WARNING: Cannot load image "<<filename<<endl;
	return r;
	*/
	GameImage* img=getImgData(filename);
	if(!img) return client->loadResourceImage(0,0,0,0);	//returns empty resource
	DResource* r=client->loadResourceImage(img->data,img->width,img->height,img->depth);
	if(!r) cout<<"WARNING: Cannot load image "<<filename<<endl;
	return r;
}
DResource* GameResourceManager::getImg(string filename) {
	return images.get(filename);
}

DResource* GameResourceManager::loadFont(DUserClient* client,string filename) {
	DResource* r=client->loadResource(FONT,filename);
	if(!r) cout<<"WARNING: Cannot load font "<<filename<<endl;
	return r;
}
DResource* GameResourceManager::getFont(string filename) {
	return fonts.get(filename);
}

GameCarBody* GameResourceManager::loadBody(DUserClient*,string id) {
	return XmlParser::parseCar(id);
}
GameCarBody* GameResourceManager::getBody(string filename) {
	return bodies.get(filename);
}

GameLevelDesc* GameResourceManager::loadLevel(DUserClient* client,string id) {
	return XmlParser::parseLevel(id);
}
GameLevelDesc* GameResourceManager::getLevel(string filename) {
	return levels.get(filename);
}

SoundManager* GameResourceManager::getSoundManager() {
	return soundManager;
}
