/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * ModelLoader.h - trida pro nacteni modelu
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include "Triangle.h"

using namespace std;
using namespace glm;

class ModelLoader {

public:

	enum Modes {
		VERTEX_UVS_NORMALS,
		VERTEX_UVS,
		VERTEX_NORMALS,
		VERTEX
	};

	typedef struct material {
		float absorption;
		float disperse;
		float reflection;
		vec3 color;
	} Material;

	ModelLoader(string path, Modes mode, Material material);
	vector<Triangle> getData();

	void cleanData();

	vector<vec4> va;
	vector<vec4> ua;
	vector<vec4> na;
	vector<vec4> ca;

private:

	void loadModel(string path, int mode);

	vector<vec3> vertices;
	vector<vec2> tex_coords;
	vector<vec3> normals;

	vector<unsigned int> vertex_indices;
	vector<unsigned int> uv_indices;
	vector<unsigned int> normal_indices;

	vector<Triangle> triangles;

	Modes load_mode;
	Material mat;
};