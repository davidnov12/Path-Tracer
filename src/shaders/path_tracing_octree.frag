#version 450

#define MAX_DEPTH 4

#define LEFT -0.7
#define RIGHT 0.7
#define UP 0.7
#define DOWN -0.65
#define FRONT -0.9
#define BACK 0.9

#define PI 3.1415926535

// Paprsek
struct Ray{
	vec3 position;
	vec3 direction;
};

// Box
struct Box{
	float walls[5];
	vec3 colors[5];
	vec3 normals[5];
	float absorption[5];
	float disperse[5];
	float reflectivity[5];
};

// Koule
struct Sphere{
	vec3 center;
	float radius;
	vec3 color;
	float reflectivity;
	float disperse;
	float absorption;
};

// Trojuhelnik
struct Triangle{
	vec3 vertex0;
	vec3 vertex1;
	vec3 vertex2;
	float disperse;
	vec3 normal0;
	vec3 normal1;
	vec3 normal2;
	float absorption;
	vec2 uv0;
	vec2 uv1;
	vec2 uv2;
	vec3 color;
	float reflectivity;
};

// Prusecik
struct Intersection{
	float dist;
	vec3 position;
	vec3 normal;
	vec3 color;
	float reflectivity;
	float absorption;
	float disperse;
};

// Uzel Octree
struct Node{
	int childs[8];
	int index;
	int count;
	int leaf;
	int gap;
	vec3 start;
	vec3 end;
};

// Kontext pri pruchodu Octree 
struct OctreeContext{
	//float tx; float ty; float tz;
	float tx0; float ty0; float tz0;
	float tx1; float ty1; float tz1;
	uint current;
};

// Trojuhelniky
layout (std430, binding = 0) buffer geometry{
    Triangle triangles[];
};

// Uzly Octree
layout (std430, binding = 1) buffer tree_nodes{
	Node nodes[];
};

// Indexy Octree
layout (std430, binding = 2) buffer node_indices{
	int indices[];
};

// Koule
layout (std430, binding = 3) buffer sphere_geom{
	Sphere spheres[];
};

uniform Box cornell_box;
uniform float randoms[30];
uniform float sample_ID;
uniform float stride;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform int spheres_count;
uniform int triangles_count;
uniform int nodes_count;
uniform int indices_count;

in vec3 scr_coord;

out vec4 color;

float ambient = 0.05;
float diffuse = 0.95;

bool trian = true;

vec3 light_color = vec3(1.0, 1.0, 1.0);

Ray original_ray;

// Vygenerovani nahodneho smeru uvnitr polokoule na zaklade nahodnych cisel
vec3 randomDirection(float rand1, float rand2){
	float sinTheta = sqrt(1 - rand1 * rand1);
	//float phi = 2 * PI * rand2;
	float x = sinTheta * cos(2 * PI * rand2);
	float z = sinTheta * sin(2 * PI * rand2);
	return vec3(x, rand1, z);
}


// Spocitani tangenty a binormaly
void calcTB(vec3 normal, out vec3 tangent, out vec3 bitangent){
	
	if(abs(normal.x) != 1.0 && abs(normal.y) != 1.0 && abs(normal.z) != 1.0){ 

		tangent = cross(normal, vec3(0.0, 1.0, 0.0));
		tangent = normalize(tangent);
		bitangent = cross(normal, tangent);
		bitangent = normalize(bitangent);
	}

	else{

		vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0));
		vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0));

		if(length(c1) > length(c2))
			tangent = c1;
		else
			tangent = c2;

		tangent = normalize(tangent);
		bitangent = cross(normal, tangent);
		bitangent = normalize(bitangent);
	}
	
}


// Prevedni vygenerovaneho smeru do lokalniho souradneho systemu v bode kolize
vec3 convertToBNT(vec3 original, vec3 normal, vec3 tangent, vec3 bitangent){
	mat3 trans = mat3(bitangent, normal, tangent);
	vec3 dir = trans * original;
	return dir;	
}

