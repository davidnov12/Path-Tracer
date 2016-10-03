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


int main() {
	Window window(800, 600, "PathTracing");
	Shader path;
	Scene cornell(vec3(0.0, 1.0, -0.5));
	Render tracer(&path, &cornell, vec3(0.0, 0.0, 2.0));

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	tracer.setUniforms();

	// Hlavni smycka
	while (!window.getCloseState()) {
		//glClearColor(0.3, 0.8, 0.2, 1.0);
		//glClear(GL_COLOR_BUFFER_BIT);

		tracer.draw();

		window.swapBuffers();
	}

	tracer.finish();
	window.closeWindow();

	return 0;
}