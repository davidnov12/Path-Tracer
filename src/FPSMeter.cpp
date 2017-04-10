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
	glGenQueries(2, queryID);
	first = true;
	total = 0;
}

void FPSMeter::startTask(){
	glQueryCounter(queryID[0], GL_TIMESTAMP);
}

void FPSMeter::stopTask(){
	glQueryCounter(queryID[1], GL_TIMESTAMP);
}

void FPSMeter::gainFPS(){
	GLint stopTimerAvailable = 0;
	while (!stopTimerAvailable) {
		glGetQueryObjectiv(queryID[1], GL_QUERY_RESULT_AVAILABLE, &stopTimerAvailable);
	}

	glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &start);
	glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &finish);

	if (first) {
		total += start;
		first = false;
	}
	else
		total += finish - start;
}

float FPSMeter::getFPS(){
	return 1000000000.0 / (finish - start);
}

float FPSMeter::getRenderingTime(){
	return (finish - start) / 1000000.0;
}

float FPSMeter::getTotalTime(){
	return total / 1000000000.0;
}

void FPSMeter::refreshTotalTime() {
	total = 0;
}
