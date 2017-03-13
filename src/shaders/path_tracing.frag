#version 450

#define MAX_DEPTH 4

#define LEFT -0.7
#define RIGHT 0.7
#define UP 0.7
#define DOWN -0.5
#define FRONT -0.9
#define BACK 0.9

#define DIFFUSE_MAT 0.1
#define SPECULAR_MAT 0.5
#define MIRROR_MAT 0.8

#define PI 3.1415926535

// Paprsek
struct Ray{
	vec3 position;
	vec3 direction;
};

// Koule
struct Sphere{
	vec3 center;
	float radius;
	vec3 color;
	float reflectivity;
};

// Trojuhelnik
struct Triangle{
	vec3 vertex0;
	vec3 vertex1;
	vec3 vertex2;
	vec3 normal;
	vec3 color;
	float reflectivity;
};

// Prusecik
struct Intersection{
	float dist;
	vec3 position;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	vec3 color;
	float reflectivity;
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
	float tx0; float ty0; float tz0;
	float tx1; float ty1; float tz1;
	int current; int node_id;
	bool init;
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


uniform Sphere spheres[4];
uniform float randoms[100];
uniform float sample_ID;
uniform float stride;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform int triangles_count;
uniform int nodes_count;
uniform int indices_count;

in vec3 scr_coord;

out vec4 color;

bool light_source = false;

float ambient = 0.1;
float diffuse = 0.9;

vec3 light_color = vec3(1.0);

Ray original_ray;

// Vygenerovani nahodneho smeru uvnitr polokoule na zaklade nahodnych cisel
vec3 randomDirection(float rand1, float rand2){
	float sinTheta = sqrt(1 - rand1 * rand1);
	float phi = 2 * PI * rand2;
	float x = sinTheta * cos(phi);
	float z = sinTheta * sin(phi);
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
	//if(t + 0.1 > distanc) return false;

	float b1 = dot(dis, s1) * invdiv;
	if (b1 < 0.0 || b1 > 1.0) return false;
	
	float b2 = dot(ray.direction, s2) * invdiv;
	if (b2 < 0.0 || (b1 + b2) > 1.0) return false;	

	inter.dist = t;
	inter.position = ray.position + t * ray.direction;
	//inter.normal = normalize(cross(e1, e2));
	inter.normal = tr.normal;
	inter.color = tr.color;
	inter.reflectivity = tr.reflectivity;

	return true;
}


// Prusecik paprsku s kouli
bool raySphereIntersection(Ray ray, Sphere sp, out Intersection inter){
	
	vec3 center = sp.center;
	float radius = sp.radius;

	float t0, t1;
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

	return true;
}


// Prusecik paprsku se stenami
bool rayBoxIntersection(Ray ray, out Intersection inter){
	
	bool res = false;
	float closest = 5.0;

	// Dolni stena
	float planeHeight;
	float t;
	vec3 point;
	//if(abs(t) < abs(closest)) closest = t;
	if(ray.direction.y < 0.0){
	planeHeight = DOWN;
	t = (planeHeight - ray.position.y) / ray.direction.y;
	point = ray.position + (t * ray.direction);
	if(point.z > FRONT && point.z < 2.0 && point.x > LEFT && point.x < RIGHT){
			if(abs(t) < abs(closest)) {
				closest = t;
				inter.dist = t;
				inter.position = point;
				inter.normal = vec3(0.0, 1.0, 0.0);
				inter.color = vec3(0.9, 0.8, 0.0);
				inter.color = vec3(0.99);
				//inter.color = vec3(0.9, 0.1, 0.1);
				inter.reflectivity = 0.1;
				
				res = true;	
			}
		}
	}
	
	// Horni stena
	if(ray.direction.y > 0.0){
		planeHeight = UP;
		t = (planeHeight - ray.position.y) / ray.direction.y;
		point = ray.position + (t * ray.direction);
	
		if(point.z > FRONT && point.z < 2.0 && point.x > LEFT && point.x < RIGHT){
			if(abs(t) < abs(closest)){ 
				closest = t;
				inter.dist = t;
				inter.position = point;
				inter.normal = vec3(0.0, -1.0, 0.0);
				//inter.color = vec3(0.9, 0.0, 0.3);
				//inter.color = vec3(0.9, 0.1, 0.1);
				inter.color = vec3(0.99);
				inter.reflectivity = 0.1;
			
				// Svetlo 
				if(point.z > (light_pos.z - 0.1) && point.z < (light_pos.z + 0.1) && point.x > (light_pos.x - 0.1) && point.x < (light_pos.x + 0.1))
					light_source = true;

				res = true;
			}
		}
	}

	// Leva stena
	if(ray.direction.x < 0.0){
		planeHeight = LEFT;
		t = (planeHeight - ray.position.x) / ray.direction.x;
		point = ray.position + (t * ray.direction);
	
		if(point.z > FRONT && point.z < 2.0 && point.y > DOWN && point.y < UP){
			if(t < closest){ 
				closest = t;
				inter.dist = t;
				inter.position = point;
				inter.normal = vec3(1.0, 0.0, 0.0);
				//inter.color = vec3(0.8, 0.99, 0.1);
				inter.color = vec3(0.6, 0.1, 0.8);
				inter.reflectivity = 0.1;
				res = true;	
			}
		}
	}

	// Prava stena
	if(ray.direction.x > 0.0){
		planeHeight = RIGHT;
		t = (planeHeight - ray.position.x) / ray.direction.x;
		point = ray.position + (t * ray.direction);
	
		if(point.z > FRONT && point.z < 2.0 && point.y > DOWN && point.y < UP){
			if(t < closest){
				closest = t;
				inter.dist = t;
				inter.position = point;
				inter.normal = vec3(-1.0, 0.0, 0.0);
				inter.color = vec3(0.1, 0.9, 0.1);
				inter.reflectivity = 0.1;
				res = true;	
			}
		}
	}

	// Predni stena
	if(ray.direction.z < 0.0){
		planeHeight = FRONT;
		t = (planeHeight - ray.position.z) / ray.direction.z;
		point = ray.position + (t * ray.direction);
	
		if(point.x > LEFT && point.x < RIGHT && point.y > DOWN && point.y < UP){
			if(t < closest){
				closest = t;
				inter.dist = t;
				inter.position = point;
				inter.normal = vec3(0.0, 0.0, 1.0);
				inter.color = vec3(0.99);
				//inter.color = vec3(0.9, 0.1, 0.1);
				inter.reflectivity = 0.1;
				res = true;
			}
		}
	}

	return res;

}


// Octree

uint a;
float tx0, ty0, tz0, tx1, ty1, tz1;

// Prvni zasazeny uzel
int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm) {
	uint answer = 0;
							
	if (tx0 > ty0) {
		if (tx0 > tz0) { // PLANE YZ
			if (tym < tx0) answer |= 2;    
			if (tzm < tx0) answer |= 1;    
			return int(answer);
		}
	}
	else {
		if (ty0 > tz0) { // PLANE XZ
			if (txm < ty0) answer |= 4;    
			if (tzm < ty0) answer |= 1; 
			return int(answer);
		}
	}
	// PLANE XY
	if (txm < tz0) answer |= 4;    
	if (tym < tz0) answer |= 2;    
	return int(answer);
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
bool iterativeTraversal(Ray ray, int node, out Intersection inters) {
	int current;
	float txm, tym, tzm;
	bool init = false, res = false;
	OctreeContext context;
	OctreeContext states[10];
	float closest = 100.0;
	int sp = 0;
	int cycles = 0;

	do{
		cycles += 1;
		
		if (nodes[node].leaf == 1) {
			
				
			if (nodes[node].count != 0) {
	
				Intersection inter;
				//for(int i = 0; i < triangles_count; i++){
				for(int i = 0; i < nodes[node].count; i++){
					//if(rayTriangleIntersection(original_ray, triangles[i], inter)){
					if(rayTriangleIntersection(original_ray, triangles[indices[nodes[node].index + i]], inter)){	
						if(!res || inter.dist < inters.dist){
						//if(i == 12) abc = true;
							res = true;
							inters = inter;
						}
					}
				}

				if(res) return true;
			}

			sp -= 1;
			if (sp < 0) {
				break;
				//node_id = node;
				//return false;
			}

			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current;
			node = states[sp].node_id;
			init = states[sp].init;
		}

		
		if (tx1 < 0 || ty1 < 0 || tz1 < 0) {
			sp -= 1;
			if (sp < 0) {
				break;
				//node_id = node;
				//return false;
			}
			tx0 = states[sp].tx0;
			ty0 = states[sp].ty0;
			tz0 = states[sp].tz0;
			tx1 = states[sp].tx1;
			ty1 = states[sp].ty1;
			tz1 = states[sp].tz1;
			current = states[sp].current;
			node = states[sp].node_id;
			init = states[sp].init;
		}


		txm = 0.5f * (tx0 + tx1);
		tym = 0.5f * (ty0 + ty1);
		tzm = 0.5f * (tz0 + tz1);


		if (!init) {
			init = !init;
			current = firstNode(tx0, ty0, tz0, txm, tym, tzm);
		}


		context.tx0 = tx0;
		context.ty0 = ty0;
		context.tz0 = tz0;
		context.tx1 = tx1;
		context.ty1 = ty1;
		context.tz1 = tz1;
		context.node_id = node;
		context.init = true;
		init = false;

		switch (current) {
		case 0:
			node = nodes[node].childs[a];
			context.current = nextNode(txm, 4, tym, 2, tzm, 1);
			tx1 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 1:
			node = nodes[node].childs[1 ^ a];
			context.current = nextNode(txm, 5, tym, 3, tz1, 8);
			tx1 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 2:
			node = nodes[node].childs[2 ^ a];
			context.current = nextNode(txm, 6, ty1, 8, tzm, 3);
			tx1 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 3:
			node = nodes[node].childs[3 ^ a];
			context.current = nextNode(txm, 7, ty1, 8, tz1, 8);
			tx1 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 4:
			node = nodes[node].childs[4 ^ a];
			context.current = nextNode(tx1, 8, tym, 6, tzm, 5);
			tx0 = txm; ty1 = tym; tz1 = tzm;
			break;

		case 5:
			node = nodes[node].childs[5 ^ a];
			context.current = nextNode(tx1, 8, tym, 7, tz1, 8);
			tx0 = txm; ty1 = tym; tz0 = tzm;
			break;

		case 6:
			node = nodes[node].childs[6 ^ a];
			context.current = nextNode(tx1, 8, ty1, 8, tzm, 7);
			tx0 = txm; ty0 = tym; tz1 = tzm;
			break;

		case 7:
			node = nodes[node].childs[7 ^ a];
			context.current = 8;
			tx0 = txm; ty0 = tym; tz0 = tzm;
			break;

		case 8:
			do {
			
				cycles += 1;

				sp -= 1;
				if (sp < 0) {
					//result = -1;
					//node_id = node;
					return false;
				}

				tx0 = states[sp].tx0;
				ty0 = states[sp].ty0;
				tz0 = states[sp].tz0;
				tx1 = states[sp].tx1;
				ty1 = states[sp].ty1;
				tz1 = states[sp].tz1;
				current = states[sp].current;
				node = states[sp].node_id;

				txm = 0.5f * (tx0 + tx1);
				tym = 0.5f * (ty0 + ty1);
				tzm = 0.5f * (tz0 + tz1);

				switch (current) {

				case 0:
					node = nodes[node].childs[a];
					context.current = nextNode(txm, 4, tym, 2, tzm, 1);
					tx1 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 1:
					node = nodes[node].childs[1 ^ a];
					context.current = nextNode(txm, 5, tym, 3, tz1, 8);
					tx1 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 2:
					node = nodes[node].childs[2 ^ a];
					context.current = nextNode(txm, 6, ty1, 8, tzm, 3);
					tx1 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 3:
					node = nodes[node].childs[3 ^ a];
					context.current = nextNode(txm, 7, ty1, 8, tz1, 8);
					tx1 = txm; ty0 = tym; tz0 = tzm;
					break;

				case 4:
					node = nodes[node].childs[4 ^ a];
					context.current = nextNode(tx1, 8, tym, 6, tzm, 5);
					tx0 = txm; ty1 = tym; tz1 = tzm;
					break;

				case 5:
					node = nodes[node].childs[5 ^ a];
					context.current = nextNode(tx1, 8, tym, 7, tz1, 8);
					tx0 = txm; ty1 = tym; tz0 = tzm;
					break;

				case 6:
					node = nodes[node].childs[6 ^ a];
					context.current = nextNode(tx1, 8, ty1, 8, tzm, 7);
					tx0 = txm; ty0 = tym; tz1 = tzm;
					break;

				case 7:
					node = nodes[node].childs[7 ^ a];
					context.current = 8;
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
	a = 0;

	original_ray.direction = ray.direction;
	original_ray.position = ray.position;
	Ray tmp_ray;
	tmp_ray.position = ray.position;
	tmp_ray.direction = ray.direction;

	if (ray.direction.x < 0.0) {
		tmp_ray.position.x = (RIGHT + LEFT) - ray.position.x;
		tmp_ray.direction.x *= -1.0;
		a |= 4;
	}

	if (ray.direction.y < 0.0) {
		tmp_ray.position.y = (UP + DOWN) - ray.position.y;
		tmp_ray.direction.y *= -1.0;
		a |= 2;
	}

	if (ray.direction.z > 0.0) {
		tmp_ray.position.z = (BACK + FRONT) - ray.position.z;
		tmp_ray.direction.z *= -1.0;
		a |= 1;
	}

	/*if (ray.direction.x == 0.0) tmp_ray.direction.x = 0.000000000001f;
	if (ray.direction.y == 0.0) tmp_ray.direction.y = 0.000000000001f;
	if (ray.direction.z == 0.0) tmp_ray.direction.z = -0.000000000001f;*/

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

	if(max(tx0, max(ty0, tz0)) < min(tx1, min(ty1, tz1)))
		if(iterativeTraversal(tmp_ray, 0, inter)){
			inters = inter;
			return true;
		}

	return false;
}

vec3 cll;

// Spocitani nejblizsi kolize paprsku se scenou
bool calculCollision(Ray ray, out Intersection inter, bool shadow){

	bool intersect = false;
	Intersection coll;

	int sphere_cnt = 4;
	
	// Steny
	if(!shadow){
		if(rayBoxIntersection(ray, coll)){
			intersect = true;
			inter = coll;
		}
	}

	// Koule
	if(ray.position.y < light_pos.y){
		for(int i = 0; i < sphere_cnt; i++){
			if(raySphereIntersection(ray, spheres[i], coll)){
				if(!intersect || coll.dist < inter.dist){
					intersect = true;
					inter = coll;
				}
			}
		}
	}

	// Octree
	if(octreeTraversal(ray, coll)){
		if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
		}
	}
	
	// Trojuhelniky
	/*for(int i = 0; i < triangles_count; i++){
		if(rayTriangleIntersection(ray, triangles[i], coll)){
			if(!intersect || coll.dist < inter.dist){
				//if(i == 12) abc = true;
				intersect = true;
				inter = coll;
			}
		}
	}*/
		
	return intersect;
}


vec3 directLight(vec3 position, vec3 normal, vec3 color){
  vec3 direct = vec3(0.0);
  vec3 light_dir = normalize(light_pos - position);

  Ray light_ray;
  light_ray.position = position;
  light_ray.direction = light_dir;

  Intersection inter;
  bool shadow = calculCollision(light_ray, inter, true);
  direct = (ambient * color) + ((shadow? min(inter.dist * 4.9, 0.1) : 1) * max(dot(light_dir, normal), 0.0) * color * diffuse * light_color);
  
  //if(shadow) direct = vec3(1, 1, 0);

  return direct;
}


vec3 pathTrace(Ray original_ray){

	Ray ray = original_ray;
	vec3 ray_color = vec3(1.0);
	vec3 point_color;
	vec3 albedo;
	vec3 tangent, bitangent;
	Intersection inter;
	int depth = 0;
	float coef = 1.0;

	while (true) {
		
		if(!calculCollision(ray, inter, false) && depth != 0){
			if(coef > 0.3){
				ray_color = vec3(0.2);
				break;
			}
			else if(coef > 0.1){
				ray_color = vec3(0.7);
				break;
			}
			/*else {
				ray_color *= 1.9;
				break;
			}*/
		}

		// Kolize se zdrojem svetla -> bila barva
		if(light_source){
			ray_color = light_color;
			break;
		}

		// Maximalni hloubka cesty -> navrat vysledne barvy
		if(depth >= MAX_DEPTH)
			break;

		// Difuzni material
		if(inter.reflectivity == DIFFUSE_MAT){
			vec3 color = inter.color * coef * (1.0 - inter.reflectivity);
			coef *= inter.reflectivity;
		
			if(depth == 0)
				albedo = color;

			// Osvetleni v bode
			point_color = directLight(inter.position, inter.normal, inter.color);

			// barva * BRDF(difuzni)
			ray_color = (ray_color / PI + 2 * point_color) * albedo;

			// Konec cesty -> navrat vysledne barvy
			if(randoms[int(gl_FragCoord.x * gl_FragCoord.y * sample_ID) % 100] > (max(color.r, max(color.g, color.b)))){
				ray_color *= 1.12;
				break;	
			}			

			// Dalsi odraz
			if(inter.tangent == inter.bitangent){
				calcTB(inter.normal, tangent, bitangent);
				inter.tangent = tangent;
				inter.bitangent = bitangent;
			}
  
			// Vygenerovani nahodneho smeru odrazu
			float rand1 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.7 * sample_ID) % 100];
			float rand2 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 1.3 * sample_ID) % 100];
			vec3 refl_dir = randomDirection(rand1, rand2);
			refl_dir = convertToBNT(refl_dir, inter.normal, inter.tangent, inter.bitangent);

			ray.position = inter.position;
			ray.direction = refl_dir;
		}

		// Reflexni material
		else if(inter.reflectivity == MIRROR_MAT){
			vec3 color = inter.color * inter.reflectivity;
			coef *= inter.reflectivity;

			if(depth == 0)
				albedo = color;

			// Paprsek odrazeny podle normaly
			ray.position = inter.position + 0.001 * ray.direction;
			ray.direction = normalize(reflect(ray.direction, inter.normal));
		}

		else if(inter.reflectivity == SPECULAR_MAT){
			vec3 color = inter.color;
			coef *= 0.1;

			Intersection ii;
			ray.position = inter.position + 0.001 * ray.direction;
			ray.direction = reflect(ray.direction, inter.normal);
			calculCollision(ray, ii, false);

			// Odlesk od svetla
			if(ii.position.x <= (light_pos.x + 0.1) && ii.position.x >= (light_pos.x - 0.1) && ii.position.y == UP){
				if(ii.position.z <= (light_pos.z + 0.1) && ii.position.z >= (light_pos.z - 0.1))
					return light_color;
			}

			if(depth == 0)
				albedo = color;

			// Osvetleni v bode
			point_color = directLight(inter.position, inter.normal, inter.color);

			// barva * BRDF
			ray_color = (ray_color / PI + 2.0 * point_color) * albedo;
			// barva odrazu
			ray_color += 0.3 * directLight(ii.position, ii.normal, ii.color);;

			// Konec cesty -> navrat vysledne barvy
			if(randoms[int(gl_FragCoord.x * gl_FragCoord.y * sample_ID) % 100] > (max(color.r, max(color.g, color.b)))){
				ray_color *= 1.17;
				break;
			}

			// Dalsi odraz
			calcTB(inter.normal, tangent, bitangent);
  
			// Vygenerovani nahodneho smeru odrazu
			float rand1 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 2.7 * sample_ID) % 100];
			float rand2 = randoms[int(gl_FragCoord.x * gl_FragCoord.y * 1.3 * sample_ID) % 100];
			vec3 refl_dir = randomDirection(rand1, rand2);
			refl_dir = convertToBNT(refl_dir, inter.normal, tangent, bitangent);

			ray.position = inter.position;
			ray.direction = refl_dir;

		}

		depth += 1;
	}

	return ray_color;
}


void main(){

	// Smer paprsku
	vec3 ray_dir = vec3(scr_coord - view_pos);
	ray_dir = normalize(ray_dir);

	// Originalni paprsek
	Ray ray;
	ray.position = view_pos;
	ray.direction = ray_dir;

	if(sample_ID < 2.0) discard;

	// Barva pixelu
	float x = 1.0 / (stride - 1.0);
	color = vec4(pathTrace(ray), x);
}
