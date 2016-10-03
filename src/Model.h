/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Model.h - trida pro reprezentaci modelu
*/

#pragma once

#include <iostream>
#include <vector>
#include "Triangle.h"

using namespace std;

class Model {

public:

	// Konstruktor
	Model(string model_file);

	// Ziskani trojuhelniku modelu
	vector<Triangle> getTriangles();

private:

	// Trojuhelniky modelu
	vector<Triangle> triangles;
	uint tri_count;

};