/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Triangle.h - trida pro reprezentaci trojuhelniku
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Triangle {

public:

	// Konstruktor
	Triangle(vector<vec3> verts, vec3 norm);

	// Ziskani vrcholu a normaly trojuhelniku
	vector<vec3> getVertices();
	vec3 getNormal();

private:

	// Vrcholy a normala trojuhelniku
	vector<vec3> vertices;
	vec3 normal;
	
};