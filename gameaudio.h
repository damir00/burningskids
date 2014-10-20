/*
 * gameaudio.h
 *
 *  Created on: Jun 12, 2012
 *      Author: matej
 */

#ifndef GAMEAUDIO_H_
#define GAMEAUDIO_H_

#include "gameutils.h"
#include "gamestructs.h"
#include <map>
#include <vector>

/** enumerated value */
typedef int GAenum;
#define GA_ENGINE_START		0x1001

#ifdef __APPLE__
	#include <OpenAL/alut.h>
#else
	#include <AL/al.h>
	#include <AL/alut.h>
#endif

using namespace std;

struct CarSounds {
	GameCarSound* data;

	vector<ALuint> engineSources; // Looping engine sounds
	vector<ALuint> disposableSources; // Sounds that can be disposed when stopped
	vector<ALuint> starterSources; // Looping engine sounds
};

class SoundManager {
	int screens;

	vector<ALuint> availableSources;
	vector<ALuint> usedSources;

	map<const char *, ALuint> usedBuffers;

	ALuint soundtrackSource;

	bool clearALErrors();
	bool clearALUTErrors();
	ALuint getSource();
	void returnSource(unsigned int source);

	ALuint getBuffer(const char* sample, bool persistent=true);
	void disposeBuffer(ALuint buf);
	bool clearAllErrors();
	bool isPlaying(ALuint source);
	void loadOgg(const char *fileName, vector <char> &buffer, ALenum &format, ALsizei &freq);

	void setSourcePosition(ALuint source, dvect pos);
public:
	SoundManager();
	virtual ~SoundManager();

	void disposeSounds(CarSounds* snds);
	void setListenerPosition(dvect pos);

	/* Car methods */
	void carStart(CarSounds* snds); // Makes starting sound and starts idle loop
	void carUpdate(CarSounds* snds, float speed, dvect pos);
	void carStop(CarSounds* snds, bool abrupt=false);
	void carSound(CarSounds* snds, const char* sample);

	/* Soundtrack methods */
	void soundtrackInit(string sndtrk);
	void soundtrackPlay();
	void soundtrackStop();
};

#endif /* GAMEAUDIO_H_ */
