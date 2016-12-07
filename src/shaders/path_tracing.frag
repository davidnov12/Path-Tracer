#version 450

#define MAX_BOUNCES 4

uniform vec3 light_pos;
uniform vec3 view_pos;
uniform mat4 inv_viewproj;


in vec3 scr_coord;

out vec4 color;

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

// Prusecik
struct Intersection{
	float dist;
	vec3 position;
	vec3 normal;
	vec3 color;
	float reflectivity;
};

int samples_per_pixel = 256;

uniform Sphere spheres[2];
bool light_source = false;

float ambient = 0.2;
float diffuse = 0.8;

#define LEFT -0.7
#define RIGHT 0.7
#define UP 0.52
#define DOWN -0.52
#define FRONT -1.4

#define X 0
#define Y 1
#define Z 2

#define PI 3.1415926535

//--Generator nahodnych cisel-----------------
//--------------------------------------------
uint seed;


// Priprava seedu
void seedInit(){
	seed = gl_SampleID;
	seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
}


// Generator pseudonahodnych cisel
uint randXorshift(){
    seed ^= (seed << 13);
    seed ^= (seed >> 17);
    seed ^= (seed << 5);
    return seed;
}


// Prevedeni cisla do rozsahu <0, 1>
float clampNumber(uint num){
	return float(num) / 4294967296.0;
}

//--------------------------------------------
//--------------------------------------------

//--Funkce pro indirect illumination----------
//--------------------------------------------
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
	
	if(abs(normal.x) > abs(normal.y))
		tangent = vec3(normal.z, 0.0, -normal.x) / sqrt(normal.x * normal.x + normal.z * normal.z);
	else
		tangent = vec3(0.0, -normal.z, normal.y) / sqrt(normal.y * normal.y + normal.z * normal.z);

	tangent = normalize(tangent);

	bitangent = cross(normal, tangent);
	bitangent = normalize(bitangent);
}


// Prevedni vygenerovaneho smeru do lokalniho souradneho systemu v bode kolize
vec3 convertToBNT(vec3 original, vec3 normal, vec3 tangent, vec3 bitangent){
	mat3 trans = mat3(bitangent, normal, tangent);
	vec3 dir = trans * original;
	return dir;	
}

//--------------------------------------------
//--------------------------------------------


//--Pruseciky---------------------------------
//--------------------------------------------

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

	inter.dist = t0;
	inter.position = ray.position + (t0 * ray.direction);
	inter.normal = normalize(inter.position - center);
	inter.color = sp.color;
	inter.reflectivity = sp.reflectivity;

	return true;
}

// Prusecik paprsku s rovinou
bool rayBoxIntersection(Ray ray, out Intersection inter){
	
	// Dolni stena
	float planeHeight = DOWN;
	float t = (planeHeight - ray.position.y) / ray.direction.y;
	vec3 point = ray.position + (t * ray.direction);
	if(point.z > FRONT && point.z < 1.0 && point.x > LEFT && point.x < RIGHT){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, 1.0, 0.0);
			inter.color = vec3(0.8);
			inter.reflectivity = 0.1;

			return true;	
	}
	
	// Horni stena
	planeHeight = UP;
	t = (planeHeight - ray.position.y) / ray.direction.y;
	point = ray.position + (t * ray.direction);

	if(point.z > FRONT && point.z < 1.0 && point.x > LEFT && point.x < RIGHT){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, -1.0, 0.0);
			inter.color = vec3(0.8);
			inter.reflectivity = 0.1;
			
			// Svetlo 
			if(point.z > -0.13 && point.z < 0.13 && point.x > -0.13 && point.x < 0.13)
				light_source = true;

			return true;
	}

	// Leva stena
	planeHeight = LEFT;
	t = (planeHeight - ray.position.x) / ray.direction.x;
	point = ray.position + (t * ray.direction);

	if(point.z > FRONT && point.z < 1.0 && point.y > DOWN && point.y < UP){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(1.0, 0.0, 0.0);
			//inter.color = vec3(0.6, 1.0, 0.0);
			inter.color = vec3(0.6, 1.0, 0.0);
			inter.reflectivity = 0.1;

			return true;	
	}

	// Prava stena
	planeHeight = RIGHT;
	t = (planeHeight - ray.position.x) / ray.direction.x;
	point = ray.position + (t * ray.direction);

	if(point.z > FRONT && point.z < 1.0 && point.y > DOWN && point.y < UP){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(-1.0, 0.0, 0.0);
			//inter.color = vec3(0.6, 1.0, 0.0);
			inter.color = vec3(0.6, 1.0, 0.0);
			inter.reflectivity = 0.1;

			return true;	
	}

	// Predni stena
	planeHeight = FRONT;
	t = (planeHeight - ray.position.z) / ray.direction.z;
	point = ray.position + (t * ray.direction);

	if(point.x > LEFT && point.x < RIGHT && point.y > DOWN && point.y < UP){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, 0.0, 1.0);
			inter.color = vec3(0.8);
			inter.reflectivity = 0.1;
			
			return true;	
	}

	return false;
}


