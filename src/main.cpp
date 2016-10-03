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


int main() {
	Window window(800, 600, "PathTracing");
	Shader path;

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("shaders/path_tracing.vert"), GL_FALSE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("shaders/path_tracing.frag"), GL_FALSE);
	path.compileProgram(GL_FALSE);

	// Hlavni smycka
	while (!window.getCloseState()) {
		glClearColor(0.3, 0.8, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		path.useProgram();

		window.swapBuffers();
	}

	window.closeWindow();

	return 0;
}