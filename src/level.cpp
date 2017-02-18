#include "../includes/level.h"

level::level()
{

}

level::level(string name)
{
	this->name = name;
}


void level::draw()
{

}

void level::addObject(gObject object)
{
	if (object.getTag() == "bg"){
		objects.insert(objects.begin(), object);
	}
	else { 
		objects.push_back(object); 
	}
}