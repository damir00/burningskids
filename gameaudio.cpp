/*
 * gameaudio.cpp
 *
 *  Created on: Jun 12, 2012
 *      Author: matej
 */

#include "gameaudio.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vorbis/vorbisfile.h>

#define SOURCES	256
#define BUFFERS	100
#define OGG_BUFFER_SIZE 1024*32

SoundManager::SoundManager() {
	if (!alutInit(0, NULL) || !clearAllErrors()) return;

	short i = 0;
	for (; i < SOURCES; i++) {
		ALuint source;
		alGenSources(1, &source);
		if (!clearALErrors()) break;
		availableSources.push_back(source);
	}
	cout << "openal: " << i << " sources generated" << endl;

	screens = 1;
	soundtrackSource = getSource();
}

ALuint SoundManager::getSource() {
	ALuint source = 0;
	if (availableSources.size() != 0) {
		source = availableSources.back();
		availableSources.pop_back();
		usedSources.push_back(source);
	}
	return source;
}

void SoundManager::returnSource(unsigned int source) {
	vector<ALuint>::iterator iter;
	iter = find(usedSources.begin(), usedSources.end(), source);

	if (iter != usedSources.end()) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if (state == AL_PLAYING) alSourceStop(source);
		clearALErrors();
		alSourcei( source, AL_BUFFER, 0 );
		availableSources.push_back(source);
		usedSources.erase(iter);
	}
}

ALuint SoundManager::getBuffer(const char* sample, bool persistent) {
	if (persistent) {
		map<const char *,ALuint>::iterator iter;
		iter = usedBuffers.find(sample);
		if (iter != usedBuffers.end()) {
			return iter->second;
		}
	}

	ALuint buffer;
	buffer = alutCreateBufferFromFile(sample);
	clearAllErrors();

	if (persistent) usedBuffers[sample] = buffer;
	return buffer;
}

void SoundManager::disposeBuffer(ALuint buf) {
	if (buf) {
		alDeleteBuffers(1, &buf);
		clearAllErrors();
	}
}

bool SoundManager::clearAllErrors() {
	return clearALErrors() | clearALUTErrors();
}

bool SoundManager::clearALErrors() {
	ALenum msg = alGetError();
	if (msg) {
		cout << "openal error: " << alutGetErrorString(msg) << endl;
		return false;
	}
	return true;
}

bool SoundManager::clearALUTErrors() {
	ALenum msg = alutGetError();
	if (msg) {
		cout << "openal error: " << alutGetErrorString(msg) << endl;
		return false;
	}
	return true;
}

bool SoundManager::isPlaying(ALuint source) {
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;

}

void SoundManager::loadOgg(const char *fileName, vector <char> &buffer, ALenum &format, ALsizei &freq) {
	int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
	int bitStream;
	long bytes;
	char array[OGG_BUFFER_SIZE];    // Local fixed size array
	FILE *f;

	// Open for binary reading
	f = fopen(fileName, "rb");
	vorbis_info *pInfo;
	OggVorbis_File oggFile;
	ov_open(f, &oggFile, NULL, 0);
	// Get some information about the OGG file
	pInfo = ov_info(&oggFile, -1);

	// Check the number of channels... always use 16-bit samples
	if (pInfo->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;
	// end if

	// The frequency of the sampling rate
	freq = pInfo->rate;
	do {
		// Read up to a buffer's worth of decoded sound data
		bytes = ov_read(&oggFile, array, OGG_BUFFER_SIZE, endian, 2, 1, &bitStream);
		// Append to end of buffer
		buffer.insert(buffer.end(), array, array + bytes);
	} while (bytes > 0);
	ov_clear(&oggFile);
}

void SoundManager::setSourcePosition(ALuint source, dvect pos) {
	float ppos[3] = {pos.x, 0, pos.y};
	alSourcefv(source, AL_POSITION, ppos);
}

void SoundManager::setListenerPosition(dvect pos) {
	float ppos[3] = {pos.x, 0, pos.y};
	alListenerfv(AL_POSITION, ppos);
}

SoundManager::~SoundManager() {
	alutExit();
}

void SoundManager::disposeSounds(CarSounds* snds) {

}

void SoundManager::carStart(CarSounds* snds) {
	ALuint engine_source, start_source;
	ALuint engine_buffer, start_buffer;
	for (int i = 0; i < screens; ++i) {
		engine_source = getSource();
		snds->engineSources.push_back(engine_source);

		clearAllErrors();
		engine_buffer = getBuffer(snds->data->engine.c_str());
		clearAllErrors();
		alSourcei( engine_source, AL_BUFFER, engine_buffer);
		clearAllErrors();
		alSourcei( engine_source, AL_LOOPING, 1);
		clearAllErrors();
		alSourcef(engine_source, AL_GAIN, 5);
		clearAllErrors();
		start_source = getSource();
		snds->starterSources.push_back(start_source);

		clearAllErrors();
		start_buffer = getBuffer(snds->data->engine_start.c_str());
		clearAllErrors();
		alSourcei(start_source, AL_BUFFER, start_buffer);
		clearAllErrors();
		alSourcePlay(start_source);
		clearAllErrors();
	}
}

void SoundManager::carUpdate(CarSounds* snds, float speed, dvect pos) {
	for (int i = 0; i < screens; i++) {
		ALuint engine_source = snds->engineSources.at(i);
		setSourcePosition(engine_source, pos);
		ALint state;

		ALuint start_source = snds->starterSources.at(i);
		alGetSourcei(start_source, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING) {

			alGetSourcei(engine_source, AL_SOURCE_STATE, &state);
			if (state != AL_PLAYING) alSourcePlay(engine_source);

			clearAllErrors();
			float pitch = speed * 42 + .5;

			alSourcef(engine_source, AL_PITCH, pitch); // top speed == 0.035
	//		m->getAudio()->clearAllErrors();
		}
	}
}

void SoundManager::carStop(CarSounds* snds, bool abrupt) {

}

void SoundManager::carSound(CarSounds* snds, const char* sample) {

}

void SoundManager::soundtrackInit(string sndtrk) {
	soundtrackStop();

	if (sndtrk.length() < 5) return;
	ALuint buf;
	if (!sndtrk.compare(sndtrk.length() - 4, 4, ".wav")) {
		buf = getBuffer(sndtrk.c_str(), false);
	} else if (!sndtrk.compare(sndtrk.length() - 4, 4, ".ogg")) {
		vector<char> bufferData;
		ALenum format;
		ALsizei freq;

		loadOgg(sndtrk.c_str(), bufferData, format, freq);
		alGenBuffers(1, &buf);
		alBufferData(buf, format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), freq);
		clearAllErrors();
	} else return;

	alSourcei(soundtrackSource, AL_BUFFER, buf);
	clearAllErrors();
}

void SoundManager::soundtrackPlay() {
	alSourcePlay(soundtrackSource);
	clearAllErrors();
}

void SoundManager::soundtrackStop() {
	if (isPlaying(soundtrackSource)) alSourceStop(soundtrackSource);
	ALint buf;
	alGetSourcei(soundtrackSource, AL_BUFFER, &buf);
	disposeBuffer(buf);
}
