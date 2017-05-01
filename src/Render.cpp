/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Render.cpp - trida rendereru
 */

#include "Render.h"



std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> uniGen(0, 1);

Render::Render(Shader* program, Scene* scene, Camera* cam, Octree* tree){
	this->program = program;
	this->scene = scene;
	camera = cam;
	octree = tree;

	float cx = -1.0, cy = 1.0;
	int i = 0;

	for (int y = 0; y < 10; y++) {

		for (int x = 0; x < 10; x++) {
			
			screenCoords[(i * 6)] = vec3(cx, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 1] = vec3(cx, cy, -0.5);
			screenCoords[(i * 6) + 2] = vec3(cx + 0.2, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 3] = vec3(cx + 0.2, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 4] = vec3(cx + 0.2, cy, -0.5);
			screenCoords[(i * 6) + 5] = vec3(cx, cy, -0.5);

			i += 1;
			cx += 0.2;
		}
		cx = -1.0;
		cy -= 0.2;
	}
	

	createBuffers();
}

void Render::updateScene(){

	if(scene->modelsCount() != 0){
	// Trojuhelniky
		GLuint geom_buff;
		glGenBuffers(1, &geom_buff);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, geom_buff);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Scene::Primitive) * scene->totalTriangles(), scene->getModel(0).data, GL_STATIC_DRAW);
	}

	if(octree != nullptr){
		if (octree->getNodesCount() != 0) {
			//cout << "BUFF\n";
			GLuint nodes_buff;
			glGenBuffers(1, &nodes_buff);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nodes_buff);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree::Node) * octree->getNodesCount(), octree->getNodes(), GL_STATIC_DRAW);
		}

		if(octree->getNodesCount() != 0){
			GLuint indices_buff;
			glGenBuffers(1, &indices_buff);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buff);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * octree->getIndicesLength(), octree->getIndices(), GL_STATIC_DRAW);
		}
	}

	GLuint sphere_buff;
	glGenBuffers(1, &sphere_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sphere_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere) * scene->getSpheres().size(), scene->getSpheres().data(), GL_STATIC_DRAW);
}

void Render::setUniforms(){

	for (int i = 0; i < 5; i++) {
		string s = "cornell_box.walls[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().walls[i]);
		s = "cornell_box.colors[" + to_string(i) + "]";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().colors[i].r, scene->getBox().colors[i].g, scene->getBox().colors[i].b);
		s = "cornell_box.normals[" + to_string(i) + "]";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().normals[i].x, scene->getBox().normals[i].y, scene->getBox().normals[i].z);
		s = "cornell_box.absorption[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().absorption[i]);
		s = "cornell_box.disperse[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().disperse[i]);
		s = "cornell_box.reflectivity[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().reflectivity[i]);
	}
	
}

void Render::cameraMove(float x, float y, float lx, float lz, bool resized, bool light_move, bool* moved, bool walls) {
	
	*moved = false;

	if (x != last_x || y != last_y || resized || light_move || walls) {
		glClear(GL_COLOR_BUFFER_BIT);
		id = 2.0;
		step = stride;
		last_x = x;
		last_y = y;
		*moved = true;
		if(walls) wallColor = !wallColor;
		if (wallColor) {
			scene->setWallColor(Scene::LEFT_WALL, vec3(0.1, 0.6, 0.9));	
			scene->setWallColor(Scene::RIGHT_WALL, vec3(0.8, 0.9, 0.1));
		}
		else {
			scene->setWallColor(Scene::LEFT_WALL, vec3(0.9, 0.2, 0.4));
			scene->setWallColor(Scene::RIGHT_WALL, vec3(0.1, 0.9, 0.1));
		}
	}
	
	glUniform1i(glGetUniformLocation(program->getProgram(), "triangles_count"), scene->totalTriangles());
	glUniform1i(glGetUniformLocation(program->getProgram(), "spheres_count"), scene->spheresCount());
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
	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Render::setStride(int newStride){
	stride = newStride;
}

float Render::getSamples(){
	return id;
}


void Render::draw(float w, float h, Window* win){
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < 30; i++) {
		string s = "randoms[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), uniGen(gen));
	}

	glUniform1f(glGetUniformLocation(program->getProgram(), "sample_ID"), id);
	glUniform1f(glGetUniformLocation(program->getProgram(), "stride"), step);

	glUniform1f(glGetUniformLocation(program->getProgram(), "width"), w);
	glUniform1f(glGetUniformLocation(program->getProgram(), "height"), h);

	id += stride;
	step += stride;

	// Nastaveni programu
	program->useProgram();

	glBindVertexArray(vao);
	
	for (int i = 0; i < 100; i++) {
		glDrawArrays(GL_TRIANGLES, i * 6, 6);
		
		//win->swapBuffers();
	}
	win->swapBuffers();
	//glDrawArrays(GL_TRIANGLES, 0, 6);
	
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