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
	void cameraMove(float x, float y, float lx, float lz, bool resized, bool light_move, bool algorithm);

	// Priprava kreslici textury
	void setTextureFramebuffer();

	// Pocet vykreslenych snimku
	float getID();

	// Zmena kroku vykreslovani vzorku
	void setStride(int newStride);

	// Pocet vykreslenych vzorku
	float getSamples();

	// Kresleni sceny
	void draw(float w, float h);

	// Uvolneni dat
	void finish();

private:

	// Souradnice prumetny
	vec3 screenCoords[6];

	// VAO, VBO
	GLuint vao, vbo;
	GLuint screen, frame;

	// Vzorky pro PT
	GLfloat id = 1.0, stride = 1.0, step = 1.0;
	GLfloat last_x = 0, last_y = 0;

	// Shader program pro vykreslovani
	Shader* program;
	Shader display;

	// Scena k vykresleni
	Scene* scene;

	// Kamera
	Camera* camera;

	// Vytvoreni VAO a VBO
	void createBuffers();
};
