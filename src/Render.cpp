/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Render.cpp - trida rendereru
*/

#include "Render.h"


Render::Render(Shader* program, Scene* scene, vec3 cameraPosition){
	this->program = program;
	this->scene = scene;
	camera = cameraPosition;

	// Nastaveni souradnic prumetny
	screenCoords[0] = vec3(-1.0, -1.0, 1.0);
	screenCoords[1] = vec3(-1.0, 1.0, 1.0);
	screenCoords[2] = vec3(1.0, -1.0, 1.0);
	screenCoords[3] = vec3(1.0, -1.0, 1.0);
	screenCoords[4] = vec3(1.0, 1.0, 1.0);
	screenCoords[5] = vec3(-1.0, 1.0, 1.0);

	createBuffers();
}

void Render::setUniforms(){
	// Svetlo sceny
	glUniform3f(glGetUniformLocation((*program).getProgram(), "light_pos"), (*scene).getLight().x, (*scene).getLight().y, (*scene).getLight().z);

	// Kamera
	glUniform3f(glGetUniformLocation((*program).getProgram(), "view_pos"), camera.x, camera.y, camera.z);

	// Objekty sceny
}

void Render::draw(){
	// Nastaveni programu
	(*program).useProgram();
	
	//cout << program.getProgram() << endl;
	//cout << vao << endl;

	// Kresleni
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Render::finish(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Render::createBuffers(){
	// VAO - vertex array object
	glGenBuffers(1, &vao);
	glBindVertexArray(vao);

	// VBO - vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	// Atribut pro shadery - souradnice prumetny
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Odvazani VAO
	glBindVertexArray(0);
}
