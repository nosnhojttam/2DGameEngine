#include "../includes/gObject.h"

/* --------------------------------------------*/
/* ----------------Game Object---------------- */
/* --------------------------------------------*/

gObject::gObject() 
{
	this->position = XMFLOAT4(0, 0, 0, 0);
}

gObject::gObject(texture* tex, string tag, SimpleVertex* vertices, int numVertices)
{
	this->tex = *tex;
	this->tag = tag;
	this->vertices = vertices;
	this->numVertices = numVertices;
	this->position = XMFLOAT4(0, 0, 0, 0);
	scale_factor = .005;
	diff = 0.0f;

	/*
	size_t tag_index = tag.find("collider");
	//this->col = collider(vertices,  numVertices);

	if (tag.find("collider") != std::string::npos)
		this->col = new collider(vertices, new XMFLOAT4(0, 0, 0, 0), numVertices);
	else
	*/
	this->col = NULL;
}

void gObject::draw(ID3D11DeviceContext* g_pImmediateContext, ID3D11Buffer* constant_buffer, long elapsed, ID3D11SamplerState* sampler)
{
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	VS_CONSTANT_BUFFER VsConstData;
	

	VsConstData.WorldMatrix.x = position.x;
	VsConstData.WorldMatrix.y = position.y;
	VsConstData.WorldMatrix.z = position.z;
	VsConstData.WorldMatrix.w = scale_factor;

	VsConstData.ViewMatrix.x = 0;
	VsConstData.ViewMatrix.y = 0;
	VsConstData.ViewMatrix.z = 0;
	VsConstData.ViewMatrix.w = 0;

	/*
	if (getTag() == "The Player") 
	{
		VsConstData.info.x = elapsed/100000;
		VsConstData.info.y = currentDirection;
	}
	*/

	if (getTag() == "The Player")
	{
		VsConstData.info.x = anim.getX(elapsed);
		VsConstData.info.y = anim.getY(elapsed);
	}


	//if (getTag() == "BG") {
	//	scale_factor = .006;
	//}


	ID3D11ShaderResourceView* srv = getTexture().getSRV();

	g_pImmediateContext->PSSetSamplers(0, 1, &sampler);
	g_pImmediateContext->UpdateSubresource(constant_buffer, 0, 0, &VsConstData, 0, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &constant_buffer);
	g_pImmediateContext->PSSetConstantBuffers(0, 1, &constant_buffer);
	g_pImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	g_pImmediateContext->PSSetShaderResources(0, 1, &srv);
	g_pImmediateContext->Draw(getNumVertices(), 0);



}

void gObject::changeState(animation::State state, animation::Direction direction)
{
	anim.setState(state);
	anim.setDirection(direction);
}

void gObject::changeState(animation::State state)
{
	anim.setState(state);
}

void gObject::setVB(ID3D11Buffer* vertexBuffer) 
{
	this->vertexBuffer = vertexBuffer;
}

string gObject::getTag()
{
	return this->tag;
}

SimpleVertex* gObject::getSV() 
{
	return vertices;
}

int gObject::getNumVertices() 
{
	return numVertices;
}

XMFLOAT4 gObject::getPos() 
{
	return position;
}

texture& gObject::getTexture()
{
	return tex;
}

bool gObject::nearOrigin(int width, int height)
{
	if (col != NULL)
	{
		if ((abs(position.x) < (width / 2)) && (abs(position.y) < (height / 2)))
		{
			return true;
		}
	}
	return false;
}

bool gObject::collidesWith(gObject* obj)
{
	if (obj->col != NULL && this->col->collision(obj->col))
	{
		return true;
	}
	return false;
}



void gObject::setScrollSpeed() 
{
	float avg = 0;
	for (int i = 0; i < numVertices; i++) 
	{
		avg += vertices[i].Pos.z;
	}
	avg /= numVertices;
	scroll_speed = avg;
}

collider* gObject::getCol() {
	return col;
}

void gObject::setCol(collider* col) {
	this->col = col;
}

void gObject::translate(float x, float y, float z, float w)
{
	// change collider
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
	this->position.w += w;
	
	// change vertices
	for (int i = 0; i < numVertices; i++) {
		vertices[i].Pos.x += x;
		vertices[i].Pos.y += y;
		vertices[i].Pos.z += z;
		if (getCol() != NULL) {
		}
	}

	

}
/* --------------------------------------------*/
/* ---------------Moving Object--------------- */
/* --------------------------------------------*/
void MovingObject::translate(float x, float y, float z, float w)
{
	// change collider
	this->position.x += x;
	this->position.y += y;
	this->position.z += z;
	this->position.w += w;

	// change vertices
	for (int i = 0; i < numVertices; i++) {
		vertices[i].Pos.x += x;
		vertices[i].Pos.y += y;
		vertices[i].Pos.z += z;
	}
}



/* --------------------------------------------*/
/* -------------Stationary Object------------- */
/* --------------------------------------------*/
