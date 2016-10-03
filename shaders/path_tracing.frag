#version 400

uniform vec3 light_pos;
uniform vec3 view_pos;

out vec4 color;

void main(){
	color = vec4(0.3, 0.8, 0.2, 1.0);
}