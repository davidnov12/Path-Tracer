/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Sphere.h - trida pro reprezentaci koule
*/

#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Sphere {

public:

	// Konstruktor
	Sphere(float radius, vec3 center, vec3 color, double probability);

	// Funkce pro ziskani vlastnosti koule
	float getRadius();
	vec3 getCenter();
	vec3 getColor();
	double getProbability();

private:

	// Parametry koule
	float radius;
	vec3 center;
	vec3 color;
	double probability;
};