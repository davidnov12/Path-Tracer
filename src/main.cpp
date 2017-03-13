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

#define BUNNY_SCENE 1
//#define HOUSE_SCENE 1

char title[100];


int main() {
	Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Path Tracing", true);
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Scene cornell(vec3(0.0, 0.7, -0.2));
	FPSMeter fps;


	#ifdef BUNNY_SCENE
	ModelLoader ml("../model/bunny.obj", ModelLoader::VERTEX_NORMALS);
	cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
	cornell.scaleObject(cornell.getModel(0), vec3(0.1f));
	cornell.translateObject(cornell.getModel(0), vec3(0.2f, -0.62f, 0.12f));
	Octree oct(cornell.getModel(0), 15);
	Render tracer(&path, &cornell, &camera, &oct);
	
	#elif HOUSE_SCENE
	ModelLoader ml("../model/house.obj", ModelLoader::VERTEX_NORMALS);
	cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
	cornell.scaleObject(cornell.getModel(0), vec3(0.15f));
	cornell.translateObject(cornell.getModel(0), vec3(0.3f, -0.3f, 0.3f));
	cornell.rotateObject(cornell.getModel(0), -0.3f, Y_AXIS);
	Octree oct(cornell.getModel(0), 10);
	Render tracer(&path, &cornell, &camera, &oct);

	#else
	ModelLoader ml("../model/cube.obj", ModelLoader::VERTEX_NORMALS);
	cornell.addModel(ml, vec3(0.99f), DIFFUSE_MAT);
	Octree oct(cornell.getModel(0), 10);
	Render tracer(&path, &cornell, &camera, &oct);

	Sphere p(0.06, vec3(0.27, -0.2927, 0.02), vec3(0.99f, 0.99f, 0.99f), DIFFUSE_MAT);
	cornell.addSphere(p);

	#endif

	// Vyplneni sceny
	Sphere s(0.037, vec3(-0.15, -0.327, -0.45), vec3(0.99f, 0.99f, 0.99f), DIFFUSE_MAT);
	Sphere q(0.045, vec3(-0.42, -0.3207, -0.04), vec3(0.99f, 0.99f, 0.99f), MIRROR_MAT);
	Sphere r(0.037, vec3(-0.15, 0.052, -0.45), vec3(0.99f, 0.99f, 0.99f), SPECULAR_MAT);
	cornell.addSphere(s);
	cornell.addSphere(q);
	cornell.addSphere(r);
	
	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("../src/shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	tracer.setUniforms();
	tracer.updateScene();

	// Hlavni smycka
	while (!window.getCloseState()) {
		tracer.cameraMove(window.getXOffset(), window.getYOffset(), window.getLXOffset(), window.getLZOffset(), window.isResized(), window.lightMove());
		tracer.setUniforms();

		fps.startTask();

		tracer.draw(window.getCurrentWidth(), window.getCurrentHeight());

		fps.stopTask();
		fps.gainFPS();

		sprintf_s(title, "Path Tracing  Rendering Time: %.2f ms  Samples: %d  FPS: %.2f  Total: %.2f s", fps.getRenderingTime(), (int)tracer.getSamples(), fps.getFPS(), fps.getTotalTime());
		window.setTitle(title);

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer.finish();
	window.closeWindow();
	
	return 0;
}
