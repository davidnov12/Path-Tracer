/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Scene.h - trida pro reprezentaci sceny
 */

#pragma once

#include <vector>
#include "Sphere.h"
#include "ModelLoader.h"
#include <glm/glm.hpp>

#define DIFFUSE_MAT 0.1
#define SPECULAR_MAT 0.5
#define MIRROR_MAT 0.8

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

using namespace std;
using namespace glm;

class Scene {
		
public:

	// Struktura pro ulozeni primitiva (trojuhelniku)
	typedef struct primitive {
		vec4 vertex0;
		vec4 vertex1;
		vec4 vertex2;
		vec4 normal;
		vec4 color_mat;
	} Primitive;

	// Struktura pro ulozeni modelu
	typedef struct model {
		Primitive* data;
		int triangles_count;
	} Model;

	// Konstruktor
	Scene(vec3 lightPosition);
	Scene(vec3 lightPosition, vec3 lightColor);

	// Ziskani obsahu sceny
	vector<Sphere> getSpheres();
	Model getModel(int pos);
	int modelsCount();
	int spheresCount();
	vec3 getLight();
	vec3 getLightColor();

	// Pridani objektu do sceny
	void addSphere(Sphere obj);
	void addModel(ModelLoader obj, vec3 color, float material);
	void addModel(Model mod);

	void translateObject(Model md, vec3 translate);
	void scaleObject(Model md, vec3 scale);
	void rotateObject(Model md, float angle, int axis);

private:

	// Obsah sceny
	vector<Sphere> spheres;
	vector<Model> models;

	// Svetlo sceny
	vec3 lightPos;
	vec3 lightCol;
};