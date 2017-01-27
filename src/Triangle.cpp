/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Triangle.cpp - trida pro reprezentaci trojuhelniku
 */

#include "Triangle.h"

Triangle::Triangle(vector<vec3> verts, vec3 norm){
	vertices = verts;
	normal = norm;
}

Triangle::Triangle(vector<vec3> verts, vector<vec2> uvs, vec3 norm){
	vertices = verts;
	tex_coords = uvs;
	normal = norm;
}

vector<vec3> Triangle::getVertices(){
	return vertices;
}

vector<vec2> Triangle::getUVs(){
	return tex_coords;
}

vec3 Triangle::getNormal(){
	return normal;
}
