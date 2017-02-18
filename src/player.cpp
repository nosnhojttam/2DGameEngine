#include "../includes/player.h"

player::player()
{
	this->numVertices = 6;
	vertices = new SimpleVertex[numVertices];

	/*vertices[0] = { XMFLOAT3(-5.0f, 0.0f, 15.0f), XMFLOAT2(0.0f, 0.0f) };
	vertices[1] = { XMFLOAT3(5.0f, 0.0f, 5.0f), XMFLOAT2(1.0f, 0.0f) };
	vertices[2] = { XMFLOAT3(5.0f, -10.0f, 5.0f), XMFLOAT2(1.0f, 1.0f) };
	vertices[3] = { XMFLOAT3(-5.0f, 0.0f, 5.0f), XMFLOAT2(0.0f, 0.0f) };
	vertices[4] = { XMFLOAT3(5.0f, -10.0f, 5.0f), XMFLOAT2(1.0f, 1.0f) };
	vertices[5] = { XMFLOAT3(-5.0f, -10.0f, 5.0f), XMFLOAT2(0.0f, 1.0f) };*/


	vertices[0] = { XMFLOAT3(-10.0f, -65.0f, 15.0f), XMFLOAT2(0.0f, 0.0f) };
	vertices[1] = { XMFLOAT3(10.0f, -65.0f, 5.0f), XMFLOAT2(1.0f, 0.0f) };
	vertices[2] = { XMFLOAT3(10.0f, -85.0f, 5.0f), XMFLOAT2(1.0f, 1.0f) };
	vertices[3] = { XMFLOAT3(-10.0f, -65.0f, 5.0f), XMFLOAT2(0.0f, 0.0f) };
	vertices[4] = { XMFLOAT3(10.0f, -85.0f, 5.0f), XMFLOAT2(1.0f, 1.0f) };
	vertices[5] = { XMFLOAT3(-10.0f, -85.0f, 5.0f), XMFLOAT2(0.0f, 1.0f) };
	
	this->position = XMFLOAT4(0,0,0,0);
	this->col = new collider(this->vertices, new XMFLOAT4(0, 0, 0, 0), numVertices);
	scale_factor =.005;
	this->tex = texture();
	this->anim = animation();
	this->tag = "The Player"; 
}






