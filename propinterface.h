#ifndef _PROPINTERFACE_H_
#define _PROPINTERFACE_H_

#include <string>
#include <map>

template <class varType>
class PropInterfaceVars {
	varType def;	//default
	std::map<std::string,varType> item_map;
public:
	PropInterfaceVars(varType _def) {
		_def=def;
	}
	void set(std::string id,varType value) {
		item_map[id]=value;
	}
	varType get(std::string id) {
		typename std::map<std::string,varType>::iterator it=item_map.find(id);
		if(it==item_map.end()) return def;
		return it->second;
	}
};

class PropInterface {
	PropInterfaceVars<int> ints;
	PropInterfaceVars<float> floats;
	PropInterfaceVars<std::string> strings;
public:
	PropInterface();
	virtual int getPropInt(std::string prop);
	virtual void setPropInt(std::string prop,int v);
	virtual float getPropFloat(std::string prop);
	virtual void setPropFloat(std::string prop,float v);
	virtual std::string getPropString(std::string prop);
	virtual void setPropString(std::string prop,std::string v);
};

#endif
