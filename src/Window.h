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
	Window(int width, int height, string title, bool visible);
	Window(int width, int height, string title, bool visible, GLFWwindow* shared);

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

	// Pozice kamery
	float getXOffset();
	float getYOffset();

	// Pozice svetla
	float getLXOffset();
	float getLZOffset();

	// Soucasna vyska a sirka okna
	float getCurrentWidth();
	float getCurrentHeight();

	// Vlastnosti sceny a okna
	bool isResized();
	bool lightMove();

private:

	// Atributy okna
	int width, height;
	string title;
	bool visible;

	// Okno
	GLFWwindow* window;
	GLFWwindow* share;

	// Vytvoreni okna
	GLFWwindow* createWindow();
};
