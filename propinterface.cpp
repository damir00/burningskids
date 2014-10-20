
#include "propinterface.h"

PropInterface::PropInterface() :
	ints(0),floats(0),strings("") {
}

int PropInterface::getPropInt(std::string prop) {
	return ints.get(prop);
}
void PropInterface::setPropInt(std::string prop,int v) {
	ints.set(prop,v);
}
float PropInterface::getPropFloat(std::string prop) {
	return floats.get(prop);
}
void PropInterface::setPropFloat(std::string prop,float v) {
	floats.set(prop,v);
}
std::string PropInterface::getPropString(std::string prop) {
	return strings.get(prop);
}
void PropInterface::setPropString(std::string prop,std::string v) {
	strings.set(prop,v);
}
