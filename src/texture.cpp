#include "../includes/texture.h"

texture::texture()
{

}

texture::texture(string name, string url)
{
	this->name = name;
	this->url = url;
}

string texture::getURL()
{
	return url;
}

string texture::getName()
{
	return name;
}

void texture::setSRV(ID3D11ShaderResourceView* srv) {
	this->srv = srv;
}

ID3D11ShaderResourceView* texture::getSRV() {
	return srv;
}

