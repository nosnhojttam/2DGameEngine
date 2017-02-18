// grlLoader.cpp : Defines the entry point for the console application.
//

#include "../includes/grlLoader.h"

grlLoader::grlLoader() 
{

}

level* grlLoader::loadGRL(string file)
{
	pugi::xml_document doc;
	pugi::xml_parse_result parsed_grl = doc.load_file(file.c_str());

	cout << "file: " << file.c_str() << endl;

	level* l = new level(file);
	textureMap = new unordered_map<string, texture*>;

	
	for (pugi::xml_node object :  doc.child("objects").children("object"))
	{
		if (object.child("texture") != NULL)
		{
			string objectName, tex, numVertices, vertices;
			objectName = object.child("name").text().as_string();
			tex = object.child("texture").text().as_string();
			numVertices = object.child("numVertices").text().as_string();
			vertices = object.child("vertices").text().as_string();

			int numV = stoi(numVertices);
			stringstream ss(numV);
			float holdVertices[5];

			size_t index = 0;
			size_t oldIndex = 0;
			size_t Ndex = 0;
			int count = 0;
			int vertCount = 0;
			texture* t;
			SimpleVertex* sv = new SimpleVertex[numV];

			while ((index != string::npos))
			{

				index = vertices.find(" ", oldIndex + 1);
				Ndex = vertices.find("\n", oldIndex + 1);
				if (index < Ndex) {
					string value = vertices.substr(oldIndex, (index - oldIndex));
					oldIndex = index;
					double f = stod(value);
					if (f < .01 && f > -.01) {
						f = 0;
					}
					holdVertices[count] = (float)f;

					count++;
				}
				else {
					string value = vertices.substr(oldIndex, (Ndex - oldIndex));
					oldIndex = Ndex;

					double f = stod(value);
					if (f < .01 && f > -.01) {
						f = 0;
					}
					holdVertices[4] = (float)f;

					XMFLOAT3 pos(holdVertices[0], holdVertices[1], holdVertices[2]);
					XMFLOAT2 tex(holdVertices[3], holdVertices[4]);

					//sv->Pos = pos;
					//sv->Tex = tex;
					//vertex.push_back(sv);

					sv[vertCount].Pos = pos;
					sv[vertCount].Tex = tex;
					vertCount++;
					count = 0;
				}
				oldIndex++;
				if (vertCount > numV) {
					break;
				}
			}

			if (textureMap->count(tex) == 0) {
				t = new texture(objectName, tex);
				textureMap->emplace(tex, t);
			}
			else {
				//unordered_map<string, texture*>::const_iterator i = texMap.find(tex);
				t = textureMap->at(tex);
			}


			l->addObject(gObject(t, objectName, sv, numV));


		}
		else
		{
			string objectName, tex, numVertices, vertices;
			objectName = object.child("name").text().as_string();
			numVertices = object.child("numVertices").text().as_string();
			vertices = object.child("vertices").text().as_string();

			int numV = stoi(numVertices);
			stringstream ss(numV);
			float holdVertices[5];

			size_t index = 0;
			size_t oldIndex = 0;
			size_t Ndex = 0;
			int count = 0;
			int vertCount = 0;
			texture* t;
			SimpleVertex* sv = new SimpleVertex[numV];

			while ((index != string::npos))
			{

				index = vertices.find(" ", oldIndex + 1);
				Ndex = vertices.find("\n", oldIndex + 1);
				if (index < Ndex) {
					string value = vertices.substr(oldIndex, (index - oldIndex));
					oldIndex = index;
					double f = stod(value);
					if (f < .01 && f > -.01) {
						f = 0;
					}
					if (count < 3) {
						int x = f;
						f = (double)x;
					}
					holdVertices[count] = (float)f;

					count++;
				}
				else {
					string value = vertices.substr(oldIndex, (Ndex - oldIndex));
					oldIndex = Ndex;

					double f = stod(value);
					if (f < .01 && f > -.01) {
						f = 0;
					}
					holdVertices[4] = (float)f;

					XMFLOAT3 pos(holdVertices[0], holdVertices[1], holdVertices[2]);
					XMFLOAT2 tex(holdVertices[3], holdVertices[4]);

					//sv->Pos = pos;
					//sv->Tex = tex;
					//vertex.push_back(sv);

					sv[vertCount].Pos = pos;
					sv[vertCount].Tex = tex;
					vertCount++;
					count = 0;
				}
				oldIndex++;
				if (vertCount > numV) {
					break;
				}
			}

			collider* c = new collider(sv, new XMFLOAT4(0, 0, 0, 0), numV);

			for (int i = 0; i < l->objects.size(); i++) {
				size_t tag_index = objectName.find("_collider");
				string colliderName = objectName.substr(0, tag_index);
				//this->col = collider(vertices,  numVertices);

				if (l->objects[i].getTag().find(colliderName) != std::string::npos)
					l->objects[i].setCol(c);
			}

			//l->addObject(gObject(t, objectName, sv, numV));

		}
		
	}


	return l;
	

}

string grlLoader::getTagInner(string tag, string xml)
{
	string open = "<" + tag + ">";
	string close = "</" + tag + ">";
	unsigned openIndex = xml.find(open) + open.length();
	unsigned closeIndex = xml.find(close) - openIndex;
	//cout << xml;
	if (xml.length() > 0)
		return xml.substr(openIndex, closeIndex);
	else
		return "";
}

vector<string> grlLoader::getChildrenWithTag(string tag, string xml)
{
	vector<string> sets = vector<string>();
	string open = "<" + tag + ">";
	string close = "</" + tag + ">";
	unsigned openIndex = xml.find(open);
	unsigned closeIndex = xml.find(close);

	while (openIndex != string::npos && closeIndex != string::npos)
	{
		string child = xml.substr(openIndex, closeIndex + close.length());
		sets.push_back(child);
		xml = xml.substr(closeIndex + close.length());
		openIndex = xml.find(open) + open.length();
		closeIndex = xml.find(close);
	}
	return sets;
}

unordered_map<string, texture*>* grlLoader::getTextureMap() {
	return textureMap;
}