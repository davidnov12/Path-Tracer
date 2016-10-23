/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* FPSMeter.cpp - meric FPS
*/

#include "FPSMeter.h"

FPSMeter::FPSMeter(){
	time = 0.0;
	frames = 0.0;
}

bool FPSMeter::checkFPS(double currentTime){
	if (currentTime - time >= 1.0) return true;
	frames += 1.0;
	return false;
}

void FPSMeter::reinit(double time){
	this->time = time;
	frames = 0.0;
}

float FPSMeter::getFPS(){
	return frames;
}
