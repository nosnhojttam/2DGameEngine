#ifndef TEXTURE
#define TEXTURE

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <d3d11.h>
#include <d3dx11.h>


using namespace std;

class texture
{
	public:
		texture();
		texture(string name, string url);
		string name;
		string getURL();
		string getName();
		void setSRV(ID3D11ShaderResourceView* srv);
		ID3D11ShaderResourceView* getSRV();
	protected:
		string url;
		ID3D11ShaderResourceView* srv;
};

#endif