// Spocitani nejblizsi kolize paprsku se scenou
bool calculCollision(Ray ray, out Intersection inter, bool shadow){

	bool intersect = false;
	Intersection coll;

	int sphere_cnt = 2;
	if(!shadow){
		if(rayBoxIntersection(ray, coll)){
			intersect = true;
			inter = coll;
		}
	}

	for(int i = 0; i < sphere_cnt; i++){
		if(raySphereIntersection(ray, spheres[i], coll)){
			if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
			}
		}
	}

	return intersect;
}

//--------------------------------------------
//--------------------------------------------

int SPP = 4;

vec3 directLight(vec3 position, vec3 normal, vec3 color){
  vec3 direct = vec3(0.0);
  vec3 light_dir = normalize(light_pos - position);

  Ray light_ray;
  light_ray.position = position;
  light_ray.direction = light_dir;

  Intersection inter;
  bool shadow = calculCollision(light_ray, inter, true);
  direct = (ambient * color) + ((shadow? 0 : 1) * max(dot(light_dir, normal), 0.0) * color * diffuse);

  return direct;
}


vec3 indirectLight(vec3 position, vec3 normal, out vec3 collision, out vec3 coll_normal){
  vec3 indirect = vec3(0.0);

  vec3 tang, bitang;
  calcTB(normal, tang, bitang);
  float pdf = 1 / (2 * PI);

  float rand1 = clampNumber(randXorshift());
  float rand2 = clampNumber(randXorshift());

  vec3 refl_dir = randomDirection(rand1, rand2);
  refl_dir = convertToBNT(refl_dir, normal, tang, bitang);

  Ray reflected;
  reflected.position = position;
  reflected.direction = refl_dir;

  Intersection inters;
  calculCollision(reflected, inters, false);
  vec3 int_color = inters.color;
  vec3 int_normal = inters.normal;

  collision = inters.position;
  coll_normal = inters.normal;

  indirect += directLight(inters.position, inters.normal, inters.color);
  indirect = indirect / pdf * rand1 ;

  return indirect;
}



vec3 rayTrace(Ray first_ray){

	vec3 ray_color = vec3(0.0);
	float coef = 1.0;
	Intersection inter;
	int bounces = 0;
	Ray ray = first_ray;

		if(calculCollision(ray, inter, false)){
			if(light_source) return vec3(1.0);

			color = color * coef * (1.0 - inter.reflectivity);
			coef *= inter.reflectivity;

			ray_color += directLight(inter.position, inter.normal, inter.color);

			vec3 indirect_color = vec3(0.0);
			vec3 in_pos = inter.position, in_norm = inter.normal, out_pos, out_norm;
			seed = gl_SampleID;
			for(int i = 0; i < SPP; i++){
				indirect_color += indirectLight(in_pos, in_norm, out_pos, out_norm);
			}
			indirect_color /= SPP;
			ray_color = (ray_color / PI + 2 * indirect_color) * inter.color;
			//ray_color = ray_color + indirect_color * 0.56;
		}

		else{
			bounces = MAX_BOUNCES;
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
	
	// Barva pixelu
	color = vec4(rayTrace(ray), 1.0);	
}
