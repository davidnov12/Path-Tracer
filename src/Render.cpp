/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Render.cpp - trida rendereru
*/

#include "Render.h"


Render::Render(Shader* program, Scene* scene, Camera* cam){
	this->program = program;
	this->scene = scene;
	camera = cam;

	// Nastaveni souradnic prumetny
	screenCoords[0] = vec3(-1.0, -1.0, -1.0);
	screenCoords[1] = vec3(-1.0, 1.0, -1.0);
	screenCoords[2] = vec3(1.0, -1.0, -1.0);
	screenCoords[3] = vec3(1.0, -1.0, -1.0);
	screenCoords[4] = vec3(1.0, 1.0, -1.0);
	screenCoords[5] = vec3(-1.0, 1.0, -1.0);

	createBuffers();
}

void Render::setUniforms(){
	// Svetlo sceny
	glUniform3f(glGetUniformLocation(program->getProgram(), "light_pos"), scene->getLight().x, scene->getLight().y, scene->getLight().z);

	// Kamera
	glUniform3f(glGetUniformLocation(program->getProgram(), "view_pos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	// Inverzni VP matice
	//mat4 view = lookAt(vec3(0), vec3(0, 0, 1), vec3(0, 1, 0));
	mat4 inv_viewproj = inverse(camera->getProjMat() * camera->getViewMat());
	glUniformMatrix4fv(glGetUniformLocation(program->getProgram(), "inv_viewproj"), 1, GL_FALSE, value_ptr(inv_viewproj));
	
	// Objekty sceny
	glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[0].center"), scene->getSpheres().at(0).getCenter().x, scene->getSpheres().at(0).getCenter().y, scene->getSpheres().at(0).getCenter().z);
	glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[0].color"), scene->getSpheres().at(0).getColor().r, scene->getSpheres().at(0).getColor().g, scene->getSpheres().at(0).getColor().b);
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[0].radius"), scene->getSpheres().at(0).getRadius());
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[0].reflectivity"), scene->getSpheres().at(0).getProbability());

	glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[1].center"), scene->getSpheres().at(1).getCenter().x, scene->getSpheres().at(1).getCenter().y, scene->getSpheres().at(1).getCenter().z);
	glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[1].color"), scene->getSpheres().at(1).getColor().r, scene->getSpheres().at(1).getColor().g, scene->getSpheres().at(1).getColor().b);
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[1].radius"), scene->getSpheres().at(1).getRadius());
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[1].reflectivity"), scene->getSpheres().at(1).getProbability());
}

void Render::draw(){
	// Nastaveni programu
	program->useProgram();

	setUniforms();

	// Kresleni
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//glDrawArrays(GL_POINT, 0, 1);
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