// Funkce pro ziskani barycentrickych souradnic
void barycentricInterpolation(Triangle tr, vec3 point, out vec3 bar_coords){
	vec3 v0 = tr.vertex1 - tr.vertex0;
	vec3 v1 = tr.vertex2 - tr.vertex0;
	vec3 v2 = point - tr.vertex0;
	
	float d00 = dot(v0, v0);
	float d01 = dot(v0, v1);
	float d11 = dot(v1, v1);
	float d20 = dot(v2, v0);
	float d21 = dot(v2, v1);

	float den = d00 * d11 - d11 * d01;
	bar_coords.y = (d11 * d20 - d01 * d21) / den;
	bar_coords.z = (d00 * d21 - d01 * d20) / den;
	bar_coords.x = 1.0 - bar_coords.y - bar_coords.z;
}

// Prusecik paprsku s trojuhelnikem
bool rayTriangleIntersection(Ray ray, Triangle tr, out Intersection inter){
	vec3 e1 = tr.vertex1 - tr.vertex0;
	vec3 e2 = tr.vertex2 - tr.vertex0;

	vec3 s1 = cross(ray.direction, e2);

	float div = dot(s1, e1);
	if(div == 0) return false;

	float invdiv = 1 / div;
	vec3 dis = ray.position - tr.vertex0;
	vec3 s2 = cross(dis, e1);

	float t = dot(e2, s2) * invdiv;

	if(t < 0.009) return false;

	float b1 = dot(dis, s1) * invdiv;
	if (b1 < 0.0 || b1 > 1.0) return false;
	
	float b2 = dot(ray.direction, s2) * invdiv;
	if (b2 < 0.0 || (b1 + b2) > 1.0) return false;	

	

	inter.dist = t;
	inter.position = ray.position + t * ray.direction;
	
	vec3 bar_coords;
	barycentricInterpolation(tr, inter.position, bar_coords);
	inter.normal = normalize(tr.normal0 * bar_coords.x + tr.normal1 * bar_coords.y + tr.normal2 * bar_coords.z);
	//inter.normal = (tr.normal0 + tr.normal1 + tr.normal2) / 3.0;
	//vec2 uvs = tr.uv0 * bar_coords.x + tr.uv1 * bar_coords.y + tr.uv2 * bar_coords.z;
	//inter.color = texture(tex, uvs);

	//inter.normal = tr.normal;
	inter.color = tr.color;
	inter.reflectivity = tr.reflectivity;
	inter.absorption = tr.absorption;
	inter.disperse = tr.disperse;

	return true;
}


// Prusecik paprsku s kouli
bool raySphereIntersection(Ray ray, Sphere sp, out Intersection inter){
	
	vec3 center = sp.center;
	float radius = sp.radius;

	float t0;
	vec3 L = center - ray.position;
	float tca = dot(L, ray.direction);
	
	if (tca < 0) return false;
	float d2 = dot(L, L) - tca * tca;
	
	if (d2 > radius) return false;
	float thc = sqrt(radius - d2);
	
	t0 = tca - thc;
	//t1 = tca + thc;

	if(t0 < 0.000001) return false;

	inter.dist = t0;
	inter.position = ray.position + (t0 * ray.direction);
	inter.normal = normalize(inter.position - center);
	inter.color = sp.color;
	inter.reflectivity = sp.reflectivity;
	inter.disperse = sp.disperse;
	inter.absorption = sp.absorption;

	return true;
}


