/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Sphere.cpp - trida pro reprezentaci koule
*/

#include "Sphere.h"

Sphere::Sphere(float radius, vec3 center, vec3 color, double probability){
	this->radius = radius;
	this->center = center;
	this->color = color;
	this->probability = probability;
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

double Sphere::getProbability(){
	return probability;
}
