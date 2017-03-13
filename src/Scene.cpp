/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Scene.cpp - trida pro reprezentaci sceny
 */

#include "Scene.h"

Scene::Scene(vec3 lightPosition){
	lightPos = lightPosition;
}

Scene::Scene(vec3 lightPosition, vec3 lightColor){
	lightPos = lightPosition;
	lightCol = lightColor;
}

vector<Sphere> Scene::getSpheres(){
	return spheres;
}

Scene::Model Scene::getModel(int pos){
	return models.at(pos);
}

int Scene::modelsCount(){
	return models.size();
}

int Scene::spheresCount(){
	return spheres.size();
}

vec3 Scene::getLight(){
	return lightPos;
}

vec3 Scene::getLightColor(){
	return lightCol;
}

void Scene::addSphere(Sphere obj){
	spheres.push_back(obj);
}

void Scene::addModel(ModelLoader obj, vec3 color, float material){
	Model md;
	int len = obj.getData().size();
	Primitive* geometry = new Primitive[len];

	for (int a = 0; a < len; a++) {
		geometry[a].vertex0 = vec4(obj.getData().at(a).getVertices().at(0), 1.0f);
		geometry[a].vertex1 = vec4(obj.getData().at(a).getVertices().at(1), 1.0f);
		geometry[a].vertex2 = vec4(obj.getData().at(a).getVertices().at(2), 1.0f);
		geometry[a].normal = vec4(obj.getData().at(a).getNormal(), 1.0f);
		geometry[a].color_mat = vec4(color, material);
	}

	md.data = geometry;
	md.triangles_count = len;

	obj.cleanData();

	models.push_back(md);
}

void Scene::addModel(Model mod){
	models.push_back(mod);
}

void Scene::translateObject(Model md, vec3 translate){
	for(int i = 0; i < md.triangles_count; i++){
		md.data[i].vertex0 = md.data[i].vertex0 + vec4(translate, 0.0f);
		md.data[i].vertex1 = md.data[i].vertex1 + vec4(translate, 0.0f);
		md.data[i].vertex2 = md.data[i].vertex2 + vec4(translate, 0.0f);
	}
}

void Scene::scaleObject(Model md, vec3 scale){
	for (int i = 0; i < md.triangles_count; i++) {
		md.data[i].vertex0 = md.data[i].vertex0 * vec4(scale, 1.0f);
		md.data[i].vertex1 = md.data[i].vertex1 * vec4(scale, 1.0f);
		md.data[i].vertex2 = md.data[i].vertex2 * vec4(scale, 1.0f);
	}
}

void Scene::rotateObject(Model md, float angle, int axis){
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);

	for (int i = 0; i < md.triangles_count; i++) {
		if (axis == X_AXIS) {
			md.data[i].vertex0.y = md.data[i].vertex0.y * cos_angle - md.data[i].vertex0.z * sin_angle;
			md.data[i].vertex0.z = md.data[i].vertex0.y * sin_angle + md.data[i].vertex0.z * cos_angle;

			md.data[i].vertex1.y = md.data[i].vertex1.y * cos_angle - md.data[i].vertex1.z * sin_angle;
			md.data[i].vertex1.z = md.data[i].vertex1.y * sin_angle + md.data[i].vertex1.z * cos_angle;

			md.data[i].vertex2.y = md.data[i].vertex2.y * cos_angle - md.data[i].vertex2.z * sin_angle;
			md.data[i].vertex2.z = md.data[i].vertex2.y * sin_angle + md.data[i].vertex2.z * cos_angle;

			md.data[i].normal.y = md.data[i].normal.y * cos_angle - md.data[i].normal.z * sin_angle;
			md.data[i].normal.z = md.data[i].normal.y * sin_angle + md.data[i].normal.z * cos_angle;
		}
		else if (axis == Y_AXIS) {
			md.data[i].vertex0.x = md.data[i].vertex0.x * cos_angle + md.data[i].vertex0.z * sin_angle;
			md.data[i].vertex0.z = -md.data[i].vertex0.x * sin_angle + md.data[i].vertex0.z * cos_angle;

			md.data[i].vertex1.x = md.data[i].vertex1.x * cos_angle + md.data[i].vertex1.z * sin_angle;
			md.data[i].vertex1.z = -md.data[i].vertex1.x * sin_angle + md.data[i].vertex1.z * cos_angle;

			md.data[i].vertex2.x = md.data[i].vertex2.x * cos_angle + md.data[i].vertex2.z * sin_angle;
			md.data[i].vertex2.z = -md.data[i].vertex2.x * sin_angle + md.data[i].vertex2.z * cos_angle;

			md.data[i].normal.x = md.data[i].normal.x * cos_angle + md.data[i].normal.z * sin_angle;
			md.data[i].normal.z = -md.data[i].normal.x * sin_angle + md.data[i].normal.z * cos_angle;
		}
		else if (axis == Z_AXIS) {
			md.data[i].vertex0.x = md.data[i].vertex0.x * cos_angle - md.data[i].vertex0.y * sin_angle;
			md.data[i].vertex0.y = md.data[i].vertex0.x * sin_angle + md.data[i].vertex0.y * cos_angle;

			md.data[i].vertex1.x = md.data[i].vertex1.x * cos_angle - md.data[i].vertex1.y * sin_angle;
			md.data[i].vertex1.y = md.data[i].vertex1.x * sin_angle + md.data[i].vertex1.y * cos_angle;

			md.data[i].vertex2.x = md.data[i].vertex2.x * cos_angle - md.data[i].vertex2.y * sin_angle;
			md.data[i].vertex2.y = md.data[i].vertex2.x * sin_angle + md.data[i].vertex2.y * cos_angle;

			md.data[i].normal.x = md.data[i].normal.x * cos_angle - md.data[i].normal.y * sin_angle;
			md.data[i].normal.y = md.data[i].normal.x * sin_angle + md.data[i].normal.y * cos_angle;
		}

		md.data[i].normal = normalize(md.data[i].normal);
	}
}
