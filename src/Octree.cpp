/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.cpp - trida pro praci s oktalovym stromem
 */


#include "Octree.h"

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
}

void Octree::initOctree(){
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

void Octree::buildOctree(Scene::Model md, int primitivesPerList){
	
	vec4 edge, v1, v2;
	float len;

	int cn = 0;

	// Vsechny uzly Octree
	for (int j = 0; j < nodes.size(); j++) {

		//cout << j << "   " << nodes.size() << endl;

		// Listove uzly
		if (nodes.at(j).leaf) {

			// Vsechny trojuhelniky v modelu
			for (int i = 0; i < md.triangles_count; i++) {

				// Vsechny 3 hrany trojuhelniku
				for(int k = 0; k < 3; k++){
					
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
						
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Vrchol B trojuhelniku lezi v bounding boxu
					if (isInBox(vec3(v1), vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
						
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Hrana trojuhelniku prochazi pres bounding box
					if (edgeBoxIntersection(v1, edge, vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
						//cout << "EDG " << t << endl;
						if (!uniqueIndices(i, j) && t <= len && t >= 0.0) {
							tmp_indices.at(j).push_back(i);
						}
					}
				}
			}

			// Rozdeleni uzlu
			if (tmp_indices.at(j).size() > primitivesPerList && nodes.size() < MAX_NODES){
				
				makeChilds(j);
			}
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
			if (abs(t) < abs(closest) && t > 0.0) {
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
			if (abs(t) < abs(closest) && t > 0.0) {
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
			if (abs(t) < abs(closest) && t > 0.0) {
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
			if (abs(t) < abs(closest) && t > 0.0) {
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
			if (abs(t) < abs(closest) && t > 0.0) {
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
			if (abs(t) < abs(closest) && t > 0.0) {
				closest = t;
				res = true;
			}
		}
	//}

	t = closest;
	
	return res;

	
}

bool Octree::uniqueIndices(int inserted, int nodeIndex){
	
	for (int i = 0; i < tmp_indices.at(nodeIndex).size(); i++)
		if (tmp_indices.at(nodeIndex).at(i) == inserted)
			return true;

	return false;
}

void Octree::linkOctree(){
	int current_prt = 0;

	for (int i = 0; i < nodes.size(); i++) {
		
		if (nodes.at(i).leaf) {
			nodes.at(i).index = current_prt;
			nodes.at(i).count = tmp_indices.at(i).size();
			current_prt += tmp_indices.at(i).size();
			
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