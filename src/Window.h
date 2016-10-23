/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Window.h - trida pro spravu okna
*/

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

class Window {

public:

	// Konstruktor
	Window(int width, int height, string title);

	// Uzavreni okna
	void closeWindow();

	// Ukazatel na objekt okna
	GLFWwindow* getWindow();

	// Ma byt okno uzavreno
	bool getCloseState();

	// Prohozeni framebufferu
	void swapBuffers();

	// Nastaveni titulku okna
	void setTitle(string title);

private:

	// Atributy okna
	int width, height;
	string title;

	// Okno
	GLFWwindow* window;

	// Vytvoreni okna
	GLFWwindow* createWindow();
};
