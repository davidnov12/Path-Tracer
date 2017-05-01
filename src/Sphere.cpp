/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Sphere.cpp - trida pro reprezentaci koule
*/

#include "Sphere.h"


Sphere::Sphere(float radius, vec3 center, Material material){
	this->radius = radius;
	this->center = center;
	this->color = material.color;
	this->reflection = material.reflection;
	this->absorp = material.absorption;
	this->disper = material.disperse;
}

float Sphere::getRadius(){
	return radius;
}

vec3 Sphere::getCenter(){
	return center;
}

vec3 Sphere::getColor(){
	return color;
}
