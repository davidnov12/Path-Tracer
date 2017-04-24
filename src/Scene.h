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

#define DEFAULT_DOWN -0.45
#define DEFAULT_UP 0.55
#define DEFAULT_LEFT -0.7
#define DEFAULT_RIGHT 0.7
#define DEFAULT_FRONT -0.9


using namespace std;
using namespace glm;

class Scene {
		
public:

	// Steny sceny
	typedef enum wall {
		DOWN_WALL,
		UP_WALL,
		LEFT_WALL,
		RIGHT_WALL,
		FRONT_WALL
	};

	// Struktura pro reprezentaci cornell boxu
	typedef struct box {
		float walls[5];
		vec3 normals[5];
		vec3 colors[5];
		float reflectivity[5];
	} Box;

	// Struktura pro ulozeni primitiva (trojuhelniku)
	typedef struct primitive {
		vec4 vertex0;
		vec4 vertex1;
		vec4 vertex2;
		vec4 normal0;
		vec4 normal1;
		vec4 normal2;
		vec4 uv0_1;
		vec4 uv2;
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

	Box getBox();
	void setWall(int pos, float value);
	void setWallColor(int pos, vec3 color);
	void setWallNormal(int pos, vec3 normal);
	void setWallReflectivity(int pos, float value);

	// Pridani objektu do sceny
	void addSphere(Sphere obj);
	void addModel(ModelLoader obj, vec3 color, float material);
	void addModel(Model mod);

	void translateObject(Model md, vec3 translate);
	void scaleObject(Model md, vec3 scale);
	void rotateObject(Model md, float angle, int axis);

private:

	void defaultScene();

	// Obsah sceny
	vector<Sphere> spheres;
	vector<Model> models;

	// Svetlo sceny
	vec3 lightPos;
	vec3 lightCol;

	Box cornellBox;
};