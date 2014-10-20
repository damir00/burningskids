#ifndef _DUTILS_H_
#define _DUTILS_H_

#include <string>
#include "dvect.h"

float degToRad(float deg);
float radToDeg(float rad);
char* get_file_content(std::string filename);

dvect lerpVect(dvect p1,dvect p2,float x);
float lerpFloat(float p1,float p2,float x);

#endif

