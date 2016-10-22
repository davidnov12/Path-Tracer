/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Camera.h - trida kamery
*/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Camera {

public:

	// Konstruktor
	Camera(vec3 pos, double width, double height);

	// View a Projection matice
	mat4 getViewMat();
	mat4 getProjMat();

	vec3 getPosition();

private:

	// Eulerovske uhly
	const GLfloat YAW = -90.0;
	const GLfloat PITCH = 0.0;
	
	const double FOV = 45.0;
	double width, height;

	// Smerove vektory
	vec3 position;
	vec3 up;
	vec3 right;
	vec3 forward;
	vec3 worldUp;

	// Spocitani vektoru
	void updateVectors();

};