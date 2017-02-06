#version 450

uniform sampler2D screen;

in vec2 tex_coords;

out vec4 color;

void main(){
	//color = vec4(0.4, 0.1, 0.7, 1.0);
	color = texture(screen, tex_coords);
	//color = vec4(color.r, color.g, color.b, 1.0);
	//color = vec4(0.0, 1.0, 0.6, 1.0);
}