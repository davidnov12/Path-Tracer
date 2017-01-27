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

	ModelLoader(string path);
	vector<Triangle> getData();

private:

	void loadModel(string path);

	vector<vec3> vertices;
	vector<vec2> tex_coords;
	vector<vec3> normals;

	vector<unsigned int> vertex_indices;
	vector<unsigned int> uv_indices;
	vector<unsigned int> normal_indices;

	vector<Triangle> triangles;
};