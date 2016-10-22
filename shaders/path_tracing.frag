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
};

// Prusecik
struct Intersection{
	float dist;
	vec3 position;
	vec3 normal;
	vec3 color;
};

uniform Sphere spheres[2];


float ambient = 0.2;
float diffuse = 0.8;

// Spocitani pruseciku paprsek-rovina
bool rayPlane(Ray ray, out Intersection inter){

	//float dist = -(ray.position.y - height) / ray.direction.y;
	//vec3 coll = ray.position + (ray.direction * dist);

	float d = -1 / (ray.direction.z + ray.position.z );

	float x = ray.position.x + (d * ray.direction.x);
	float y = ray.position.y + (d * ray.direction.y);

	if(x >= -1.0 && x <= 1.0 && y >= -1.0 && y <= 1.0){
		inter.dist = d;
		inter.position = vec3(x, y, -1.0);
		inter.normal = vec3(0.0, 0.0, 1.0);
		inter.color = vec3(0.0, 0.7, 0.4);

		return true;
	}

	/*if(dist > 0 && coll.x > -size && coll.z > -size  && coll.x < size && coll.z < size){
		inter.dist = dist;
		inter.position = coll;
		inter.normal = ray.position.y > height? vec3(0, 1, 0) : vec3(0, -1, 0);
		inter.color = vec3(0.0, 0.7, 0.4);

		return true;
	}*/

	return false;
}

// Spocitani pruseciku paprsek-koule
bool raySphere(Ray ray, Sphere sphere, out Intersection inter){

	vec3 oc = ray.position - sphere.center;
	float b = 2 * dot(ray.direction, oc);
	float c = dot(oc, oc) - (sphere.radius * sphere.radius);
	float d = b*b - 4*c;

	if(d < 0)
		return false;

	float sd = sqrt(d);
	float t1 = (-b + sd)/2;
	float t2 = (-b - sd)/2;

	if(t1 <= 0 && t2 <= 0)
		return false;

	inter.dist = (t1 > 0 && t1 < t2)? t1 : t2;
	inter.position = ray.position + inter.dist * ray.direction;
	inter.normal = normalize(inter.position - sphere.center);
	inter.color = sphere.color;

	return true;
}

// Spocitani nejblizsi kolize paprsku se scenou
bool calcCollision(Ray ray, out Intersection inter){

	bool intersect = false;
	Intersection coll;

	/*if(rayPlane(ray, 10, -2, coll)){
		intersect = true;
		inter = coll;
	}*/

	if(rayPlane(ray, coll)){
		intersect = true;
		inter = coll;
	}

	/*int sphere_cnt = 2;

	for(int i = 0; i < sphere_cnt; i++){
		if(raySphere(ray, spheres[i], coll)){
			if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
			}
		}
	}*/

	return intersect;
}

// Sledovani paprsku
vec3 rayTrace(Ray first_ray){

	vec3 ray_color = vec3(0.0);
	Intersection inter;
	int bounces = 0;
	Ray ray = first_ray;

	while(bounces < MAX_BOUNCES){
		if(calcCollision(ray, inter)){

			// Vektor ke svetlu
			vec3 light_dir = normalize(light_pos - inter.position);

			// Stinovy paprsek
			Ray light_ray = Ray(inter.position, light_dir);
			light_ray.position += 0.001 * light_ray.direction;

			// Novy paprsek
			ray = Ray(inter.position, reflect(ray.direction, inter.normal));
			ray.position += 0.001 * ray.direction;

			// Osvetleni v bode pruseciku
			bool shadow = calcCollision(light_ray, inter);
			ray_color += (ambient * inter.color) + ((shadow? 0 : 1) * max(dot(light_dir, inter.normal), 0.0) * inter.color * diffuse);

			bounces += 1;
		}

		else{
			bounces = MAX_BOUNCES;
			ray_color = vec3(0.8, 0.4, 0.2);
		}
	}

	return ray_color;
}


bool testCollision(Ray r){
	vec3 center = vec3(-0.3, 0.0, -0.4);
	float radius = 0.07;
	float t0, t1;
	vec3 L = center - r.position;
	float tca = dot(L, r.direction);
	if (tca < 0) return false;
	float d2 = dot(L, L) - tca * tca;
	if (d2 > radius) return false;
	float thc = sqrt(radius - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	vec3 coll = r.position + (t0 * r.direction);
	vec3 normal = coll - center;
	normal = normalize(normal);
	vec3 lightr = light_pos - coll;
	lightr = normalize(lightr);

	vec3 ambient = 0.2 * vec3(0.7, 0.3, 0.6);
	vec3 diffuse = 0.8 * max(dot(lightr, normal), 0.0) * vec3(0.7, 0.3, 0.6);

	vec3 res = ambient + diffuse;

	color = vec4(res, 1.0);

	return true;
}

bool sphere2(Ray r){
	vec3 center = vec3(0.4, 0.0, -0.4);
	float radius = 0.1;
	float t0, t1;
	vec3 L = center - r.position;
	float tca = dot(L, r.direction);
	if (tca < 0) return false;
	float d2 = dot(L, L) - tca * tca;
	if (d2 > radius) return false;
	float thc = sqrt(radius - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	vec3 coll = r.position + (t0 * r.direction);
	vec3 normal = coll - center;
	normal = normalize(normal);
	vec3 lightr = light_pos - coll;
	lightr = normalize(lightr);

	vec3 ambient = 0.2 * vec3(0.3, 0.8, 0.5);
	vec3 diffuse = 0.8 * max(dot(lightr, normal), 0.0) * vec3(0.3, 0.8, 0.5);

	vec3 res = ambient + diffuse;

	color = vec4(res, 1.0);

	return true;
}


void main(){

	// Smer paprsku
	vec3 ray_dir = vec3(scr_coord.x - view_pos.x, scr_coord.y - view_pos.y,  scr_coord.z - view_pos.z);
	ray_dir = normalize(ray_dir);

	// Originalni paprsek
	Ray ray;
	ray.position = view_pos;
	ray.direction = ray_dir;

	//color = vec4(ray_dir, 1.0);
	
	if(testCollision(ray)); //color = vec4(0.7, 0.3, 0.6, 1.0);
	else if(sphere2(ray));
	else color = vec4(0.0);
		

	/*
	if(mod(gl_FragCoord.x, 2.0) > 0.5) color = vec4(0.3, 0.8, 0.2, 1.0);
	else color = vec4(0.3, 0.1, 0.8, 1.0);
	*/
	//color = vec4(0.3, 0.1, 0.8, 1.0);
}
