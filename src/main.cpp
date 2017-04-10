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
#include <process.h>
#include "Octree.h"

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 600

#define SPHERE_SCENE 1
#define BUNNY_SCENE 2
#define DEER_SCENE 3

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

int main() {
	
	print_intro();
	int sc;
	cin >> sc;

	Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Path Tracing", true);
	FPSMeter fps;
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Scene cornell(vec3(0.0, 0.5, -0.2));
	Render* tracer;
	

	if(sc == SPHERE_SCENE) {
		Sphere r(0.032, vec3(-0.15, 0.07, -0.45), vec3(0.99f, 0.99f, 0.99f), SPECULAR_MAT);
		Sphere s(0.032, vec3(-0.15, -0.28, -0.45), vec3(0.99f, 0.99f, 0.99f), DIFFUSE_MAT);
		Sphere p(0.052, vec3(0.27, -0.22, 0.02), vec3(0.99f, 0.99f, 0.99f), DIFFUSE_MAT);
		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), vec3(0.99f, 0.99f, 0.99f), MIRROR_MAT);
		cornell.addSphere(s);
		cornell.addSphere(p);
		cornell.addSphere(q);
		cornell.addSphere(r);
	}
	else if (sc == BUNNY_SCENE) {
		ModelLoader ml("../model/bunny.obj", ModelLoader::VERTEX_NORMALS);
		cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
		cornell.scaleObject(cornell.getModel(0), vec3(0.13f));
		cornell.translateObject(cornell.getModel(0), vec3(0.2f, -0.58f, -0.05f));
		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), vec3(0.99f, 0.99f, 0.99f), MIRROR_MAT);
		cornell.addSphere(q);
	}
	else if(sc == DEER_SCENE) {	
		ModelLoader ml("../model/deer.obj", ModelLoader::VERTEX_UVS_NORMALS);
		cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
		cornell.scaleObject(cornell.getModel(0), vec3(0.022f));
		cornell.rotateObject(cornell.getModel(0), -1.07f, Y_AXIS);
		cornell.translateObject(cornell.getModel(0), vec3(0.2f, -0.4f, 0.1f));
		Sphere q(0.042, vec3(-0.42, -0.25, -0.04), vec3(0.99f, 0.99f, 0.99f), MIRROR_MAT);
		cornell.addSphere(q);		
	}
	else if (sc == 4) {
		ModelLoader ml("../model/Space.obj", ModelLoader::VERTEX_NORMALS);
		cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
		cornell.scaleObject(cornell.getModel(0), vec3(0.17f));
	}
	else {
		return 1;
	}

	Octree oct(cornell.getModel(0), 15);
	Render tr(&path, &cornell, &camera, &oct);
	tracer = &tr;
	
	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("../src/shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	tracer->setUniforms();
	tracer->updateScene();

	//oct->print_tree();


	// Hlavni smycka
	while (!window.getCloseState()) {
		tracer->cameraMove(window.getXOffset(), window.getYOffset(), window.getLXOffset(), window.getLZOffset(), window.isResized(), window.lightMove(), &moved);
		tracer->setUniforms();

		if (moved) fps.refreshTotalTime();

		fps.startTask();

		tracer->draw(window.getCurrentWidth(), window.getCurrentHeight());

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