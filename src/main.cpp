/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * main.cpp - spousteci modul aplikace
 */


#include "Window.h"
#include "Shader.h"
#include "Render.h"
#include "Camera.h"
#include "FPSMeter.h"
//#include <process.h>
#include "Octree.h"

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 600

#define SPHERE_SCENE 1
#define BUNNY_SCENE 2
#define DEER_SCENE 3
#define TELESCOPE_SCENE 4

#define LIST 1
#define OCTREE 2

char title[100];
bool moved;

void print_intro() {
	cout << "Path Tracing app" << endl;
	cout << "Choose scene:" << endl;
	cout << "1 - spheres in Cornell box" << endl;
	cout << "2 - bunny (336 faces) in Cornell box" << endl;
	cout << "3 - deer (1 503 faces) in Cornell box" << endl;
	cout << "4 - NASA telescope (13 812 faces) in Cornell box" << endl;
}

void choose_ds() {
	cout << "Data structure:" << endl;
	cout << "1 - list" << endl;
	cout << "2 - octree" << endl;
}

int main() {
	
	print_intro();
	int sc;
	int data_st;
	cin >> sc;

	Scene cornell(vec3(0.0, 0.5, -0.2));
	
	// Volba sceny
	if(sc == SPHERE_SCENE) {
		Sphere r(0.032, vec3(-0.15, 0.07, -0.45), Sphere::Material{ 1.0f, 1.0f, 0.4f, vec3(0.99f) });
		Sphere s(0.032, vec3(-0.15, -0.28, -0.45), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		Sphere p(0.052, vec3(0.27, -0.22, 0.02), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), Sphere::Material{ 1.0f, 1.0f, 0.0f, vec3(0.99f) });
		cornell.addSphere(s);
		cornell.addSphere(p);
		cornell.addSphere(q);
		cornell.addSphere(r);

		data_st = LIST;
	}
	else if (sc == BUNNY_SCENE) {
		
		ModelLoader ml("../model/bunny.obj", ModelLoader::VERTEX_NORMALS, ModelLoader::Material{0.9f, 0.0f, 0.0f, vec3(0.99f)});
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.12f));
		cornell.translateObject(0, vec3(0.2f, -0.58f, -0.05f));
		//cout << cornell.totalTriangles() << endl;

		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addSphere(q);

		choose_ds();
		cin >> data_st;
	}
	else if(sc == DEER_SCENE) {	
		ModelLoader ml("../model/deer.obj", ModelLoader::VERTEX_UVS_NORMALS, ModelLoader::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.022f));
		cornell.rotateObject(0, -1.07f, Y_AXIS);
		cornell.translateObject(0, vec3(0.2f, -0.4f, 0.1f));
		Sphere q(0.042, vec3(-0.42, -0.25, -0.04), Sphere::Material{ 1.0f, 1.0f, 0.0f, vec3(0.99f) });
		cornell.addSphere(q);	

		choose_ds();
		cin >> data_st;
	}
	else if (sc == TELESCOPE_SCENE) {
		ModelLoader ml("../model/space.obj", ModelLoader::VERTEX_NORMALS, ModelLoader::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.11f));
		cornell.translateObject(0, vec3(0.0f, -0.23f, 0.11f));

		choose_ds();
		cin >> data_st;
	}
	else {
		return 1;
	}

	Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Path Tracing", true);
	FPSMeter fps;
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Render* tracer;
	Octree oct;

	// Priprava datove struktury
	if(data_st == LIST){
		Render tr(&path, &cornell, &camera, nullptr);
		tracer = &tr;
	}
	else if(data_st == OCTREE) {
		oct.build(cornell.getModels().data(), cornell.modelsCount(), cornell.totalTriangles(), 15);
		//oct.print_tree();
		Render tr(&path, &cornell, &camera, &oct);
		tracer = &tr;
	}
	else {
		return 1;
	}

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("../src/shaders/path_tracing.vert"), GL_TRUE);
	if(data_st == LIST) path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing_list.frag"), GL_TRUE);
	else if(data_st == OCTREE) path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing_octree.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	//cornell.setWallColor(Scene::LEFT_WALL, vec3(0.6, 0.1, 0.8));
	//cornell.setWallColor(Scene::LEFT_WALL, vec3(0.9, 0.2, 0.4));
	cornell.setWallColor(Scene::LEFT_WALL, vec3(0.1, 0.6, 0.9));
	//cornell.setWallColor(Scene::RIGHT_WALL, vec3(0.1, 0.9, 0.1));
	cornell.setWallColor(Scene::RIGHT_WALL, vec3(0.8, 0.9, 0.1));
	tracer->setUniforms();
	tracer->updateScene();

	
	// Hlavni smycka
	while (!window.getCloseState()) {
		tracer->cameraMove(window.getXOffset(), window.getYOffset(), window.getLXOffset(), window.getLZOffset(), window.isResized(), window.lightMove(), &moved, window.wallsColor());
		tracer->setUniforms();

		if (moved) fps.refreshTotalTime();

		fps.startTask();

		tracer->draw(window.getCurrentWidth(), window.getCurrentHeight(), &window);

		fps.stopTask();
		fps.gainFPS();

		sprintf_s(title, "Path Tracing  Rendering Time: %.2f ms  Samples: %d  FPS: %.2f  Total: %.2f s", fps.getRenderingTime(), (int)tracer->getSamples(), fps.getFPS(), fps.getTotalTime());
		window.setTitle(title);

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer->finish();
	window.closeWindow();
	
	return 0;
}