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

#define WIDTH 700
#define HEIGHT 700


int main() {
	Window window(WIDTH, HEIGHT, "PathTracing");
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), WIDTH, HEIGHT);
	Scene cornell(vec3(0.0, 0.7, 0.0));
	Render tracer(&path, &cornell, &camera);

	// Vyplneni sceny
	Sphere s(0.3, vec3(0.4, 0.3, -0.5), vec3(0.7, 0.4, 0.0), 0.0);
	Sphere p(0.2, vec3(-0.4, 0.2, -0.3), vec3(0.7, 0.0, 0.5), 0.0);
	cornell.addSphere(s);
	cornell.addSphere(p);

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	tracer.setUniforms();

	// Hlavni smycka
	while (!window.getCloseState()) {
		//glClearColor(0.3, 0.8, 0.2, 1.0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		tracer.draw();

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer.finish();
	window.closeWindow();

	return 0;
}