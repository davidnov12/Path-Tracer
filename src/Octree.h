/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.h - trida pro praci s oktalovym stromem
 */

#pragma once

#include "Scene.h"

#define LEFT -0.7
#define RIGHT 0.7
#define UP 0.7
#define DOWN -0.52
#define FRONT -0.9

class Octree {

public:

	typedef struct node {
		int childs[8];
		int index;
		int count;
		int leaf;
		vec4 start;
		vec4 end;
	} Node;

	Octree(Scene::Model md, int primitivesPerList);

private:

	void initOctree();
	void buildOctree(Scene::Model md, int primitivesPerList);
	bool isInBox(vec3 point, vec3 startBox, vec3 endBox);
	void makeChilds(Node n);
	bool edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd, float length);
	bool uniqueIndices(int inserted, int nodeIndex);

	Node* root;
	vector<Node> nodes;
	vector<vector<int>> tmp_indices;
	vector<int> indices;
	int nodes_count;

	float t;

};