// Prusecik paprsku se stenami
bool rayBoxIntersection(Ray ray, out Intersection inter){
	
	bool res = false;
	float closest = 15.0;
	float t = 10.0;

	float nom[5] = {ray.position.y, ray.position.y, ray.position.x, ray.position.x, ray.position.z};
	float denom[5] = {ray.direction.y, ray.direction.y, ray.direction.x, ray.direction.x, ray.direction.z};

	for(int a = 0; a < 5; a++){
		
		if(a % 2 == 0){
			if(denom[a] > 0.0) continue;
		}

		else{
			if(denom[a] < 0.0) continue;
		}
		
		t = (cornell_box.walls[a] - nom[a]) / denom[a];
		
		if(t < closest && t > 0.0){
			closest = t;
			
			inter.dist = t;
			inter.position = ray.position + (t * ray.direction);
			inter.color = cornell_box.colors[a];
			inter.normal = cornell_box.normals[a];
			inter.reflectivity = cornell_box.reflectivity[a];
			inter.absorption = cornell_box.absorption[a];
			inter.disperse = cornell_box.disperse[a];

			res = true;
		}
	}

	return res;
}


// Octree

// Prvni zasazeny uzel
uint firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) {
	int answer = 0;
							
	if (tx0 > ty0) {
		if (tx0 > tz0) { // PLANE YZ
			if (tym < tx0) answer |= 2;    
			if (tzm < tx0) answer |= 1;    
			return answer;
		}
	}

	else if (ty0 > tz0) { // PLANE XZ
			if (txm < ty0) answer |= 4;    
			if (tzm < ty0) answer |= 1; 
			return answer;
	}

	// PLANE XY
	if (txm < tz0) answer |= 4;    
	if (tym < tz0) answer |= 2;    
	return answer;
}


// Dalsi zasazeny uzel
int nextNode(float txm, int x, float tym, int y, float tzm, int z) {
	if (txm < tym) {
		if (txm < tzm) return x;   
	}

	else {
		if (tym < tzm) return y; 
	}

	return z;
}

