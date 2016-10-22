/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* Camera.cpp - trida kamery
*/

#include "Camera.h"

Camera::Camera(vec3 pos, double width, double height){
	this->width = width;
	this->height = height;
	position = pos;
	forward = vec3(0.0, 0.0, -1.0);
	worldUp = vec3(0.0, 1.0, 0.0);
	updateVectors();
}

mat4 Camera::getViewMat(){
	return lookAt(position, position + vec3(0.0, 0.0, -1.0), up);
}

mat4 Camera::getProjMat(){
	return perspective(FOV, width / height, 0.1, 1000.0);
}

vec3 Camera::getPosition(){
	return position;
}

void Camera::updateVectors(){
	/*vec3 front;
	front.x = cos(glm::radians(YAW)) * cos(glm::radians(PITCH));
	front.y = sin(glm::radians(PITCH));
	front.z = sin(glm::radians(YAW)) * cos(glm::radians(PITCH));
	*/

	/*float radx = radians(PITCH);
	float rady = radians(YAW);
	float x = cos(rady) * cos(radx);
	float y = sin(rady);
	float z = cos(rady) * sin(radx);*/

	//position = vec3(x, x, z);
	up = vec3(0.0, 1.0, 0.0);



	/*
	forward = normalize(front);
	right = normalize(cross(forward, worldUp));
	up = normalize(cross(right, forward));
*/}
