#ifndef COLLIDER
#define COLLIDER

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <d3d11.h>
#include <d3dx11.h>
#include "simplevertex.h"


using namespace std;

class collider
{
public:
	collider();
	collider(SimpleVertex* vertices, XMFLOAT4* pos, int numVertices);
	XMFLOAT4* getPos();
	SimpleVertex* getSV();
	void translate(XMFLOAT4* newPos);
	int getNumVertices();
	bool collision(collider* c);

private:
	XMFLOAT4* pos;
	SimpleVertex* vertices;
	int numVertices;
};

#endif