#version 450

layout (location = 0) in vec3 pos;

out vec3 scr_coord;

uniform float width, height;

void main(){
<<<<<<< HEAD
	float ratio = 1.0;
	
	if(width != 0.0 || height != 0.0)
		ratio = width / height;

	scr_coord = vec3(pos.x * ratio, pos.y, pos.z);
	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
}
=======
	scr_coord = vec3(pos.xyz);
	gl_Position = vec4(pos.xyz, 1.0);
}
>>>>>>> origin/master
