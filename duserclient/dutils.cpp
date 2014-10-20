
#include "dutils.h"

#include <stdio.h>
#include <string.h>

using namespace std;

float degToRad(float deg) {
	return deg/57.2957795;
}
float radToDeg(float rad) {
	return rad*57.2957795;
}
char* get_file_content(string filename) {
	FILE* f=fopen(filename.c_str(),"r");

	if(!f) return NULL;

	fseek(f,0,SEEK_END);
	long file_size = ftell(f);
	fseek(f,0,SEEK_SET);

	char* data=new char[file_size+1];
	memset(data,0,file_size+1);
	fread(data,file_size,1,f);
	fclose(f);

	return data;
}

dvect lerpVect(dvect p1,dvect p2,float x) {
	return p1*(1-x)+p2*x;	
}
float lerpFloat(float p1,float p2,float x) {
	return p1*(1-x)+p2*x;
}

