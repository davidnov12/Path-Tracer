#version 450

#define MAX_BOUNCES 2

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

Sphere spheres[2];

bool lightD = false;

float ambient = 0.2;
float diffuse = 0.8;


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
bool rayPlaneIntersection(Ray ray, out Intersection inter){
	
	// Horni Dolni
	float planeHeight = -0.42;
	float t = (planeHeight - ray.position.y) / ray.direction.y;
	vec3 point = ray.position + (t * ray.direction);

	if(point.z > -1.4 && point.z < 1.0 && point.x > -0.75 && point.x < 0.75){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, 1.0, 0.0);
			inter.color = vec3(0.85);
			inter.reflectivity = 0.28;

			return true;	
	}


	planeHeight = 0.55;
	t = (planeHeight - ray.position.y) / ray.direction.y;
	point = ray.position + (t * ray.direction);

	if(point.z > -1.4 && point.z < 1.0 && point.x > -0.75 && point.x < 0.75){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, -1.0, 0.0);
			inter.color = vec3(0.85);
			inter.reflectivity = 0.28;
			

			if(point.z > -0.09 && point.z < 0.09 && point.x > -0.09 && point.x < 0.09){
				lightD = true;
				
			}

			return true;
	}

	// Bocni
	planeHeight = -0.75;
	t = (planeHeight - ray.position.x) / ray.direction.x;
	point = ray.position + (t * ray.direction);

	if(point.z > -1.4 && point.z < 1.0 && point.y > -0.42 && point.y < 0.55){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(1.0, 0.0, 0.0);
			//inter.color = vec3(0.6, 0.6, 0.2);
			//inter.color = vec3(0.9, 0.0, 0.45);
			inter.color = vec3(0.6, 1.0, 0.0);
			inter.reflectivity = 0.19;

			return true;	
	}

	planeHeight = 0.75;
	t = (planeHeight - ray.position.x) / ray.direction.x;
	point = ray.position + (t * ray.direction);

	if(point.z > -1.4 && point.z < 1.0 && point.y > -0.42 && point.y < 0.55){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(-1.0, 0.0, 0.0);
			inter.color = vec3(0.6, 1.0, 0.0);
			inter.reflectivity = 0.19;

			return true;	
	}

	// Predni
	planeHeight = -1.4;
	t = (planeHeight - ray.position.z) / ray.direction.z;
	point = ray.position + (t * ray.direction);

	if(point.x > -0.75 && point.x < 0.75 && point.y > -0.42 && point.y < 0.55){
			inter.dist = t;
			inter.position = point;
			inter.normal = vec3(0.0, 0.0, 1.0);
			inter.color = vec3(0.85);
			inter.reflectivity = 0.12;
			
			return true;	
	}

	return false;
}


// Spocitani nejblizsi kolize paprsku se scenou
bool calculCollision(Ray ray, out Intersection inter, bool sh){

	bool intersect = false;
	Intersection coll;

	int sphere_cnt = 2;
	if(!sh){
	if(rayPlaneIntersection(ray, coll)){
		intersect = true;
		inter = coll;
	}
	}

	for(int i = 0; i < sphere_cnt; i++){
		if(raySphereIntersection(ray, spheres[i], coll)){
		//if(raySphere(ray, spheres[i], coll)){
			if(!intersect || coll.dist < inter.dist){
				intersect = true;
				inter = coll;
			}
		}
	}

	return intersect;
}


// Sledovani paprsku
vec3 rayTrace(Ray first_ray){

	vec3 ray_color = vec3(0.0);
	float coef = 1.0;
	Intersection inter;
	int bounces = 0;
	Ray ray = first_ray;

	while(bounces < MAX_BOUNCES && coef > 0.1){
		if(calculCollision(ray, inter, false)){
			if(lightD) return vec3(1.0);

			// Vektor ke svetlu
			vec3 light_dir = normalize(light_pos - inter.position);

			// Stinovy paprsek
			Ray light_ray;
			light_ray.position = inter.position;
			light_ray.direction = light_dir;
			//light_ray.position += 0.001 * light_ray.direction;

			//inter.color = inter.color * coef * (1.0 - inter.reflectivity);
			vec3 color = inter.color;
			vec3 normal = inter.normal;
			color = color * coef * (1.0 - inter.reflectivity);

			coef *= inter.reflectivity;
			// Novy paprsek
			ray = Ray(inter.position, reflect(ray.direction, inter.normal));
			ray.position += 0.001 * ray.direction;

			// Osvetleni v bode pruseciku
			//bool shadow = false;
			/*Intersection n;
			n.position = inter.position;
			n.dist = inter.dist;*/
			bool shadow = calculCollision(light_ray, inter, true);
			ray_color += (0.2 * color) + ((shadow? 0 : 1) * max(dot(light_dir, normal), 0.0) * color * 0.8);

			bounces += 1;
		}

		else{
			bounces = MAX_BOUNCES;
			//ray_color = vec3(0.8, 0.4, 0.2);
		}
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

	//color = vec4(ray_dir, 1.0);
	spheres[0].center = vec3(-0.15, -0.1777, -0.45);
	spheres[0].radius = 0.037;
	spheres[0].color = vec3(0.6, 1.0, 0.0);
	//spheres[0].color = vec3(0.7, 0.3, 0.6);
	//spheres[0].color = vec3(0.94);
	spheres[0].reflectivity = 0.1;

	spheres[1].center = vec3(0.2, -0.2, -0.12);
	spheres[1].radius = 0.06;
	spheres[1].color = vec3(0.6, 1.0, 0.0);
	//spheres[1].color = vec3(0.3, 0.8, 0.5);
	//spheres[1].color = vec3(0.94);
	spheres[1].reflectivity = 0.1;

	//Intersection inter;
	
	color = vec4(rayTrace(ray), 1.0);

	/*if(calculCollision(ray, inter)){
		vec3 lightr = light_pos - inter.position;
		lightr = normalize(lightr);

		Ray lightray;
		lightray.position = inter.position;
		lightray.direction = lightr;

		Intersection n;

		bool shadow = calculCollision(lightray, n);

		vec3 ambient = 0.2 * inter.color;
		vec3 diffuse = (shadow ? 0 : 1) * 0.8 * max(dot(lightr, inter.normal), 0.0) * inter.color;

		vec3 res = ambient + diffuse;

		color = vec4(res, 1.0);
	}
	
	else color = vec4(0.0);*/
	
}
