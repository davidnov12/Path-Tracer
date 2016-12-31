/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * path_tracing.vert - Vertex Shader (souradnice prumetny)
 */

#version 450

layout (location = 0) in vec3 pos;

out vec3 scr_coord;

void main(){
	scr_coord = vec3(pos.xyz);
	gl_Position = vec4(pos.xyz, 1.0);
}