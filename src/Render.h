/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Render.h - trida rendereru
*/

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <random>
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"

using namespace glm;

class Render{

public:

	// Konstruktor
	Render(Shader* program, Scene* scene, Camera* cam);

	// Nastaveni uniformnich promennych v shaderech
	void setUniforms();

	// Pohyb kamery
	void cameraMove(float x, float y);

	// Kresleni sceny
	void draw();

	// Uvolneni dat
	void finish();

private:

	// Souradnice prumetny
	vec3 screenCoords[6];

	// VAO, VBO
	GLuint vao, vbo;

	// Shader program pro vykreslovani
	Shader* program;

	// Scena k vykresleni
	Scene* scene;

	// kamery
	Camera* camera;

	// Vytvoreni VAO a VBO
	void createBuffers();
};