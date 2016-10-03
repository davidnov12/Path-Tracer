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

vector<vec3> Triangle::getVertices(){
	return vertices;
}

vec3 Triangle::getNormal(){
	return normal;
}