// Pruchod Octree
bool iterativeTraversal(Ray ray, float tx0, float tx1, float ty0, float ty1, float tz0, float tz1, uint fg, uint node, out Intersection inters) {
	uint current;
	float txm, tym, tzm;
	float dx, dy, dz;
	bool init = false, res = false;
	OctreeContext context;
	OctreeContext states[12];
	float closest = 100.0;
	int sp = 0;
	int cycles = 0;
	trian = true;

	dx = tx1 - tx0;
	dy = ty1 - ty0;
	dz = tz1 - tz0;

	do{
		cycles += 1;
		

		if (tx1 < 0 || ty1 < 0 || tz1 < 0) {
			sp -= 1;
			if (sp < 0) {
				break;
			}

			/*tx0 = 0.5 * (states[sp].tx - (dx / pow(2.0, sp)));
			ty0 = 0.5 * (states[sp].ty - (dy / pow(2.0, sp)));
			tz0 = 0.5 * (states[sp].tz - (dz / pow(2.0, sp)));
			tx1 = states[sp].tx - tx0;
			ty1 = states[sp].ty - ty0;
			tz1 = states[sp].tz - tz0;*/
			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current & 0x0f;
			node = states[sp].current >> 4;
			init = true;
			txm = 0.5 * (tx0 + tx1);
			tym = 0.5 * (ty0 + ty1);
			tzm = 0.5 * (tz0 + tz1);
		}
		
		//if(node == 20) ab = 12;
		if (nodes[node].leaf == 1) {
				
			
			//if(node == 6) ab = 10;

			if (nodes[node].count != 0) {

				Intersection inter;
				for(int i = 0; i < nodes[node].count; i++){
					
					if(rayTriangleIntersection(original_ray, triangles[indices[nodes[node].index + i]], inter)){	
						
						if(!res || inter.dist < closest){
						
							closest = inter.dist;
							res = true;
							inters = inter;
							trian = false;
						}
					}
				}
				
				if(res) return true;
				
			}

			sp -= 1;
			if (sp < 0) {
				break;
			}

			/*tx0 = 0.5 * (states[sp].tx - (dx / pow(2.0, sp)));
			ty0 = 0.5 * (states[sp].ty - (dy / pow(2.0, sp)));
			tz0 = 0.5 * (states[sp].tz - (dz / pow(2.0, sp)));
			tx1 = states[sp].tx - tx0;
			ty1 = states[sp].ty - ty0;
			tz1 = states[sp].tz - tz0;*/
			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current & 0x0f;
			node = states[sp].current >> 4;
			init = true;
		}

		
		txm = 0.5f * (tx0 + tx1);
		tym = 0.5f * (ty0 + ty1);
		tzm = 0.5f * (tz0 + tz1);


		if (!init) {
			init = true;
			current = firstNode(tx0, ty0, tz0, txm, tym, tzm);
		}


		/*context.tx = tx0 + tx1;
		context.ty = ty0 + ty1;
		context.tz = tz0 + tz1;*/
		context.tx0 = tx0;
		context.ty0 = ty0;
		context.tz0 = tz0;
		context.tx1 = tx1;
		context.ty1 = ty1;
		context.tz1 = tz1;
		context.current = 0;
		context.current = context.current | (node << 4);
		init = false;

		switch (current) {
		case 0:
			node = nodes[node].childs[fg];
			context.current = context.current | nextNode(txm, 4, tym, 2, tzm, 1);
			tx1 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 1:
			node = nodes[node].childs[fg ^ 1];
			context.current = context.current | nextNode(txm, 5, tym, 3, tz1, 8);
			tx1 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 2:
			node = nodes[node].childs[fg ^ 2];
			context.current = context.current | nextNode(txm, 6, ty1, 8, tzm, 3);
			tx1 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 3:
			node = nodes[node].childs[fg ^ 3];
			context.current = context.current | nextNode(txm, 7, ty1, 8, tz1, 8);
			tx1 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 4:
			node = nodes[node].childs[fg ^ 4];
			context.current = context.current | nextNode(tx1, 8, tym, 6, tzm, 5);
			tx0 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 5:
			node = nodes[node].childs[fg ^ 5];
			context.current = context.current | nextNode(tx1, 8, tym, 7, tz1, 8);
			tx0 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 6:
			node = nodes[node].childs[fg ^ 6];
			context.current = context.current | nextNode(tx1, 8, ty1, 8, tzm, 7);
			tx0 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 7:
			node = nodes[node].childs[fg ^ 7];
			context.current = context.current | 8;
			tx0 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 8:
			do {
			
				cycles += 1;

				sp -= 1;
				if (sp < 0) {
					return false;
				}

				/*tx0 = 0.5 * (states[sp].tx - (dx / pow(2.0, sp)));
				ty0 = 0.5 * (states[sp].ty - (dy / pow(2.0, sp)));
				tz0 = 0.5 * (states[sp].tz - (dz / pow(2.0, sp)));
				tx1 = states[sp].tx - tx0;
				ty1 = states[sp].ty - ty0;
				tz1 = states[sp].tz - tz0;*/
				tx0 = states[sp].tx0;
				ty0 = states[sp].ty0;
				tz0 = states[sp].tz0;
				tx1 = states[sp].tx1;
				ty1 = states[sp].ty1;
				tz1 = states[sp].tz1;
				current = states[sp].current & 0x0f;
				node = states[sp].current >> 4;

				txm = 0.5f * (tx0 + tx1);
				tym = 0.5f * (ty0 + ty1);
				tzm = 0.5f * (tz0 + tz1);

				switch (current) {

				case 0:
					node = nodes[node].childs[fg];
					context.current = context.current | nextNode(txm, 4, tym, 2, tzm, 1);
					tx1 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 1:
					node = nodes[node].childs[1 ^ fg];
					context.current = context.current | nextNode(txm, 5, tym, 3, tz1, 8);
					tx1 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 2:
					node = nodes[node].childs[2 ^ fg];
					context.current = context.current | nextNode(txm, 6, ty1, 8, tzm, 3);
					tx1 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 3:
					node = nodes[node].childs[3 ^ fg];
					context.current = context.current | nextNode(txm, 7, ty1, 8, tz1, 8);
					tx1 = txm; ty0 = tym; tz0 = tzm;
					break;

				case 4:
					node = nodes[node].childs[4 ^ fg];
					context.current = context.current | nextNode(tx1, 8, tym, 6, tzm, 5);
					tx0 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 5:
					node = nodes[node].childs[5 ^ fg];
					context.current = context.current | nextNode(tx1, 8, tym, 7, tz1, 8);
					tx0 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 6:
					node = nodes[node].childs[6 ^ fg];
					context.current = context.current | nextNode(tx1, 8, ty1, 8, tzm, 7);
					tx0 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 7:
					node = nodes[node].childs[7 ^ fg];
					context.current = context.current | 8;
					tx0 = txm; ty0 = tym; tz0 = tzm;
					break;

				}

			} while (current == 8 && cycles < 30);
			
			break;
		}

		states[sp] = context;
		sp += 1;

	} while (sp >= 0 && cycles < 30);

	
	return res;
}

