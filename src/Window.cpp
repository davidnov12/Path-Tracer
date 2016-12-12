/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Window.cpp - trida pro spravu okna
*/

#include "Window.h"

float x_off, y_off;
float lastX, lastY;
bool firstMouse = true, pressed;

#define CAMERA_OFFSET 0.05

void cameraMove(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		y_off += CAMERA_OFFSET;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		x_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		y_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		x_off += CAMERA_OFFSET;
	
	if (x_off > 0.55) x_off = 0.55;
	if (x_off < -0.55) x_off = -0.55;
	
	if (y_off > 0.45) y_off = 0.45;
	if (y_off < -0.25) y_off = -0.25;
}

// Pohyb mysi
void mouse(GLFWwindow* window, double xpos, double ypos) {

	if (pressed) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		x_off -= 0.002*(xpos - lastX);
		y_off -= 0.002*(lastY - ypos);

		lastX = xpos;
		lastY = ypos;

	}

	lastX = xpos;
	lastY = ypos;

	if (x_off > 0.55) x_off = 0.55;
	if (x_off < -0.55) x_off = -0.55;

	if (y_off > 0.45) y_off = 0.45;
	if (y_off < -0.25) y_off = -0.25;
}

// Stisknuti mysi
void press(GLFWwindow* window, int button, int action, int mods) {

	if (action == GLFW_PRESS) pressed = true;
	else pressed = false;
}


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

float Window::getXOffset() {
	return x_off;
}

float Window::getYOffset() {
	return y_off;
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

	glfwSetKeyCallback(window, cameraMove);
	glfwSetMouseButtonCallback(window, press);
	glfwSetCursorPosCallback(window, mouse);

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
