/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.cpp - trida pro praci s oktalovym stromem
 */


#include "Octree.h"

<<<<<<< HEAD
Octree::Octree(Scene::Model md, int primitivesPerList){
	mod = md;
	initOctree();
	buildOctree(md, primitivesPerList);
	linkOctree();
}

int * Octree::getIndices(){
	return indices.data();
}

Octree::Node * Octree::getNodes(){
	return nodes.data();
}

int Octree::getIndicesLength(){
	return indices.size();
}

int Octree::getNodesCount(){
	return nodes.size();
=======
Octree::Octree(Scene::Model md, int primitivesPerList) {
	initOctree();
	buildOctree(md, primitivesPerList);
	linkOctree();
	//makeChilds(0);
	//makeChilds(2);
>>>>>>> origin/master
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

<<<<<<< HEAD
void Octree::buildOctree(Scene::Model md, int primitivesPerList){
	
	vec4 edge, v1, v2;
	float len;

	int cn = 0;

	// Vsechny uzly Octree
	for (int j = 0; j < nodes.size(); j++) {

		//cout << j << "   " << nodes.size() << endl;

		// Listove uzly
		if (nodes.at(j).leaf) {

=======
void Octree::buildOctree(Scene::Model md, int primitivesPerList) {
	//makeChilds(0);
	vec4 edge, v1, v2;
	float len;

	// Vsechny uzly Octree
	for (int j = 0; j < nodes.size(); j++) {

		//cout << j << "   " << nodes.size() << endl;

		// Listove uzly
		if (nodes.at(j).leaf) {

>>>>>>> origin/master
			// Vsechny trojuhelniky v modelu
			for (int i = 0; i < md.triangles_count; i++) {

				// Vsechny 3 hrany trojuhelniku
<<<<<<< HEAD
				for(int k = 0; k < 3; k++){
					
=======
				for (int k = 0; k < 3; k++) {

>>>>>>> origin/master
					// Hrany trojuhelniku
					if (k == 0) {
						len = length(vec3(md.data[i].vertex1) - vec3(md.data[i].vertex0));
						edge = vec4(normalize(vec3(md.data[i].vertex1) - vec3(md.data[i].vertex0)), 1.0f);
						v1 = md.data[i].vertex0;
						v2 = md.data[i].vertex1;
					}
					else if (k == 1) {
						len = length(vec3(md.data[i].vertex2) - vec3(md.data[i].vertex1));
						edge = vec4(normalize(vec3(md.data[i].vertex2) - vec3(md.data[i].vertex1)), 1.0f);
						v1 = md.data[i].vertex1;
						v2 = md.data[i].vertex2;
					}
					else if (k == 2) {
						len = length(vec3(md.data[i].vertex0) - vec3(md.data[i].vertex2));
						edge = vec4(normalize(vec3(md.data[i].vertex0) - vec3(md.data[i].vertex2)), 1.0f);
						v1 = md.data[i].vertex2;
						v2 = md.data[i].vertex0;
					}

					// Vrchol A trojuhelniku lezi v bounding boxu
					if (isInBox(vec3(v2), vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
<<<<<<< HEAD
						
=======

>>>>>>> origin/master
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Vrchol B trojuhelniku lezi v bounding boxu
					if (isInBox(vec3(v1), vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
<<<<<<< HEAD
						
=======

>>>>>>> origin/master
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Hrana trojuhelniku prochazi pres bounding box
					if (edgeBoxIntersection(v1, edge, vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
<<<<<<< HEAD
						//cout << "EDG " << t << endl;
						if (!uniqueIndices(i, j) && t <= len && t >= 0.0) {
=======

						if (!uniqueIndices(i, j) && t < len && t >= 0.0) {
>>>>>>> origin/master
							tmp_indices.at(j).push_back(i);
						}
					}
				}
			}

			// Rozdeleni uzlu
<<<<<<< HEAD
			if (tmp_indices.at(j).size() > primitivesPerList && nodes.size() < MAX_NODES){
				
=======
			if (tmp_indices.at(j).size() > primitivesPerList && nodes.size() < MAX_NODES) {
>>>>>>> origin/master
				makeChilds(j);
			}
		}
	}

}

<<<<<<< HEAD
bool Octree::isInBox(vec3 point, vec3 startBox, vec3 endBox){
	if (point.x >= startBox.x && point.y >= startBox.y && point.z <= startBox.z) 
		if(point.x <= endBox.x && point.y <= endBox.y && point.z >= endBox.z)
=======
bool Octree::isInBox(vec3 point, vec3 startBox, vec3 endBox) {
	if (point.x >= startBox.x && point.y >= startBox.y && point.z <= startBox.z)
		if (point.x <= endBox.x && point.y <= endBox.y && point.z >= endBox.z)
>>>>>>> origin/master
			return true;

	return false;
}

<<<<<<< HEAD
void Octree::makeChilds(int index){
	
	vec4 half = (nodes.at(index).start + nodes.at(index).end) / 2.0f;

	//cout << "HALF" << half.x << "  " << half.y << "  " << half.z << endl;
=======
void Octree::makeChilds(int index) {

	vec4 half = (nodes.at(index).start + nodes.at(index).end) / 2.0f;
>>>>>>> origin/master

	Node news[8];

	for (int a = 0; a < 8; a++) {
		for (int i = 0; i < 8; i++)
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

<<<<<<< HEAD
bool Octree::edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd){
		
=======
bool Octree::edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd) {

>>>>>>> origin/master
	bool res = false;
	float closest = 5.0;
	float planeHeight;
	t = 5.2;
	vec3 point;
<<<<<<< HEAD

	vec3 direction = vec3(edge);
	vec3 position = vec3(origin);

	// Dolni stena
	//if (direction.y < 0.0) {
=======

	vec3 direction = vec3(edge);
	vec3 position = vec3(origin);

	// Dolni stena
	if (direction.y < 0.0) {
>>>>>>> origin/master
		planeHeight = boxStart.y;
		t = (planeHeight - position.y) / direction.y;
		point = position + (t * direction);
		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.x >= boxStart.x && point.x <= boxEnd.x) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
	//}

	// Horni stena
<<<<<<< HEAD
	//if (direction.y > 0.0) {
=======
	if (direction.y > 0.0) {
>>>>>>> origin/master
		planeHeight = boxEnd.y;
		t = (planeHeight - position.y) / direction.y;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.x >= boxStart.x && point.x <= boxEnd.x) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
	//}

	// Leva stena
<<<<<<< HEAD
	//if (direction.x < 0.0) {
=======
	if (direction.x < 0.0) {
>>>>>>> origin/master
		planeHeight = boxStart.x;
		t = (planeHeight - position.x) / direction.x;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.y >= boxStart.y && point.y <= boxEnd.y) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
	//}

	// Prava stena
<<<<<<< HEAD
	//if (direction.x > 0.0) {
=======
	if (direction.x > 0.0) {
>>>>>>> origin/master
		planeHeight = boxEnd.x;
		t = (planeHeight - position.x) / direction.x;
		point = position + (t * direction);

		if (point.z >= boxEnd.z && point.z <= boxStart.z && point.y >= boxStart.y && point.y <= boxEnd.y) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
	//}

	// Predni stena
<<<<<<< HEAD
	//if (direction.z < 0.0) {
=======
	if (direction.z < 0.0) {
>>>>>>> origin/master
		planeHeight = boxEnd.z;
		t = (planeHeight - position.z) / direction.z;
		point = position + (t * direction);

		if (point.x >= boxStart.x && point.x <= boxEnd.x && point.y >= boxStart.y && point.y <= boxEnd.y) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
	//}

	// Zadni stena
<<<<<<< HEAD
	//if (direction.z > 0.0) {
=======
	if (direction.z > 0.0) {
>>>>>>> origin/master
		planeHeight = boxStart.z;
		t = (planeHeight - position.z) / direction.z;
		point = position + (t * direction);

		if (point.x >= boxStart.x && point.x <= boxEnd.x && point.y >= boxStart.y && point.y <= boxEnd.y) {
<<<<<<< HEAD
			if (abs(t) < abs(closest) && t > 0.0) {
=======
			if (abs(t) < abs(closest)) {
>>>>>>> origin/master
				closest = t;
				res = true;
			}
		}
<<<<<<< HEAD
	//}
=======
	}
>>>>>>> origin/master

	t = closest;
	
	return res;

	
}

bool Octree::uniqueIndices(int inserted, int nodeIndex) {

	for (int i = 0; i < tmp_indices.at(nodeIndex).size(); i++)
		if (tmp_indices.at(nodeIndex).at(i) == inserted)
			return true;

	return false;
}

<<<<<<< HEAD
void Octree::linkOctree(){
	int current_prt = 0;

	for (int i = 0; i < nodes.size(); i++) {
		
=======
void Octree::linkOctree() {
	int current_prt = 0;

	for (int i = 0; i < nodes.size(); i++) {

>>>>>>> origin/master
		if (nodes.at(i).leaf) {
			nodes.at(i).index = current_prt;
			nodes.at(i).count = tmp_indices.at(i).size();
			current_prt += tmp_indices.at(i).size();
<<<<<<< HEAD
			
			for(int j = 0; j < tmp_indices.at(i).size(); j++){
=======

			for (int j = 0; j < tmp_indices.at(i).size(); j++) {
>>>>>>> origin/master
				indices.push_back(tmp_indices.at(i).at(j));
			}
		}
	}
}


<<<<<<< HEAD
void Octree::print_tree() {

	for (int i = 0; i < nodes.size(); i++) {

		if(!nodes.at(i).leaf){
		
=======
void Octree::printTree() {

	cout << "TREE STRUCTURE:\n\n";
	for (int i = 0; i < nodes.size(); i++) {

		if (!nodes.at(i).leaf) {

>>>>>>> origin/master
			cout << i << endl;
			cout << "////////////////////////" << endl;

			for (int j = 0; j < 8; j++) {
				cout << nodes.at(i).childs[j] << "  ";
			}
<<<<<<< HEAD
			cout << endl;
		}
	}
=======
			cout << endl << endl;
		}
	}

	cout << "--------------\n";
>>>>>>> origin/master
}


void Octree::printVec(vec3 v) {
	cout << v.x << " " << v.y << " " << v.z << endl;
<<<<<<< HEAD
}
=======
}

void Octree::printUsesLeafs(){
	cout << "NOT EMPTY NODES:\n";
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes.at(i).count != 0) cout << i << "  " << nodes.at(i).count << endl;
	}
	cout << "---------------\n";
}






// OCTREE TRAVERSING

int result;

unsigned int a;

int Octree::first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm) {
	unsigned char first = 0;

	if (tx0 > ty0) {
		//YZ
		if (tx0 > tz0) { 
			if (tym < tx0) first |= 2;
			if (tzm < tx0) first |= 4;
			return (int)first;
		}
	}
	else {
		// XZ
		if (ty0 > tz0) { 
			if (txm < ty0) first |= 1;
			if (tzm < ty0) first |= 4;
			return (int)first;
		}
	}
	// XY
	if (txm < tz0) first |= 1;
	if (tym < tz0) first |= 2;
	return (int)first;
}


int Octree::new_node(double txm, int x, double tym, int y, double tzm, int z) {
	if (txm < tym) {
		if (txm < tzm) return x;
	}
	else {
		if (tym < tzm) return y;
	}
	return z;
}


void Octree::octreeTraversal(vec3 origin, vec3 dir) {
	a = 0;

	if (dir.x < 0.0f) {
		origin.x = (RIGHT + LEFT) - origin.x;
		dir.x *= -1.0f;
		a |= 4;
	}

	if (dir.y < 0.0f) {
		origin.y = (UP + DOWN) - origin.y;
		dir.y *= -1.0f;
		a |= 2;
	}

	if (dir.z > 0.0f) {
		origin.z = (FRONT - BACK) - origin.z;
		dir.z *= -1.0f;
		a |= 1;
	}


	cout << "ORIGIN: " << origin.x << " " << origin.y << " " << origin.z << endl;
	cout << "DIRECTION: " << dir.x << " " << dir.y << " " << dir.z << endl;

	tx0 = (LEFT - origin.x) / dir.x;
	tx1 = (RIGHT - origin.x) / dir.x;
	ty0 = (DOWN - origin.y) / dir.y;
	ty1 = (UP - origin.y) / dir.y;
	tz0 = (BACK - origin.z) / dir.z;
	tz1 = (FRONT - origin.z) / dir.z;

	if(max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1)))
		iterative(0);

	else
		cout << "end" << endl;

	cout << "RESULT: " << result << endl;
}


typedef struct octcontext {
	float tx0, ty0, tz0;
	float tx1, ty1, tz1;
	unsigned int current, node_id;
	bool init;
} OctreeContext;

OctreeContext context;
OctreeContext states[20];

void Octree::iterative(int node) {
	int current, sp = 0;
	float txm, tym, tzm;
	bool init = false;
	int cycles = 0;
	cout << "OCTREE TRAVERSAL:\n";

	do {
		cycles++;
		cout << "ENTERING NODE: " << node << endl;
		//cout << tx1 << " " << ty1 << " " << tz1 << endl;

		if (tx1 < 0 || ty1 < 0 || tz1 < 0) {
			sp -= 1;
			if (sp < 0) {
				result = -1;
				break;
			}
			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current;
			node = states[sp].node_id;
			init = states[sp].init;
		}

		if (nodes.at(node).leaf) {
			cout << "LEAF NODE: " << node << endl;

			if (nodes.at(node).count != 0) {
				result = node;
				return;
			}

			sp -= 1;
			if (sp < 0) {
				result = -1;
				break;
			}

			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current;
			node = states[sp].node_id;
			init = states[sp].init;
		}

		txm = 0.5f * (tx0 + tx1);
		tym = 0.5f * (ty0 + ty1);
		tzm = 0.5f * (tz0 + tz1);

		if (!init) {
			init = !init;
			current = first_node(tx0, ty0, tz0, txm, tym, tzm);
		}

		context.tx0 = tx0;
		context.ty0 = ty0;
		context.tz0 = tz0;
		context.tx1 = tx1;
		context.ty1 = ty1;
		context.tz1 = tz1;
		context.node_id = node;
		context.init = true;
		init = false;

		//cout << "CURRENT: " << current << endl;

		switch (current) {	
		case 0:
			node = nodes.at(node).childs[a];
			context.current = new_node(txm, 4, tym, 2, tzm, 1);
			tx1 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 1:
			node = nodes.at(node).childs[1 ^ a];
			context.current = new_node(txm, 5, tym, 3, tz1, 8);
			tx1 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 2:
			node = nodes.at(node).childs[2 ^ a];
			context.current = new_node(txm, 6, ty1, 8, tzm, 3);
			tx1 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 3:
			node = nodes.at(node).childs[3 ^ a];
			context.current = new_node(txm, 7, ty1, 8, tz1, 8);
			tx1 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 4:
			node = nodes.at(node).childs[4 ^ a];
			context.current = new_node(tx1, 8, tym, 6, tzm, 5);
			tx0 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 5:
			node = nodes.at(node).childs[5 ^ a];
			context.current = new_node(tx1, 8, tym, 7, tz1, 8);
			tx0 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 6:
			node = nodes.at(node).childs[6 ^ a];
			context.current = new_node(tx1, 8, ty1, 8, tzm, 7);
			tx0 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 7:
			node = nodes.at(node).childs[7 ^ a];
			context.current = 8;
			tx0 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 8:
			do {

				sp -= 1;
				if (sp < 0) {
					result = -1;
					return;
				}

				tx0 = states[sp].tx0;
				ty0 = states[sp].ty0;
				tz0 = states[sp].tz0;
				tx1 = states[sp].tx1;
				ty1 = states[sp].ty1;
				tz1 = states[sp].tz1;
				current = states[sp].current;
				node = states[sp].node_id;

				txm = 0.5f * (tx0 + tx1);
				tym = 0.5f * (ty0 + ty1);
				tzm = 0.5f * (tz0 + tz1);

				switch (current) {

				case 0:
					node = nodes.at(node).childs[a];
					context.current = new_node(txm, 4, tym, 2, tzm, 1);
					tx1 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 1:
					node = nodes.at(node).childs[1 ^ a];
					context.current = new_node(txm, 5, tym, 3, tz1, 8);
					tx1 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 2:
					node = nodes.at(node).childs[2 ^ a];
					context.current = new_node(txm, 6, ty1, 8, tzm, 3);
					tx1 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 3:
					node = nodes.at(node).childs[3 ^ a];
					context.current = new_node(txm, 7, ty1, 8, tz1, 8);
					tx1 = txm; ty0 = tym; tz0 = tzm;
					break;

				case 4:
					node = nodes.at(node).childs[4 ^ a];
					context.current = new_node(tx1, 8, tym, 6, tzm, 5);
					tx0 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 5:
					node = nodes.at(node).childs[5 ^ a];
					context.current = new_node(tx1, 8, tym, 7, tz1, 8);
					tx0 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 6:
					node = nodes.at(node).childs[6 ^ a];
					context.current = new_node(tx1, 8, ty1, 8, tzm, 7);
					tx0 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 7:
					node = nodes.at(node).childs[7 ^ a];
					context.current = 8;
					tx0 = txm; ty0 = tym; tz0 = tzm;
					break;

				}

			} while (current == 8);

			break;
		}

		states[sp] = context;
		sp += 1;

	} while (sp >= 0 && cycles < 10);
}
>>>>>>> origin/master
