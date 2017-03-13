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

	// Nastaveni souradnic prumetny
	screenCoords[0] = vec3(-1.0, -1.0, -0.5);
	screenCoords[1] = vec3(-1.0, 1.0, -0.5);
	screenCoords[2] = vec3(1.0, -1.0, -0.5);
	screenCoords[3] = vec3(1.0, -1.0, -0.5);
	screenCoords[4] = vec3(1.0, 1.0, -0.5);
	screenCoords[5] = vec3(-1.0, 1.0, -0.5);

	createBuffers();
/*
	glGenFramebuffers(1, &frame);
	glBindFramebuffer(GL_FRAMEBUFFER, frame);

	glGenTextures(1, &screen);
	glBindTexture(GL_TEXTURE_2D, screen);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 600, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screen, 0);
	GLenum db[] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, db);
*/
	/*display.attachShader(Shader::VERTEX, display.loadShader("../src/shaders/display.vert"), GL_TRUE);
	display.attachShader(Shader::GEOMETRY, display.loadShader("../src/shaders/display.geom"), GL_TRUE);
	display.attachShader(Shader::FRAGMENT, display.loadShader("../src/shaders/display.frag"), GL_TRUE);
	display.compileProgram(GL_TRUE);*/

	//setTextureFramebuffer();
}

void Render::updateScene(){

	// Trojuhelniky
	GLuint geom_buff;
	glGenBuffers(1, &geom_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, geom_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Scene::Primitive) * scene->getModel(0).triangles_count, scene->getModel(0).data, GL_STATIC_DRAW);

	GLuint nodes_buff;
	glGenBuffers(1, &nodes_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nodes_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree::Node) * octree->getNodesCount(), octree->getNodes(), GL_STATIC_DRAW);

	GLuint indices_buff;
	glGenBuffers(1, &indices_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * octree->getIndicesLength(), octree->getIndices(), GL_STATIC_DRAW);
/*
	Octree::Node datas[2];
	datas[0].childs[0] = 1.0;
	datas[0].childs[1] = 0.0;
	datas[0].childs[2] = 1.0;
	datas[0].childs[3] = 0.0;
	datas[0].childs[4] = 1.0;
	datas[0].childs[5] = 0.0;
	datas[0].childs[6] = 0.0;
	datas[0].childs[7] = 1.0;
	datas[0].index = 1.0;
	datas[0].count = 0.0;
	datas[0].leaf = 1.0;
	datas[0].start = vec4(0.0, 0.0, 0.9, 0.0);
	datas[0].end = vec4(1.0, 1.0, 0.0, 0.0);

	datas[1].childs[0] = 0.0;
	datas[1].childs[1] = 1.0;
	datas[1].childs[2] = 0.0;
	datas[1].childs[3] = 1.0;
	datas[1].childs[4] = 0.0;
	datas[1].childs[5] = 1.0;
	datas[1].childs[6] = 1.0;
	datas[1].childs[7] = 0.0;
	datas[1].index = 0.0;
	datas[1].count = 0.0;
	datas[1].leaf = 0.0;
	datas[1].start = vec4(0.0, 0.5, 0.0, 0.0);
	datas[1].end = vec4(0.0, 0.0, 1.0, 0.0);

	Octree::Node* n = octree->getNodes();
	cout << "NODE " << n[1].start.x << endl;

	// Uzly oktaloveho stromu
	GLuint nodes_buff;
	glGenBuffers(1, &nodes_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nodes_buff);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree::Node) * octree->getNodesCount(), octree->getNodes(), GL_STATIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree::Node) * 2, datas, GL_STATIC_DRAW);

	int pole[4] = {1, 0, 1, 0};

	// Indexy trojuhelniku
	GLuint indices_buff;
	glGenBuffers(1, &indices_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buff);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * octree->getIndicesLength(), octree->getIndices(), GL_STATIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 4, pole, GL_STATIC_DRAW);*/
	
	/*int cnt = 0, offset = 0;

	for (int i = 0; i < scene->modelsCount(); i++)
		cnt += scene->getModel(i).triangles_count;

	char* objects = new char[cnt * sizeof(Scene::Primitive)];

	for (int i = 0; i < scene->modelsCount(); i++) {
		memcpy_s(objects + offset, scene->getModel(i).triangles_count * sizeof(Scene::Primitive), scene->getModel(i).data, scene->getModel(i).triangles_count * sizeof(Scene::Primitive));
		offset += sizeof(Scene::Primitive) * scene->getModel(i).triangles_count;
	}

	GLuint geom_buff;
	glGenBuffers(1, &geom_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, geom_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Scene::Primitive) * cnt, objects, GL_STATIC_DRAW);

	delete[] objects;*/
}

void Render::setUniforms(){

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
	
}

void Render::cameraMove(float x, float y, float lx, float lz, bool resized, bool light_move) {
	
	if (x != last_x || y != last_y || resized || light_move) {
		glClear(GL_COLOR_BUFFER_BIT);
		id = 2.0;
		step = stride;
		last_x = x;
		last_y = y;
	}
	glUniform1i(glGetUniformLocation(program->getProgram(), "triangles_count"), scene->getModel(0).triangles_count);
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


void Render::draw(float w, float h){
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int i = 0; i < 100; i++) {
		string s = "randoms[" + to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), uniGen(gen));
	}

	glUniform1f(glGetUniformLocation(program->getProgram(), "sample_ID"), id);
	glUniform1f(glGetUniformLocation(program->getProgram(), "stride"), step);

	glUniform1f(glGetUniformLocation(program->getProgram(), "width"), w);
	glUniform1f(glGetUniformLocation(program->getProgram(), "height"), h);

	id += stride;
	step += stride;

	if ((id - stride) >= 5000.0) {
		id = 0.0;
		step = stride;
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Nastaveni programu
	program->useProgram();

	//setUniforms();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, screen);
	//glUniform1i(glGetUniformLocation(program->getProgram(), "frame"), 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, frame);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	/*
	display.useProgram();
	
	glDrawArrays(GL_POINTS, 0, 1);*/

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