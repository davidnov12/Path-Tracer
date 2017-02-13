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
	initOctree();
	buildOctree(md, primitivesPerList);
}

void Octree::initOctree(){
	Node n;
	n.index = n.count = 0;
	n.leaf = 1;
	n.start = vec4(LEFT, DOWN, 1.0f, 1.0f);
	n.end = vec4(RIGHT, UP, FRONT, 1.0f);
	for (int i = 0; i < 8; i++)
		n.childs[i] = -1;

	nodes.push_back(n);

	vector<int> c;
	tmp_indices.push_back(c);

	nodes_count = 1;
}

void Octree::buildOctree(Scene::Model md, int primitivesPerList){
	makeChilds(nodes.at(0));
	vec4 edge, v1, v2;

	// Vsechny trojuhelniky v modelu
	for (int i = 0; i < md.triangles_count; i++) {

		// Vsechny hrany trojuhelniku
		for(int k = 0; k < 3; k++){
			
			// Hrany trojuhelniku
			if (k == 0) {
				edge = md.data[i].vertex1 - md.data[i].vertex0;
				v1 = md.data[i].vertex0;
				v2 = md.data[i].vertex1;
			}
			else if (k == 1) {
				edge = md.data[i].vertex2 - md.data[i].vertex1;
				v1 = md.data[i].vertex1;
				v2 = md.data[i].vertex2;
			}
			else if (k == 2) {
				edge = md.data[i].vertex0 - md.data[i].vertex2;
				v1 = md.data[i].vertex2;
				v2 = md.data[i].vertex0;
			}

			float len = length(vec3(edge));

			// Vsechny uzly octree
			for (int j = 0; j < nodes_count; j++) {
			
				// Listovy bounding box
				if (nodes.at(j).leaf) {
				
					// Vrchol A trojuhelniku lezi v bounding boxu
					if (isInBox(vec3(v2), vec3(nodes.at(j).start), vec3(nodes.at(j).end))){
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Vrchol B trojuhelniku lezi v bounding boxu
					if (isInBox(vec3(v1), vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
						if (!uniqueIndices(i, j))
							tmp_indices.at(j).push_back(i);
					}

					// Hrana trojuhelniku prochazi pres bounding box
					if (edgeBoxIntersection(v1, edge, vec3(nodes.at(j).start), vec3(nodes.at(j).end), len)){
						vec3 point = vec3(v1) + ((t + 0.0001f) * normalize(vec3(edge)));

						if (isInBox(point, vec3(nodes.at(j).start), vec3(nodes.at(j).end))) {
							if (!uniqueIndices(i, j))
								tmp_indices.at(j).push_back(i);
						}
					}
				}
			}
		}
	}
}

bool Octree::isInBox(vec3 point, vec3 startBox, vec3 endBox){
	if (point.x >= startBox.x && point.y >= startBox.y && point.z >= startBox.z) 
		if((point.x <= endBox.x && point.y <= endBox.y && point.z <= endBox.z))
			return true;
	
	return false;
}

void Octree::makeChilds(Node n){
	
	vec4 half = (n.end - n.start) / 2.0f;
	Node news[8];

	for(int a = 0; a < 8; a++){
		for(int i = 0; i < 8; i++)
			news[a].childs[i] = -1;
		
		news[a].index = 0;
		news[a].count = 0;
		news[a].leaf = 1;
	}

	news[0].start = vec4(n.start.x, n.start.y, n.start.z, 1.0f);
	news[0].end = vec4(half.x, half.y, half.z, 1.0f);

	news[1].start = vec4(n.start.x, n.start.y, half.z, 1.0f);
	news[1].end = vec4(half.x, half.y, n.end.z, 1.0f);

	news[2].start = vec4(half.x, n.start.y, n.start.z, 1.0f);
	news[2].end = vec4(n.end.x, half.y, half.z, 1.0f);

	news[3].start = vec4(half.x, n.start.y, half.z, 1.0f);
	news[3].end = vec4(n.end.x, half.y, n.end.z, 1.0f);

	news[4].start = vec4(n.start.x, half.y, n.start.z, 1.0f);
	news[4].end = vec4(half.x, n.end.y, half.z, 1.0f);

	news[5].start = vec4(n.start.x, half.y, half.z, 1.0f);
	news[5].end = vec4(half.x, n.end.y, n.end.z, 1.0f);

	news[6].start = vec4(half.x, half.y, n.start.z, 1.0f);
	news[6].end = vec4(n.end.x, n.end.y, half.z, 1.0f);

	news[7].start = vec4(half.x, half.y, half.z, 1.0f);
	news[7].end = vec4(n.end.x, n.end.y, n.end.z, 1.0f);

	for (int a = 0; a < 8; a++) {
		vector<int> v;
		tmp_indices.push_back(v);
		nodes.push_back(news[a]);
		n.childs[a] = nodes_count + a;
	}
	n.leaf = 0;

	nodes_count = nodes.size();
}

bool Octree::edgeBoxIntersection(vec4 origin, vec4 edge, vec3 boxStart, vec3 boxEnd, float length){
	
	bool res = false;
	float closest = 5.0;
	float planeHeight;
	float c1, c2;
	vec3 point;
	float planeHeights[6] = {boxStart.y, boxEnd.y, boxStart.x, boxEnd.x, boxEnd.z, boxStart.z};
	float conditions[3] = {edge.y, edge.x, edge.z};
	float ts[3] = {(planeHeight - origin.y) / edge.y, (planeHeight - origin.x) / edge.x, (planeHeight - origin.z) / edge.z};
	float condCoord[12] = {boxEnd.z, boxStart.z, boxStart.x, boxEnd.x, 
						   boxEnd.z, boxStart.z, boxStart.y, boxEnd.y,
						   boxStart.x, boxEnd.x, boxStart.y, boxEnd.y};

	for (int i = 0; i < 6; i++) {
		
		if (i % 2 == 0) {
			if (conditions[i / 2] < 0.0)
				continue;
		}
		else {
			if (conditions[i / 2] > 0.0)
				continue;
		}

		planeHeight = planeHeights[i];
		t = ts[i / 2];
		point = vec3(origin + (t * edge));

		if(i < 2){
			c1 = point.z;
			c2 = point.x;
		}
		else if(i < 4){
			c1 = point.z;
			c2 = point.y;
		}
		else{
			c1 = point.x;
			c2 = point.y;
		}

		int index = 4 * (i / 2);
		if (c1 > condCoord[index] && c1 < condCoord[index +1] && c2 > condCoord[index + 2] && c2 < condCoord[index + 3]) {
			if (abs(t) < abs(closest) && t <= length) {
				closest = t;
				res = true;
			}
		}

	}

	/*
	// Dolni stena
	if (edge.y < 0.0) {
		planeHeight = boxStart.y;
		t = (planeHeight - origin.y) / edge.y;
		point = vec3(origin + (t * edge));
		
		if (point.z > boxEnd.z && point.z < boxStart.z && point.x > boxStart.x && point.x < boxEnd.x) {
			if (abs(t) < abs(closest)) {
				closest = t;
				res = true;
			}
		}
	}

	// Horni stena
	if (edge.y > 0.0) {
		planeHeight = boxEnd.y;
		t = (planeHeight - origin.y) / edge.y;
		point = vec3(origin + (t * edge));

		if (point.z > boxEnd.z && point.z < boxStart.z && point.x > boxStart.x && point.x < boxEnd.x) {
			if (abs(t) < abs(closest)) {
				closest = t;
				res = true;
			}
		}
	}

	// Leva stena
	if (edge.x < 0.0) {
		planeHeight = boxStart.x;
		t = (planeHeight - origin.x) / edge.x;
		point = vec3(origin + (t * edge));

		if (point.z > boxEnd.z && point.z < boxStart.z && point.y > boxStart.y && point.y < boxEnd.y) {
			if (t < closest) {
				closest = t;
				res = true;
			}
		}
	}

	// Prava stena
	if (edge.x > 0.0) {
		planeHeight = boxEnd.x;
		t = (planeHeight - origin.x) / edge.x;
		point = vec3(origin + (t * edge));

		if (point.z > boxEnd.z && point.z < boxStart.z && point.y > boxStart.y && point.y < boxEnd.y) {
			if (t < closest) {
				closest = t;
				res = true;
			}
		}
	}

	// Predni stena
	if (edge.z < 0.0) {
		planeHeight = boxEnd.z;
		t = (planeHeight - origin.z) / edge.z;
		point = vec3(origin + (t * edge));

		if (point.x > boxStart.x && point.x < boxEnd.x && point.y > boxStart.y && point.y < boxEnd.y) {
			if (t < closest) {
				closest = t;
				res = true;
			}
		}
	}

	// Zadni stena
	if (edge.z > 0.0) {
		planeHeight = boxStart.z;
		t = (planeHeight - origin.z) / edge.z;
		point = vec3(origin + (t * edge));

		if (point.x > boxStart.x && point.x < boxEnd.x && point.y > boxStart.y && point.y < boxEnd.y) {
			if (t < closest) {
				closest = t;
				res = true;
			}
		}
	}*/
	
	t = closest;
	return res;
}

bool Octree::uniqueIndices(int inserted, int nodeIndex){
	
	for (int i = 0; i < tmp_indices.at(nodeIndex).size(); i++)
		if (tmp_indices.at(nodeIndex).at(i) == inserted)
			return true;

	return false;
}
