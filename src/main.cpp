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
#include "ModelLoader.h"

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 600


void thr(void* param) {
	Window win(600, 600, "Thread", true);
	Shader display;
	
	display.attachShader(Shader::VERTEX, display.loadShader("../src/shaders/display.vert"), GL_TRUE);
	display.attachShader(Shader::GEOMETRY, display.loadShader("../src/shaders/display.geom"), GL_TRUE);
	display.attachShader(Shader::FRAGMENT, display.loadShader("../src/shaders/display.frag"), GL_TRUE);
	display.compileProgram(GL_TRUE);

	while (!win.getCloseState()) {
		glfwMakeContextCurrent(win.getWindow());
		display.useProgram();
		glDrawArrays(GL_POINTS, 0, 1);
		win.swapBuffers();
	}

	win.closeWindow();
	_endthread();
}


int main() {
<<<<<<< HEAD
	Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Path Tracing", true);
=======
	Window window(WIDTH, HEIGHT, "Path Tracing", true);
>>>>>>> origin/master
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Scene cornell(vec3(0.0, 0.7, -0.2));
	Render tracer(&path, &cornell, &camera);
	FPSMeter fps;

	ModelLoader ml("../model/cube.obj");

	for (int i = 0; i < ml.getData().size(); i++){
		cout << "VERTEX 1  " << ml.getData().at(i).getVertices().at(0).x << "  " << ml.getData().at(i).getVertices().at(0).y << "  " << ml.getData().at(i).getVertices().at(0).z << endl;
		cout << "VERTEX 2  " << ml.getData().at(i).getVertices().at(1).x << "  " << ml.getData().at(i).getVertices().at(1).y << "  " << ml.getData().at(i).getVertices().at(1).z << endl;
		cout << "VERTEX 3  " << ml.getData().at(i).getVertices().at(2).x << "  " << ml.getData().at(i).getVertices().at(2).y << "  " << ml.getData().at(i).getVertices().at(2).z << endl;

		cout << "UV 1  " << ml.getData().at(i).getUVs().at(0).x << "  " << ml.getData().at(i).getUVs().at(0).y << endl;
		cout << "UV 2  " << ml.getData().at(i).getUVs().at(1).x << "  " << ml.getData().at(i).getUVs().at(1).y << endl;
		cout << "UV 3  " << ml.getData().at(i).getUVs().at(2).x << "  " << ml.getData().at(i).getUVs().at(2).y << endl;

		cout << "NORMAL 1  " << ml.getData().at(i).getNormal().x << "  " << ml.getData().at(i).getNormal().y << "  " << ml.getData().at(i).getNormal().z << endl;

	}

	// Vyplneni sceny
	Sphere s(0.037, vec3(-0.15, -0.327, -0.45), vec3(0.99f, 0.99f, 0.99f), 0.1);
	Sphere p(0.06, vec3(0.27, -0.2927, -0.12), vec3(0.99f, 0.99f, 0.99f), 0.1);
	Sphere q(0.045, vec3(-0.42, -0.3207, -0.04), vec3(0.99f, 0.99f, 0.99f), 0.7);
	Sphere r(0.037, vec3(-0.15, 0.052, -0.45), vec3(0.99f, 0.99f, 0.99f), 0.5);
	cornell.addSphere(s);
	cornell.addSphere(p);
	cornell.addSphere(q);
	cornell.addSphere(r);

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("../src/shaders/path_tracing.vert"), GL_TRUE);
	path.attachShader(Shader::FRAGMENT, path.loadShader("../src/shaders/path_tracing.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	//tracer.setTextureFramebuffer();
	tracer.setUniforms();

	//_beginthread(thr, 0, NULL);

	fps.reinit(glfwGetTime());
	
	// Hlavni smycka
	while (!window.getCloseState()) {
		
		// mereni FPS
		if (fps.checkFPS(glfwGetTime())) {
<<<<<<< HEAD
			string alg = " Backward PT";
			if (window.algorithm()) alg = " Bidirectional PT";
			string title = "Path Tracing   (" + to_string((int)fps.getFPS()) + " FPS) (" + to_string((int) tracer.getSamples()) + " samples)" + alg;
=======
			string title = "Path Tracing   (" + to_string((int)fps.getFPS()) + " FPS)  (" + to_string((int) tracer.getID()) + " samples)";
>>>>>>> origin/master
			window.setTitle(title);
			fps.reinit(glfwGetTime());
		}

		tracer.cameraMove(window.getXOffset(), window.getYOffset(), window.getLXOffset(), window.getLZOffset(), window.isResized(), window.lightMove(), window.algorithm());
		tracer.draw(window.getCurrentWidth(), window.getCurrentHeight());

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer.finish();
	window.closeWindow();

	return 0;
}
