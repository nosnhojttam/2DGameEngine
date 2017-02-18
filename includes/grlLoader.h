#ifndef GRLLOADER
#define GRLLOADER

#include "level.h"
#include "../pugixml\pugixml.hpp"
#include <unordered_map>


using namespace std;

class grlLoader
{
	public:
		grlLoader();
		level*								loadGRL(string filename);
		unordered_map<string, texture*>*	getTextureMap();
	private:
		string								getTagInner(string tag, string xml);
		vector<string>						getChildrenWithTag(string tag, string xml);
		unordered_map<string, texture*>*	textureMap;

};
#endif