// Hlavni funkce pro pruchod Octree
bool octreeTraversal(Ray ray, out Intersection inters) {
	uint fg = 0;
	float tx0, ty0, tz0, tx1, ty1, tz1;

	original_ray.direction = ray.direction;
	original_ray.position = ray.position;
	Ray tmp_ray;
	tmp_ray.position = ray.position;
	tmp_ray.direction = ray.direction;

	if (ray.direction.x < 0.0) {
		tmp_ray.position.x = (RIGHT + LEFT) - ray.position.x;
		tmp_ray.direction.x *= -1.0;
		fg |= 4;
	}

	if (ray.direction.y < 0.0) {
		tmp_ray.position.y = (UP + DOWN) - ray.position.y;
		tmp_ray.direction.y *= -1.0;
		fg |= 2;
	}

	if (ray.direction.z > 0.0) {
		tmp_ray.position.z = (BACK + FRONT) - ray.position.z;
		tmp_ray.direction.z *= -1.0;
		fg |= 1;
	}
/*
	if (ray.direction.x == 0.0) tmp_ray.direction.x = 0.000000000001f;
	if (ray.direction.y == 0.0) tmp_ray.direction.y = 0.000000000001f;
	if (ray.direction.z == 0.0) tmp_ray.direction.z = -0.000000000001f;
	*/
	float invDirx = 1.0 / tmp_ray.direction.x;
	float invDiry = 1.0 / tmp_ray.direction.y;
	float invDirz = 1.0 / tmp_ray.direction.z;

	tx0 = (LEFT - tmp_ray.position.x) * invDirx;
	tx1 = (RIGHT - tmp_ray.position.x) * invDirx;

	ty0 = (DOWN - tmp_ray.position.y) * invDiry;
	ty1 = (UP - tmp_ray.position.y) * invDiry;

	tz0 = (BACK - tmp_ray.position.z) * invDirz;
	tz1 = (FRONT - tmp_ray.position.z) * invDirz;
	
	Intersection inter;
	
	if(max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1))){
		if(iterativeTraversal(tmp_ray, tx0, tx1, ty0, ty1, tz0, tz1, fg, 0, inter)){
			inters = inter;
			return true;
		}
	}
	
	return false;
}

// Spocitani nejblizsi kolize paprsku se scenou
bool calculCollision(Ray ray, out Intersection inter, bool shadow){

	bool intersect = false;
	Intersection coll;
	
	// Koule
	if(ray.position.y < light_pos.y){
		for(int i = 0; i < spheres_count; i++){
			if(raySphereIntersection(ray, spheres[i], coll)){
				if(!intersect || coll.dist < inter.dist){
					intersect = true;
					inter = coll;
				}
			}
		}
	}

	// Octree
	if(triangles_count > 0 && trian){
		if(octreeTraversal(ray, coll)){
			if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
			}
		}
	}
	
	if(intersect) return true;
		
	// Steny
	if(!shadow){
		if(rayBoxIntersection(ray, coll)){
			intersect = true;
			inter = coll;
		}
	}

	return intersect;
}


