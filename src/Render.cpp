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
	setTextureFramebuffer();
}

void Render::setUniforms(){
	// Svetlo sceny
	//glUniform3f(glGetUniformLocation(program->getProgram(), "light_pos"), scene->getLight().x, scene->getLight().y, scene->getLight().z);

	// Kamera
	//glUniform3f(glGetUniformLocation(program->getProgram(), "view_pos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);

	// Inverzni VP matice
	//mat4 view = lookAt(vec3(0), vec3(0, 0, 1), vec3(0, 1, 0));
	//mat4 inv_viewproj = inverse(camera->getProjMat() * camera->getViewMat());
	//glUniformMatrix4fv(glGetUniformLocation(program->getProgram(), "inv_viewproj"), 1, GL_FALSE, value_ptr(inv_viewproj));
	
	//glUniform1d(glGetUniformLocation(program->getProgram(), "sample_ID"), 0);
	//glUniform1d(glGetUniformLocation(program->getProgram(), "stride"), 1);

	// Nahodna cisla pro Path Tracing
	

	// Objekty sceny
	for (int i = 0; i < scene->getSpheres().size(); i++) {
		string s = "spheres[" + to_string(i) + "].center";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getSpheres().at(i).getCenter().x, scene->getSpheres().at(i).getCenter().y, scene->getSpheres().at(i).getCenter().z);
		s = "spheres[" + to_string(i) + "].color";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getSpheres().at(i).getColor().r, scene->getSpheres().at(i).getColor().g, scene->getSpheres().at(i).getColor().b);
		s = "spheres[" + to_string(i) + "].radius";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getSpheres().at(i).getRadius());
		s = "spheres[" + to_string(i) + "].reflectivity";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getSpheres().at(i).getProbability());
	}
	/*glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[1].center"), scene->getSpheres().at(1).getCenter().x, scene->getSpheres().at(1).getCenter().y, scene->getSpheres().at(1).getCenter().z);
	glUniform3f(glGetUniformLocation(program->getProgram(), "spheres[1].color"), scene->getSpheres().at(1).getColor().r, scene->getSpheres().at(1).getColor().g, scene->getSpheres().at(1).getColor().b);
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[1].radius"), scene->getSpheres().at(1).getRadius());
	glUniform1f(glGetUniformLocation(program->getProgram(), "spheres[1].reflectivity"), scene->getSpheres().at(1).getProbability());
	*/
}

void Render::cameraMove(float x, float y, float lx, float lz) {
	cout << id << " " << step << endl;
	if (x != last_x || y != last_y) {
		glClear(GL_COLOR_BUFFER_BIT);
		id = 1.0;
		step = stride;
		last_x = x;
		last_y = y;
	}
	glUniform3f(glGetUniformLocation(program->getProgram(), "view_pos"), (camera->getPosition().x) + x, (camera->getPosition().y) + y, camera->getPosition().z);
	glUniform3f(glGetUniformLocation(program->getProgram(), "light_pos"), (scene->getLight().x) + lx, scene->getLight().y, (scene->getLight().z) + lz);
}

void Render::setTextureFramebuffer(){
	
	// Vykreslovaci shader
	display.attachShader(Shader::VERTEX, display.loadShader("../src/shaders/display.vert"), GL_TRUE);
	display.attachShader(Shader::GEOMETRY, display.loadShader("../src/shaders/display.geom"), GL_TRUE);
	display.attachShader(Shader::FRAGMENT, display.loadShader("../src/shaders/display.frag"), GL_TRUE);
	display.compileProgram(GL_TRUE);

	// Textura pro vykreslovani
	glGenTextures(1, &screen);
	glBindTexture(GL_TEXTURE_2D, screen);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 600, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Framebuffer pro vykreslovani
	glGenFramebuffersEXT(1, &frame);
	glBindFramebufferEXT(GL_FRAMEBUFFER, frame);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, screen, 0);

	/*GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 600, 600); // Use a single renderbuffer object for both a depth AND stencil buffer.
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);*/

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Render::setStride(int newStride){
	stride = newStride;
}



void Render::draw(){

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> uniGen(0, 1);
	for (int i = 0; i < 100; i++) {
		string s = "randoms[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), uniGen(gen));
	}

	glUniform1f(glGetUniformLocation(program->getProgram(), "sample_ID"), id);
	glUniform1f(glGetUniformLocation(program->getProgram(), "stride"), step);

	id += stride;
	step += stride;

	if ((id - stride) >= 700.0) {
		id = 0.0;
		step = stride;
		glClear(GL_COLOR_BUFFER_BIT);
		system("pause");
	}

	// Nastaveni programu
	program->useProgram();

	setUniforms();
	
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	

}

void Render::finish(){
	glDeleteFramebuffers(1, &frame);
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