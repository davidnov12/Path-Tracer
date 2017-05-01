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
	Triangle(vector<vec3> verts, vector<vec3> norm);
	Triangle(vector<vec3> verts, vector<vec2> uvs, vector<vec3> norm);

	void append(vector<vec3> verts, vector<vec2> uvs, vector<vec3> normal);

	// Ziskani vrcholu a normaly trojuhelniku
	vector<vec3> getVertices();
	vector<vec2> getUVs();
	vector<vec3> getNormals();

private:

	// Vrcholy a normala trojuhelniku
	vector<vec3> vertices;
	vector<vec2> tex_coords;
	vector<vec3> normal;
	
};