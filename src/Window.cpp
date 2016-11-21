/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Window.cpp - trida pro spravu okna
*/

#include "Window.h"

Window::Window(int width, int height, string title){
	this->width = width;
	this->height = height;
	this->title = title;
	window = createWindow();
}

void Window::closeWindow(){
	glfwTerminate();
}

GLFWwindow * Window::getWindow(){
	return window;
}

bool Window::getCloseState(){
	if(glfwWindowShouldClose(window)) return true;
	
	glfwPollEvents();
	return false;
}

void Window::swapBuffers(){
	glfwSwapBuffers(window);
}

void Window::setTitle(string title){
	glfwSetWindowTitle(window, title.c_str());
}

GLFWwindow * Window::createWindow(){
	GLFWwindow* window;
	int w, h;

	// Inicializace GLFW
	if (!glfwInit())
		return NULL;

	// Atributy okna
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 16);

	// Vytvoreni okna
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	// Povedlo se vytvorit okno ?
	if (!window) {
		glfwTerminate();
		return NULL;
	}

	// Vytvoreni kontextu pro okno
	glfwMakeContextCurrent(window);

	// Inicializace GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return NULL;
	}

	// Nastaveni orezoveho okna
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_DEPTH_TEST);

	return window;
}
