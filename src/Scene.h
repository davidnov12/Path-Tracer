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
#include "Model.h"

using namespace std;

class Scene {
		
public:

	// Konstruktor
	Scene(vec3 lightPosition);

	// Ziskani obsahu sceny
	vector<Sphere> getSpheres();
	vector<Model> getModels();
	vec3 getLight();

	// Pridani objektu do sceny
	void addSphere(Sphere obj);
	void addModel(Model obj);

private:

	// Obsah sceny
	vector<Sphere> spheres;
	vector<Model> models;

	// Svetlo sceny
	vec3 light;
};