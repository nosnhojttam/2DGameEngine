#ifndef LEVEL
#define LEVEL

#include "gObject.h"

class level
{
	public:
		level();
		level(string name);
		void draw();
		vector<gObject> objects;
		void addObject(gObject object);
	private:
		string name;
};

#endif

