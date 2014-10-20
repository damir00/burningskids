
#ifndef _DIOENGINE_H_
#define _DIOENGINE_H_

#include "dvect.h"

#include <string>
using namespace std;

class DIOEngine {

public:
	//r/w
	virtual bool openSection(string id)=0;
	virtual void closeSection()=0;

	//arrays
	virtual bool getArraySize(string id)=0;

	//reading
	virtual bool openEntity(string id)=0;
	virtual void closeEntity()=0;

	virtual bool getBool(string id,bool def)=0;
	virtual int getInt(string id,int def)=0;
	virtual float getFloat(string id,float def)=0;
	virtual string getString(string id,string def)=0;

	//writing
	virtual void createEntity(string id)=0;
	virtual void commitEntity()=0;

	virtual void setBool(string id,bool value)=0;
	virtual void setInt(string id,int value)=0;
	virtual void setFloat(string id,float value)=0;
	virtual void setString(string id,string value)=0;
};

#endif


