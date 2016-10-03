/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Shader.cpp - trida pro praci se shader programem
*/

#include "Shader.h"


	Shader::Shader() {

		// Inicializace shaderu a programu
		for (int i = 0; i < SHADER_CNT; i++)
			shaders[i] = 0;

		program = 0;
	}


	const GLchar* Shader::loadShader(GLchar* path) {

		const GLchar* code;
		ifstream shaderFile;
		shaderFile.exceptions(ifstream::badbit);

		try {

			// Otevreni souboru
			shaderFile.open(path);
			stringstream stream;

			// Precteni a uzavreni souboru
			stream << shaderFile.rdbuf();
			shaderFile.close();

			// Obsah souboru
			code = stream.str().c_str();

			return (const GLchar*)code;
		}

		catch (ifstream::failure e) {

			// Chyba pri cteni souboru
			cout << "READ ERROR" << endl;
		}

		return NULL;
	}


	void Shader::attachShader(GLuint type, const GLchar* code, GLboolean verbose) {

		GLuint shader, kind;

		// Urceni typu shaderu
		switch (type) {
		case VERTEX:
			kind = GL_VERTEX_SHADER;
			break;

		case GEOMETRY:
			kind = GL_GEOMETRY_SHADER;
			break;

		case TESS_CONTROL:
			kind = GL_TESS_CONTROL_SHADER;
			break;

		case TESS_EVAL:
			kind = GL_TESS_EVALUATION_SHADER;
			break;

		case FRAGMENT:
			kind = GL_FRAGMENT_SHADER;
			break;

		case COMPUTE:
			kind = GL_COMPUTE_SHADER;
			break;
		}

		// Kompilace shaderu
		shader = glCreateShader(kind);
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		shaders[type] = shader;

		// Vystup kompilace
		if (verbose) {
			GLint success;
			GLchar info[LOG_SIZE];
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, LOG_SIZE, NULL, info);
				cout << info << endl;
			}
		}

	}


	GLuint Shader::compileProgram(GLboolean verbose) {

		GLint success;
		int result = 0;
		program = glCreateProgram();

		// Pripojeni shaderu
		for (int i = 0; i < SHADER_CNT; i++) {
			if (shaders[i])
				glAttachShader(program, shaders[i]);
		}

		// Sestaveni programu
		glLinkProgram(program);

		// Vystup ze sestaveni
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			result = 1;
			if (verbose) {
				GLchar info[LOG_SIZE];
				glGetProgramInfoLog(program, LOG_SIZE, NULL, info);
				cout << info << endl;
			}
		}

		// Odstraneni shaderu
		for (int i = 0; i < SHADER_CNT; i++)
			glDeleteShader(shaders[i]);

		return result;
	}


	void Shader::useProgram() {
		glUseProgram(program);
	}


	GLuint Shader::getProgram() {
		return program;
	}
