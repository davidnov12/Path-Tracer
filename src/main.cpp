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

#define WIDTH 600
#define HEIGHT 600


int main() {
	Window window(WIDTH, HEIGHT, "PathTracing");
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), WIDTH, HEIGHT);
	Scene cornell(vec3(0.0, 0.49, -0.2));
	Render tracer(&path, &cornell, &camera);
	FPSMeter fps;


	// Vyplneni sceny
	Sphere s(0.037, vec3(-0.15, -0.31, -0.45), vec3(0.9, 0.9, 0.9), 0.1);
	Sphere p(0.06, vec3(0.32, -0.2777, -0.12), vec3(0.9, 0.9, 0.9), 0.0);
	cornell.addSphere(s);
	cornell.addSphere(p);

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("../src/shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	tracer.setUniforms();

	fps.reinit(glfwGetTime());
	
	// Hlavni smycka
	while (!window.getCloseState()) {
		
		// mereni FPS
		if (fps.checkFPS(glfwGetTime())) {
			string title = "Path Tracing   (" + to_string((int)fps.getFPS()) + " FPS)";
			window.setTitle(title);
			fps.reinit(glfwGetTime());
		}

		tracer.cameraMove(window.getXOffset(), window.getYOffset());
		tracer.draw();

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer.finish();
	window.closeWindow();

	return 0;
}