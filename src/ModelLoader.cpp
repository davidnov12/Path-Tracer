/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * ModelLoader.h - trida pro nacteni modelu
 */

//#define _CRT_SECURE_NO_WARNINGS
#include "ModelLoader.h"


/*ModelLoader::ModelLoader(string path, Modes mode, Material material){
	load_mode = mode;
	mat = material;
	loadModel(path, mode);
}*/

ModelLoader::ModelLoader(string path, Modes mode, Material material){
	load_mode = mode;
	mat = material;
	loadModel(path, mode);
}

vector<Triangle> ModelLoader::getData(){
	return triangles;
}

void ModelLoader::cleanData(){
	vertices.clear();
	tex_coords.clear();
	normals.clear();
	vertex_indices.clear();
	uv_indices.clear();
	normal_indices.clear();
	triangles.clear();
}

void ModelLoader::loadModel(string path, int mode){
	
	ifstream modelFile;
	modelFile.exceptions(ifstream::badbit);
	string data;
	string tmp;

	try {
		// Otevreni souboru
		modelFile.open(path);
		stringstream stream;

		// Precteni a uzavreni souboru
		stream << modelFile.rdbuf();
		modelFile.close();

		// Obsah souboru
		data = stream.str();
	}

	catch (ifstream::failure e) {

		// Chyba pri cteni souboru
		cout << "READ ERROR" << endl;
		return;
	}

	while (data.compare("") != 0) {

		if (data.find("\n") == string::npos) {
			tmp = data.substr(0, data.find("\0"));
			tmp = data;
			data = "";
		}
		else {
			tmp = data.substr(0, data.find("\n"));
			data = data.substr(data.find("\n") + 1, data.length() - data.find("\n") - 1);
		}

		if(tmp.find("v ") != string::npos){
			vec3 temp;
			tmp = tmp.substr(2, tmp.length() - 3);
			sscanf_s(tmp.c_str(), "%f %f %f", &temp.x, &temp.y, &temp.z);
			//cout << "VERTEX " << temp.x << "  " << temp.y << "  " << temp.z << endl;
			//cout << tmp << endl;
			vertices.push_back(temp);
		}
		else if (tmp.find("vt ") != string::npos) {
			vec2 temp;
			tmp = tmp.substr(3, tmp.length() - 4);
			sscanf_s(tmp.c_str(), "%f %f", &temp.x, &temp.y);
			//cout << "UV " << temp.x << "  " << temp.y << endl;
			tex_coords.push_back(temp);
		}
		else if (tmp.find("vn ") != string::npos) {
			vec3 temp;
			tmp = tmp.substr(3, tmp.length() - 4);
			sscanf_s(tmp.c_str(), "%f %f %f", &temp.x, &temp.y, &temp.z);
			//cout << "NORMAL " << temp.x << "  " << temp.y << "  " << temp.z << endl;
			normals.push_back(temp);
		}
		else if (tmp.find("f ") != string::npos) {
			int indices[9];
			tmp = tmp.substr(2, tmp.length());
			
			vector<vec3> verts;
			vector<vec2> uvs;
			vector<vec3> normal;
			
			if (mode == VERTEX_UVS_NORMALS) {
				sscanf_s(tmp.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5, indices + 6, indices + 7, indices + 8);

				verts.push_back(vertices.at(indices[0] - 1));
				verts.push_back(vertices.at(indices[3] - 1));
				verts.push_back(vertices.at(indices[6] - 1));

				uvs.push_back(tex_coords.at(indices[1] - 1));
				uvs.push_back(tex_coords.at(indices[4] - 1));
				uvs.push_back(tex_coords.at(indices[7] - 1));

				normal.push_back(normals.at(indices[2] - 1));
				normal.push_back(normals.at(indices[5] - 1));
				normal.push_back(normals.at(indices[8] - 1));

				va.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[3] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[6] - 1), mat.disperse));

				ua.push_back(vec4(tex_coords.at(indices[1] - 1), tex_coords.at(indices[4] - 1)));
				ua.push_back(vec4(tex_coords.at(indices[7] - 1), 1.0f, 1.0f));

				na.push_back(vec4(normals.at(indices[2] - 1), 1.0f));
				na.push_back(vec4(normals.at(indices[5] - 1), 1.0f));
				na.push_back(vec4(normals.at(indices[8] - 1), mat.absorption));

				ca.push_back(vec4(0.9, 0.9, 0.9, 0.0));

				Triangle t(verts, uvs, normal);

				triangles.push_back(t);
			}
			else if (mode == VERTEX_NORMALS) {
				if (sscanf_s(tmp.c_str(), "%d/%d %d/%d %d/%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5) != 6) {

					if (sscanf_s(tmp.c_str(), "%d//%d %d//%d %d//%d", indices, indices + 1, indices + 2,
						indices + 3, indices + 4, indices + 5) != 6) return;
				}

				verts.push_back(vertices.at(indices[0] - 1));
				verts.push_back(vertices.at(indices[2] - 1));
				verts.push_back(vertices.at(indices[4] - 1));

				normal.push_back((normals.at(indices[1] - 1)));
				normal.push_back((normals.at(indices[3] - 1)));
				normal.push_back((normals.at(indices[5] - 1)));

				va.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[2] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[4] - 1), mat.disperse));

				ua.push_back(vec4(0.0f));
				ua.push_back(vec4(0.0f));

				na.push_back(vec4(normals.at(indices[1] - 1), 1.0f));
				na.push_back(vec4(normals.at(indices[3] - 1), 1.0f));
				na.push_back(vec4(normals.at(indices[5] - 1), mat.absorption));

				ca.push_back(vec4(mat.color, mat.reflection));

				Triangle t(verts, normal);

				triangles.push_back(t);
			}
			else if (mode == VERTEX_UVS) {
				if (sscanf_s(tmp.c_str(), "%d/%d %d/%d %d/%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5) != 6) {

					if (sscanf_s(tmp.c_str(), "%d//%d %d//%d %d//%d", indices, indices + 1, indices + 2,
						indices + 3, indices + 4, indices + 5) != 6) return;
				}

				verts.push_back(vertices.at(indices[0] - 1));
				verts.push_back(vertices.at(indices[2] - 1));
				verts.push_back(vertices.at(indices[4] - 1));

				uvs.push_back(tex_coords.at(indices[1] - 1));
				uvs.push_back(tex_coords.at(indices[3] - 1));
				uvs.push_back(tex_coords.at(indices[5] - 1));

				va.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[2] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[4] - 1), mat.disperse));

				ua.push_back(vec4(tex_coords.at(indices[1] - 1), tex_coords.at(indices[3] - 1)));
				ua.push_back(vec4(tex_coords.at(indices[5] - 1), 1.0f, 1.0f));

				na.push_back(vec4(0.0f));
				na.push_back(vec4(0.0f));
				na.push_back(vec4(mat.absorption));

				ca.push_back(vec4(mat.color, mat.reflection));

				Triangle t(verts, uvs, normal);

				triangles.push_back(t);
			}
			else if (mode == VERTEX) {
				sscanf_s(tmp.c_str(), "%d %d %d", indices, indices + 1, indices + 2);

				verts.push_back(vertices.at(indices[0] - 1));
				verts.push_back(vertices.at(indices[1] - 1));
				verts.push_back(vertices.at(indices[2] - 1));

				va.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[1] - 1), 1.0f));
				va.push_back(vec4(vertices.at(indices[2] - 1), mat.disperse));

				ua.push_back(vec4(0.0f));
				ua.push_back(vec4(0.0f));

				na.push_back(vec4(0.0f));
				na.push_back(vec4(0.0f));
				na.push_back(vec4(mat.absorption));

				ca.push_back(vec4(mat.color, mat.reflection));

				Triangle t(verts, uvs, normal);

				triangles.push_back(t);
			}

		}
		//cout << "TMP " << tmp << endl;
		//cout << d << endl;
	}
}
