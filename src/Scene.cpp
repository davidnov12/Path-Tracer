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
	defaultScene();
}

Scene::Scene(vec3 lightPosition, vec3 lightColor){
	lightPos = lightPosition;
	lightCol = lightColor;
	defaultScene();
}

vector<Sphere> Scene::getSpheres(){
	return spheres;
}

Scene::Model Scene::getModel(int pos){
	if (models.size() == 0) {
		Scene::Model m = { nullptr, 0 };
		return m;
	}
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

Scene::Box Scene::getBox(){
	return cornellBox;
}

void Scene::setWall(int pos, float value){
	cornellBox.walls[pos] = value;
}

void Scene::setWallColor(int pos, vec3 color){
	cornellBox.colors[pos] = color;
}

void Scene::setWallNormal(int pos, vec3 normal){
	cornellBox.normals[pos] = normal;
}

void Scene::setWallReflectivity(int pos, float value){
	cornellBox.reflectivity[pos] = value;
}

void Scene::addSphere(Sphere obj){
	spheres.push_back(obj);
}

void Scene::addModel(ModelLoader obj, vec3 color, float material){
	Model md;
	int len = obj.getData().size();
	Primitive* geometry = new Primitive[len];

	cout << "Preparing scene..." << endl;

	for (int a = 0; a < len; a++) {
		cout << a << "/" << len << endl;
		geometry[a].vertex0 = vec4(obj.getData().at(a).getVertices().at(0), 1.0f);
		geometry[a].vertex1 = vec4(obj.getData().at(a).getVertices().at(1), 1.0f);
		geometry[a].vertex2 = vec4(obj.getData().at(a).getVertices().at(2), 1.0f);
		geometry[a].normal0 = vec4(obj.getData().at(a).getNormals().at(0), 1.0f);
		geometry[a].normal1 = vec4(obj.getData().at(a).getNormals().at(1), 1.0f);
		geometry[a].normal2 = vec4(obj.getData().at(a).getNormals().at(2), 1.0f);
		geometry[a].uv0_1 = vec4(obj.getData().at(a).getUVs().at(0), obj.getData().at(a).getUVs().at(1));
		geometry[a].uv2 = vec4(obj.getData().at(a).getUVs().at(2), 1.0f, 1.0f);
		geometry[a].color_mat = vec4(color, material);
	}

	md.data = geometry;
	md.triangles_count = len;

	obj.cleanData();

	models.push_back(md);

	cout << "Scene ready." << endl;
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

			md.data[i].normal0.y = md.data[i].normal0.y * cos_angle - md.data[i].normal0.z * sin_angle;
			md.data[i].normal0.z = md.data[i].normal0.y * sin_angle + md.data[i].normal0.z * cos_angle;

			md.data[i].normal1.y = md.data[i].normal1.y * cos_angle - md.data[i].normal1.z * sin_angle;
			md.data[i].normal1.z = md.data[i].normal1.y * sin_angle + md.data[i].normal1.z * cos_angle;

			md.data[i].normal2.y = md.data[i].normal2.y * cos_angle - md.data[i].normal2.z * sin_angle;
			md.data[i].normal2.z = md.data[i].normal2.y * sin_angle + md.data[i].normal2.z * cos_angle;
		}
		else if (axis == Y_AXIS) {
			md.data[i].vertex0.x = md.data[i].vertex0.x * cos_angle + md.data[i].vertex0.z * sin_angle;
			md.data[i].vertex0.z = -md.data[i].vertex0.x * sin_angle + md.data[i].vertex0.z * cos_angle;

			md.data[i].vertex1.x = md.data[i].vertex1.x * cos_angle + md.data[i].vertex1.z * sin_angle;
			md.data[i].vertex1.z = -md.data[i].vertex1.x * sin_angle + md.data[i].vertex1.z * cos_angle;

			md.data[i].vertex2.x = md.data[i].vertex2.x * cos_angle + md.data[i].vertex2.z * sin_angle;
			md.data[i].vertex2.z = -md.data[i].vertex2.x * sin_angle + md.data[i].vertex2.z * cos_angle;

			md.data[i].normal0.x = md.data[i].normal0.x * cos_angle + md.data[i].normal0.z * sin_angle;
			md.data[i].normal0.z = -md.data[i].normal0.x * sin_angle + md.data[i].normal0.z * cos_angle;

			md.data[i].normal1.x = md.data[i].normal1.x * cos_angle + md.data[i].normal1.z * sin_angle;
			md.data[i].normal1.z = -md.data[i].normal1.x * sin_angle + md.data[i].normal1.z * cos_angle;

			md.data[i].normal2.x = md.data[i].normal2.x * cos_angle + md.data[i].normal2.z * sin_angle;
			md.data[i].normal2.z = -md.data[i].normal2.x * sin_angle + md.data[i].normal2.z * cos_angle;
		}
		else if (axis == Z_AXIS) {
			md.data[i].vertex0.x = md.data[i].vertex0.x * cos_angle - md.data[i].vertex0.y * sin_angle;
			md.data[i].vertex0.y = md.data[i].vertex0.x * sin_angle + md.data[i].vertex0.y * cos_angle;

			md.data[i].vertex1.x = md.data[i].vertex1.x * cos_angle - md.data[i].vertex1.y * sin_angle;
			md.data[i].vertex1.y = md.data[i].vertex1.x * sin_angle + md.data[i].vertex1.y * cos_angle;

			md.data[i].vertex2.x = md.data[i].vertex2.x * cos_angle - md.data[i].vertex2.y * sin_angle;
			md.data[i].vertex2.y = md.data[i].vertex2.x * sin_angle + md.data[i].vertex2.y * cos_angle;

			md.data[i].normal0.x = md.data[i].normal0.x * cos_angle - md.data[i].normal0.y * sin_angle;
			md.data[i].normal0.y = md.data[i].normal0.x * sin_angle + md.data[i].normal0.y * cos_angle;

			md.data[i].normal1.x = md.data[i].normal1.x * cos_angle - md.data[i].normal1.y * sin_angle;
			md.data[i].normal1.y = md.data[i].normal1.x * sin_angle + md.data[i].normal1.y * cos_angle;

			md.data[i].normal2.x = md.data[i].normal2.x * cos_angle - md.data[i].normal2.y * sin_angle;
			md.data[i].normal2.y = md.data[i].normal2.x * sin_angle + md.data[i].normal2.y * cos_angle;
		}

		md.data[i].normal0 = normalize(md.data[i].normal0);
		md.data[i].normal1 = normalize(md.data[i].normal1);
		md.data[i].normal2 = normalize(md.data[i].normal2);
	}
}