// Vypocet osvetleni v bode
vec3 directLight(vec3 position, vec3 normal, vec3 color){
  
  vec3 direct = vec3(0.0);
  Ray light_ray;
  Intersection inter;
  bool shadow;

  //vec3 random_offset;
  vec3 random_offset = vec3((0.2 * randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.7 * sample_ID) % 30])-0.1, 0.0, (0.2 * randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.2 * sample_ID) % 30])-0.1);
  light_ray.position = position;
  light_ray.direction = normalize((light_pos + random_offset) - position);;
  float len = length(light_pos - position);
  
  shadow = calculCollision(light_ray, inter, true);
  direct = (ambient * color) + ((shadow? min(inter.dist * 4.9, 0.1) : 1) * max(dot(light_ray.direction, normal), 0.0) * color * diffuse * light_color * (1.0 / (len * 1.1)));
  trian = true;

  return direct;
}


vec3 pathTrace(Ray original_ray){

	Ray ray = original_ray;
	vec3 ray_color = vec3(1.0);
	vec3 point_color;
	vec3 albedo = vec3(1.0);
	vec3 tangent, bitangent;
	Intersection inter;
	int depth = 0;
	float coef = 1.0;
	vec3 color;

	while (depth <= MAX_DEPTH) {
		
		// Zadna kolize
		if(!calculCollision(ray, inter, false)){
				ray_color = vec3(0.2);
				break;
		}
		
		color = inter.color * coef * inter.absorption;

		coef *=  inter.reflectivity;
		
		if(depth == 0)
				albedo = color;

		// Osvetleni v bode
		point_color = directLight(inter.position, inter.normal, inter.color);

		// barva * BRDF(difuzni)
		ray_color = (ray_color / PI + 2 * point_color) * albedo;

		// Konec cesty -> navrat vysledne barvy
		if(randoms[int(gl_FragCoord.x * gl_FragCoord.y * sample_ID) % 30] > (max(color.r, max(color.g, color.b))))
			break;	
						
		// Ziskani tangenty a binormaly v bode					
		calcTB(inter.normal, tangent, bitangent);
	
		if(inter.reflectivity == 0.0){
			float rand1 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.7 * sample_ID) % 30];
			float rand2 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 1.3 * sample_ID) % 30];
			vec3 refl_dir = randomDirection(rand1, rand2);
			refl_dir = convertToBNT(refl_dir, inter.normal, tangent, bitangent);

			ray.position = inter.position;
			ray.direction = refl_dir;
	
		}
		
		else{
			vec3 rand_offset = vec3((inter.disperse * randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.7 * sample_ID) % 30])-(inter.disperse/2), (inter.disperse * randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.9 * sample_ID) % 30])-(inter.disperse/2), (inter.disperse * randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.2 * sample_ID) % 30])-(inter.disperse/2));
			ray.position = inter.position + 0.001 * ray.direction;
			ray.direction = normalize((reflect(ray.direction, inter.normal))  + rand_offset);
		}

		depth += 1;
	}

	return ray_color;
}


void main(){

	trian = true;
	// Smer paprsku
	vec3 ray_dir = vec3(scr_coord - view_pos);
	ray_dir = normalize(ray_dir);

	// Originalni paprsek
	Ray ray;
	ray.position = view_pos;
	ray.direction = ray_dir;

	if(sample_ID < 2.0) discard;

	// Barva pixelu
	float alpha = 1.0 / (stride - 1.0);
	if(stride > 250) alpha = 0.0004;

	color = vec4(pathTrace(ray), alpha);
}
