#include "../includes/collider.h"

collider::collider() 
{

}

collider::collider(SimpleVertex* vertices, XMFLOAT4* pos, int numVertices) 
{
	this->vertices = vertices;
	this->pos = pos;
	this->numVertices = numVertices;
}

XMFLOAT4* collider::getPos() {
	return pos;
}

SimpleVertex* collider::getSV() {
	return vertices;
}

int collider::getNumVertices() {
	return numVertices;
}

void collider::translate(XMFLOAT4* newPos) {

	float oldX = pos->x;
	float oldY = pos->y;
	float oldZ = pos->z;
	float oldW = pos->w;

	pos->x += newPos->x;
	pos->y += newPos->y;
	pos->z += newPos->z;
	pos->w += newPos->w;
	
	// change vertices
	for (int i = 0; i < numVertices; i++) {
		vertices[i].Pos.x += newPos->x;
		vertices[i].Pos.y += newPos->y;
		vertices[i].Pos.z += newPos->z;
	}

}



bool collider::collision(collider* c) {

	// get Simple Vertices from other object
	SimpleVertex* other_v = c->getSV();

	// checks each vertex in a collider if it is within another collider using Barycentric Coordinates
	for (int i = 0; i < numVertices; i++) {
		XMFLOAT3 v = vertices[i].Pos;

		for (int j = 0; j < c->getNumVertices(); j += 3) {
			// get points of the other object's triangle
			XMFLOAT3 p0 = other_v[j].Pos;
			XMFLOAT3 p1 = other_v[j + 1].Pos;
			XMFLOAT3 p2 = other_v[j + 2].Pos;

			// Barycentric Coordinates
			float A = 0.5 * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
			float sign = A < 0 ? -1.0f : 1.0f;
			float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * v.x + (p0.x - p2.x) * v.y) * sign;
			float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * v.x + (p1.x - p0.x) * v.y) * sign;

			// collision detected
			if (s > 0 && t > 0 && (s + t) < 2 * A * sign) {
				return true;
			}
		}
	}

	/*
	// Rectangle Collision for Top Left point of Player
	XMFLOAT3 p = vertices[0].Pos;
	SimpleVertex* otherObj = c->getSV();
	XMFLOAT3 topLeft = otherObj[0].Pos;
	XMFLOAT3 botRight = otherObj[2].Pos;

	for (int i = 0; i < numVertices; i++) {
		if (vertices[i].Pos.x > topLeft.x && vertices[i].Pos.x < botRight.x &&
			vertices[i].Pos.y < topLeft.y && vertices[i].Pos.y > botRight.y) {

			cout << "jpsladf" << endl;
			return true;
		}
	}
	*/

	return false;
}
