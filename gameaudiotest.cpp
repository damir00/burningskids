/*
 * gameaudiotest.cpp
 *
 *  Created on: Jun 12, 2012
 *      Author: matej
 */

//#include "gameaudio.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <AL/al.h>
#include <AL/alut.h>
#include <termios.h>
#include <termio.h>

using namespace std;

bool clearALErrors() {
	ALenum msg = alGetError();
	if (msg) {
		cout << "openal error: " << alutGetErrorString(msg) << endl;
		return false;
	}
	return true;
}

bool clearALUTErrors() {
	ALenum msg = alutGetError();
	if (msg) {
		cout << "openal error: " << alutGetErrorString(msg) << endl;
		return false;
	}
	return true;
}

bool clearAllErrors() {
	return clearALErrors() | clearALUTErrors();
}

int main(int argc, char **argv) {
	alutInit(0, NULL);
	clearAllErrors();

	ALuint source;
	alGenSources(1, &source);
	clearALErrors();

	ALuint buffer;
	buffer = alutCreateBufferFromFile("media/audio/RacingEngineLoExt.wav");
	clearALErrors();
	alSourcei(source, AL_BUFFER, buffer);
	clearALErrors();
	alSourcei(source, AL_LOOPING, 1);

	alSourcePlay(source);

	struct termios oldT, newT;
	char c;

	ioctl(0,TCGETS,&oldT); /*get current mode */

	newT=oldT;
	newT.c_lflag &= ~ECHO; /* echo off */
	newT.c_lflag &= ~ICANON; /*one char @ a time*/

	ioctl(0,TCSETS,&newT); /* set new terminal mode */

	float pitch = 1.;
	float distort = 1.;
	while (read(0,&c,1))
	{
		switch (c) {
		case 'a': pitch += .01; break;
		case 'z': pitch -= .01; break;
		case 's': distort += .01; break;
		case 'x': distort -= .01; break;
		case 'm': goto end;
		}

		printf("pitch=%.2f distort=%.2f\n", pitch, distort);
		alSourcef(source, AL_PITCH, pitch);
		alSourcef(source, AL_GAIN, distort);
	}

end:
	ioctl(0,TCSETS,&oldT); /* restore previous terminal mode */
}
