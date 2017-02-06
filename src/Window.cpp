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
float lx_off, lz_off;
float lastX, lastY;
float rWidth, rHeight;
bool firstMouse = true, pressed, resized, light_move;

#define CAMERA_OFFSET 0.05
#define LIGHT_OFFSET 0.07

#define CAMERA_MAX_X 0.55
#define CAMERA_MIN_X -0.55
#define CAMERA_MAX_Y 0.45
#define CAMERA_MIN_Y -0.25

#define LIGHT_MAX_X 0.45
#define LIGHT_MIN_X -0.45
#define LIGHT_MAX_Z 0.55
#define LIGHT_MIN_Z -0.35

void cameraMove(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		y_off += CAMERA_OFFSET;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		x_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		y_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		x_off += CAMERA_OFFSET;

	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		lz_off += LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		lx_off -= LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS){
		lz_off -= LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS){
		lx_off += LIGHT_OFFSET;
		light_move = true;
	}
	
	if (x_off > CAMERA_MAX_X) x_off = CAMERA_MAX_X;
	if (x_off < CAMERA_MIN_X) x_off = CAMERA_MIN_X;
	
	if (y_off > CAMERA_MAX_Y) y_off = CAMERA_MAX_Y;
	if (y_off < CAMERA_MIN_Y) y_off = -CAMERA_MIN_Y;

	if (lx_off > LIGHT_MAX_X) lx_off = LIGHT_MAX_X;
	if (lx_off < LIGHT_MIN_X) lx_off = LIGHT_MIN_X;

	if (lz_off > LIGHT_MAX_Z) lz_off = LIGHT_MAX_Z;
	if (lz_off < LIGHT_MIN_Z) lz_off = LIGHT_MIN_Z;


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

		//cout << "Event\n";
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


void resize(GLFWwindow* window, int width, int height) {
	rWidth = (float)width;
	rHeight = (float)height;

	glViewport(0, 0, width, height);

	resized = true;
	//glClear(GL_COLOR_BUFFER_BIT);

	//cout << "now" << endl;
}


Window::Window(int width, int height, string title, bool visible){
	this->width = width;
	this->height = height;
	this->title = title;
	this->visible = visible;
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

float Window::getLXOffset(){
	return lx_off;
}

float Window::getLZOffset(){
	return lz_off;
}

float Window::getCurrentWidth(){
	return rWidth;
}

float Window::getCurrentHeight(){
	return rHeight;
}

bool Window::isResized(){
	if (resized) {
		resized = !resized;
		return true;
	}
	else
		return false;
}

bool Window::lightMove(){
	if (light_move) {
		light_move = false;
		return true;
	}
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
	glfwSwapInterval(60);
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	if(!visible) glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
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
	glfwSetWindowSizeCallback(window, resize);

	// Inicializace GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return NULL;
	}

	// Nastaveni orezoveho okna
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	// Povoleni MSAA a Blendingu
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_DEPTH_TEST);

	return window;
}
