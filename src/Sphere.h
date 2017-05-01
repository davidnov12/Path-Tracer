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

	typedef struct material {
		float absorption;
		float disperse;
		float reflection;
		vec3 color;
	} Material;

	// Konstruktor
	Sphere(float radius, vec3 center, Material material);

	// Funkce pro ziskani vlastnosti koule
	float getRadius();
	vec3 getCenter();
	vec3 getColor();
	//float getReflectivity();

private:

	// Parametry koule
	vec3 center;
	float radius;
	vec3 color;
	float reflection;
	float disper;
	float absorp;
	float gap[2];
};