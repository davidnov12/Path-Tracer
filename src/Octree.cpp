/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.cpp - trida pro praci s oktalovym stromem
 */


#include "Octree.h"

Octree::Octree(){

}

void Octree::build(Scene::Model md, int totalTriangles, int primitivesPerList){
//if (md.data.) return;
//mod = md;
int lf = 0;
triangles_count = totalTriangles;

cout << "Octree building..." << endl;

initOctree();
buildOctree(md, primitivesPerList);
linkOctree();

cout << "Octree builded." << endl;
cout << "Total: " << nodes.size() << " nodes" << endl;
}


void Octree::build(Scene::Model* md, int modelCount, int totalTriangles, int primitivesPerList) {

	triangles_count = totalTriangles;
	cout << "Octree building..." << endl;

	for (int i = 0; i < modelCount; i++) {
		//if (md.data().data.empty()) return;
		mod = md[i];
		int lf = 0;

		initOctree();
		buildOctree(mod, primitivesPerList);
		linkOctree();

	}

	cout << "Octree builded." << endl;
	cout << "Total: " << nodes.size() << " nodes" << endl;
}

int * Octree::getIndices() {
	return indices.data();
}

Octree::Node * Octree::getNodes() {
	return nodes.data();
}

int Octree::getIndicesLength() {
	return indices.size();
}

int Octree::getNodesCount() {
	return nodes.size();
}

void Octree::initOctree() {
	Node n;
	n.index = n.count = 0;
	n.leaf = 1;
	n.start = vec4(LEFT, DOWN, BACK, 1.0f);
	n.end = vec4(RIGHT, UP, FRONT, 1.0f);
	for (int i = 0; i < 8; i++)
		n.childs[i] = -1;

	nodes.push_back(n);

	vector<int> c;
	tmp_indices.push_back(c);

	nodes_count = 1;
}

int cts = 0;
vector<int> used;

void Octree::buildOctree(Scene::Model md, int primitivesPerList) {

	vec4 edge, v1, v2;
	float len;

	int cn = 0;
	
	// Vsechny uzly Octree
	for (int i = 0; i < nodes.size(); i++) {
		
		// Listove uzly
		if (nodes.at(i).leaf) {

			// Vsechny trojuhelniky
			for (int j = 0; j < triangles_count; j++) {
			
				if (trinagleAABBIntersection(vec3(md.data[j].vertex0), vec3(md.data[j].vertex1), vec3(md.data[j].vertex2), vec3(nodes.at(i).start), vec3(nodes.at(i).end))) {
					used.push_back(j);
					tmp_indices.at(i).push_back(j);
				}
			}

			}

			// Expanze uzlu
			if (tmp_indices.at(i).size() > primitivesPerList && nodes.size() < MAX_NODES) {
				//cout << "EXP " << i << endl;
				makeChilds(i);
			}

		}

}

bool Octree::isInBox(vec3 point, vec3 startBox, vec3 endBox){
	if (point.x >= startBox.x && point.y >= startBox.y && point.z <= startBox.z) 
		if(point.x <= endBox.x && point.y <= endBox.y && point.z >= endBox.z)
			return true;
	
	return false;
}

void Octree::makeChilds(int index){
	
	vec4 half = (nodes.at(index).start + nodes.at(index).end) / 2.0f;

	//cout << "HALF" << half.x << "  " << half.y << "  " << half.z << endl;

	Node news[8];

	for(int a = 0; a < 8; a++){
		for(int i = 0; i < 8; i++)
			news[a].childs[i] = -1;
		
		news[a].index = 0;
		news[a].count = 0;
		news[a].leaf = 1;
	}

	vec4 start = nodes.at(index).start;
	vec4 end = nodes.at(index).end;

	news[0].start = vec4(start.x, start.y, start.z, 1.0f);
	news[0].end = vec4(half.x, half.y, half.z, 1.0f);

	news[1].start = vec4(start.x, start.y, half.z, 1.0f);
	news[1].end = vec4(half.x, half.y, end.z, 1.0f);

	news[2].start = vec4(start.x, half.y, start.z, 1.0f);
	news[2].end = vec4(half.x, end.y, half.z, 1.0f);

	news[3].start = vec4(start.x, half.y, half.z, 1.0f);
	news[3].end = vec4(half.x, end.y, end.z, 1.0f);

	news[4].start = vec4(half.x, start.y, start.z, 1.0f);
	news[4].end = vec4(end.x, half.y, half.z, 1.0f);

	news[5].start = vec4(half.x, start.y, half.z, 1.0f);
	news[5].end = vec4(end.x, half.y, end.z, 1.0f);

	news[6].start = vec4(half.x, half.y, start.z, 1.0f);
	news[6].end = vec4(end.x, end.y, half.z, 1.0f);

	news[7].start = vec4(half.x, half.y, half.z, 1.0f);
	news[7].end = vec4(end.x, end.y, end.z, 1.0f);

	for (int a = 0; a < 8; a++) {
		vector<int> v;
		tmp_indices.push_back(v);
		nodes.push_back(news[a]);
		nodes.at(index).childs[a] = nodes_count + a;
	}
	nodes.at(index).leaf = 0;

	nodes_count = nodes.size();
}

