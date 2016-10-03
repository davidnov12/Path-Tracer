/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Scene.cpp - trida pro reprezentaci sceny
*/

#include "Scene.h"

Scene::Scene(){

}

vector<Sphere> Scene::getSpheres(){
	return spheres;
}

vector<Model> Scene::getModels(){
	return models;
}

void Scene::addSphere(Sphere obj){
	spheres.push_back(obj);
}

void Scene::addModel(Model obj){
	models.push_back(obj);
}
