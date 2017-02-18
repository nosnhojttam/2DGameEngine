#pragma once
#ifndef GOBJECT
#define GOBJECT

#include "texture.h"
#include "simplevertex.h"
#include "constantbuffer.h"
#include "collider.h"
#include "animation.h"
#include <vector>

class gObject
{
	public:
		gObject();
		gObject(texture* tex, string tag, SimpleVertex* vertices, int numVertices);
		void draw(ID3D11DeviceContext* g_pImmediateContext, ID3D11Buffer* constant_buffer, long elapsed, ID3D11SamplerState* sampler);
		string getTag();
		SimpleVertex* getSV();
		int getNumVertices();
		texture& getTexture();
		void setVB(ID3D11Buffer* vertexBuffer);
		void setScrollSpeed();
		bool nearOrigin(int width, int height);
		virtual void translate(float x, float y, float z, float w);
		bool collidesWith(gObject* obj);
		collider* getCol();
		void setCol(collider* col);
		XMFLOAT4 getPos(); 
		void changeState(animation::State state, animation::Direction direction);
		void changeState(animation::State state);
	protected:
		float scroll_speed;
		ID3D11Buffer* vertexBuffer;
		int numVertices;
		SimpleVertex* vertices;
		texture tex;
		string tag;
		collider* col;
		XMFLOAT4 position;
		float scale_factor;
		animation anim;
		float diff;
	private:

};

class MovingObject : public gObject {
	public:
		void translate(float x, float y, float z, float w);
};

class StationaryObject : public gObject {

};

#endif

