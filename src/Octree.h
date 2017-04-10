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
#define DOWN -0.65
#define FRONT -0.9
#define BACK 0.9

<<<<<<< HEAD
#define MAX_NODES 2.5 * md.triangles_count

using namespace std;
=======
#define MAX_NODES 300
>>>>>>> origin/master

class Octree {

public:

	typedef struct node {
		int childs[8];
		int index;
		int count;
		int leaf;
		int gap;
		vec4 start;
		vec4 end;
	} Node;

	Octree(Scene::Model md, int primitivesPerList);
	int* getIndices();
	Node* getNodes();
	int getIndicesLength();
	int getNodesCount();

//private:

	void initOctree();
	void buildOctree(Scene::Model md, int primitivesPerList);
	bool isInBox(vec3 point, vec3 startBox, vec3 endBox);
	void makeChilds(int index);
	bool edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd);
	//bool triangleBoxIntersection(vec3 v0, vec3 v1, vec3 v2, vec3 boxStart, vec3 boxEnd);
	bool trinagleAABBIntersection(vec3 v0, vec3 v1, vec3 v2, vec3 boxStart, vec3 boxEnd);
	bool uniqueIndices(int inserted, int nodeIndex);
	void linkOctree();

	void print_tree();
	void printVec(vec3 v);

	Node* root;
	vector<Node> nodes;
	vector<vector<int>> tmp_indices;
	vector<int> indices;
	int nodes_count;

	Scene::Model mod;
	float t = 5.2;
	float tx0, ty0, tz0, tx1, ty1, tz1;
<<<<<<< HEAD
};
=======
};
>>>>>>> origin/master
