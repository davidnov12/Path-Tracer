#version 450

#define MAX_DEPTH 4

#define LEFT -0.7
#define RIGHT 0.7
#define UP 0.7
#define DOWN -0.52
#define FRONT -0.9

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


layout (std430, binding = 0) buffer geometry{
    Triangle triangles[];
};


uniform Sphere spheres[4];
uniform float randoms[100];
uniform float sample_ID;
uniform float stride;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform int triangles_count;

in vec3 scr_coord;

out vec4 color;

bool light_source = false;

float ambient = 0.2;
float diffuse = 0.8;

vec3 light_color = vec3(1.0);


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
bool rayTriangleIntersection(Ray ray, Triangle tr, out Intersection inter, float distanc){
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

	if(t0 < 0.0001) return false;

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
				inter.color = vec3(0.9, 0.0, 0.3);
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
				inter.color = vec3(0.8, 0.99, 0.1);
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
				inter.reflectivity = 0.1;
			
				res = true;
			}
		}
	}
	
	return res;

}


// Spocitani nejblizsi kolize paprsku se scenou
bool calculCollision(Ray ray, out Intersection inter, bool shadow){

	bool intersect = false;
	Intersection coll;

	inter.tangent = inter.bitangent = vec3(0.0);

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
	
	// Trojuhelniky
	for(int i = 0; i < triangles_count; i++){
		if(rayTriangleIntersection(ray, triangles[i], coll, inter.dist)){
			if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
			}
		}
	}

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
  direct = (ambient * color) + ((shadow? min(inter.dist * 4.9, 0.4) : 1) * max(dot(light_dir, normal), 0.0) * color * diffuse * light_color);
  
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
