#version 450

layout (location = 0) in vec3 pos;

out vec3 scr_coord;

void main(){
	scr_coord = vec3(pos.x, pos.y, pos.z);
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	//gl_Position = vec4(0.0);
}