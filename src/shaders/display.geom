#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 tex_coords;

void main(){

	for(int i = 0; i < 4; ++i){
		gl_Position = vec4(-1.0 + 2.0 * (i % 2),-1.0 + 2.0 * (i / 2), 0.0, 1.0);
		tex_coords = vec2(i % 2, i / 2);
		EmitVertex();
	}

	EndPrimitive();
}