bool Octree::edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd){
		
	bool res = false;
	float closest = 5.0;
	float planeHeight;
	t = 5.2;
	vec3 point;

	vec3 direction = vec3(edge);
	vec3 position = vec3(origin);

	// Dolni stena
	//if (direction.y < 0.0) {
		planeHeight = boxStart.y;
		t = (planeHeight - position.y) / direction.y;
		point = position + (t * direction);
		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.x >= boxStart.x && point.x <= boxEnd.x) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	// Horni stena
	//if (direction.y > 0.0) {
		planeHeight = boxEnd.y;
		t = (planeHeight - position.y) / direction.y;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.x >= boxStart.x && point.x <= boxEnd.x) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	// Leva stena
	//if (direction.x < 0.0) {
		planeHeight = boxStart.x;
		t = (planeHeight - position.x) / direction.x;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.y >= boxStart.y && point.y <= boxEnd.y) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	// Prava stena
	//if (direction.x > 0.0) {
		planeHeight = boxEnd.x;
		t = (planeHeight - position.x) / direction.x;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.y >= boxStart.y && point.y <= boxEnd.y) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	// Predni stena
	//if (direction.z < 0.0) {
		planeHeight = boxEnd.z;
		t = (planeHeight - position.z) / direction.z;
		point = position + (t * direction);

		if (point.x >= boxStart.x && point.x <= boxEnd.x && point.y >= boxStart.y && point.y <= boxEnd.y) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	// Zadni stena
	//if (direction.z > 0.0) {
		planeHeight = boxStart.z;
		t = (planeHeight - position.z) / direction.z;
		point = position + (t * direction);

		if (point.x >= boxStart.x && point.x <= boxEnd.x && point.y >= boxStart.y && point.y <= boxEnd.y) {
			if (t < closest && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	t = closest;
	
	return res;

	
}


bool Octree::trinagleAABBIntersection(vec3 v0, vec3 v1, vec3 v2, vec3 boxStart, vec3 boxEnd){
	

	if (isInBox(v0, boxStart, boxEnd)) return true;

	if (isInBox(v1, boxStart, boxEnd)) return true;

	if (isInBox(v2, boxStart, boxEnd)) return true;

	float max_x = max(v0.x, max(v1.x, v2.x));
	float min_x = min(v0.x, min(v1.x, v2.x));
	float max_y = max(v0.y, max(v1.y, v2.y));
	float min_y = min(v0.y, min(v1.y, v2.y));
	float max_z = max(v0.z, max(v1.z, v2.z));
	float min_z = min(v0.z, min(v1.z, v2.z));


	// DOWN Y
	if (v0.y >= boxStart.y || v1.y >= boxStart.y || v2.y >= boxStart.y) {
		if (v0.y <= boxStart.y || v1.y <= boxStart.y || v2.y <= boxStart.y) {

			//cout << "DOWN IN" << endl;

			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					cts++;
					return true;
				}
			}

		}
	}

	// UP Y
	if (v0.y >= boxEnd.y || v1.y >= boxEnd.y || v2.y >= boxEnd.y) {
		if (v0.y <= boxEnd.y || v1.y <= boxEnd.y || v2.y <= boxEnd.y) {
			//cout << "UP IN" << endl;
			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					cts++;
					return true;
				}
			}

		}
	}

	// LEFT X
	if (v0.x >= boxStart.x || v1.x >= boxStart.x || v2.x >= boxStart.x) {
		if (v0.x <= boxStart.x || v1.x <= boxStart.x || v2.x <= boxStart.x) {
			//cout << "LEFT IN" << endl;
			if (boxStart.y <= max_y && boxEnd.y >= min_y) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					cts++;
					return true;
				}
			}

		}
	}

	// RIGHT X
	if (v0.x >= boxEnd.x || v1.x >= boxEnd.x || v2.x >= boxEnd.x) {
		if (v0.x <= boxEnd.x || v1.x <= boxEnd.x || v2.x <= boxEnd.x) {
			//cout << "RIGHT IN" << endl;
			if (boxStart.y <= max_y && boxEnd.y >= min_y) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					cts++;
					return true;
				}
			}

		}
	}

	// FRONT Z
	if (v0.z >= boxStart.z || v1.z >= boxStart.z || v2.z >= boxStart.z) {
		if (v0.z <= boxStart.z || v1.z <= boxStart.z || v2.z <= boxStart.z) {
			//cout << "FRONT IN" << endl;
			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.y <= max_y && boxEnd.y >= min_y) {
					cts++;
					return true;
				}
			}

		}
	}

	// BACK Z
	if (v0.z >= boxEnd.z || v1.z >= boxEnd.z || v2.z >= boxEnd.z) {
		if (v0.z <= boxEnd.z || v1.z <= boxEnd.z || v2.z <= boxEnd.z) {
			//cout << "BACK IN" << endl;
			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.y <= max_y && boxEnd.y >= min_y) {
					cts++;
					return true;
				}
			}

		}
	}

	return false;
}

bool Octree::uniqueIndices(int inserted, int nodeIndex){
	
	for (int i = 0; i < tmp_indices.at(nodeIndex).size(); i++)
		if (tmp_indices.at(nodeIndex).at(i) == inserted)
			return true;

	return false;
}

void Octree::linkOctree(){
	int current_ptr = 0;

	for (int i = 0; i < nodes.size(); i++) {
		
		if (nodes.at(i).leaf) {
			nodes.at(i).index = current_ptr;
			nodes.at(i).count = tmp_indices.at(i).size();
			current_ptr += tmp_indices.at(i).size();
			
			for(int j = 0; j < tmp_indices.at(i).size(); j++){
				indices.push_back(tmp_indices.at(i).at(j));
			}
		}
	}
}


void Octree::print_tree() {

	for (int i = 0; i < nodes.size(); i++) {

		if(!nodes.at(i).leaf){
		
			cout << i << endl;
			cout << "////////////////////////" << endl;

			for (int j = 0; j < 8; j++) {
				cout << nodes.at(i).childs[j] << "  ";
			}
			cout << endl;
		}
	}
}


void Octree::printVec(vec3 v) {
	cout << v.x << " " << v.y << " " << v.z << endl;
}