void Scene::defaultScene(){
	cornellBox.walls[0] = DEFAULT_DOWN;
	cornellBox.colors[0] = vec3(1);
	cornellBox.normals[0] = vec3(0, 1, 0);
	cornellBox.reflectivity[0] = DIFFUSE_MAT;

	cornellBox.walls[1] = DEFAULT_UP;
	cornellBox.colors[1] = vec3(1);
	cornellBox.normals[1] = vec3(0, -1, 0);
	cornellBox.reflectivity[1] = DIFFUSE_MAT;

	cornellBox.walls[2] = DEFAULT_LEFT;
	cornellBox.colors[2] = vec3(1);
	cornellBox.normals[2] = vec3(1, 0, 0);
	cornellBox.reflectivity[2] = DIFFUSE_MAT;

	cornellBox.walls[3] = DEFAULT_RIGHT;
	cornellBox.colors[3] = vec3(1);
	cornellBox.normals[3] = vec3(-1, 0, 0);
	cornellBox.reflectivity[3] = DIFFUSE_MAT;

	cornellBox.walls[4] = DEFAULT_FRONT;
	cornellBox.colors[4] = vec3(1);
	cornellBox.normals[4] = vec3(0, 0, 1);
	cornellBox.reflectivity[4] = DIFFUSE_MAT;
}
