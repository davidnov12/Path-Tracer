/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Shader.h - trida pro praci se shader programem
*/

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>


#define SHADER_CNT 6	// Pocet shaderu
#define LOG_SIZE 512	// Velikost logu

using namespace std;

class Shader {

public:

	// Konstruktor
	Shader();

	// Nacteni shader programu
	string loadShader(GLchar* path);

	// Pripojeni a zkompilovani shaderu
	void attachShader(GLuint type, const string code, GLboolean verbose);

	// Sestaveni programu
	GLuint compileProgram(GLboolean verbose);
	
	// Pouziti programu
	void useProgram();

	// Ziskani reference na program
	GLuint getProgram();

	// Typy shaderu
	enum shaderTypes {
		VERTEX,
		GEOMETRY,
		TESS_CONTROL,
		TESS_EVAL,
		FRAGMENT,
		COMPUTE
	};


private:

	// Atributy
	GLuint shaders[SHADER_CNT];
	GLuint program;

};