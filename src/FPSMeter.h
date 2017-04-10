/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * FPSMeter.h - meric FPS
 */

#pragma once

#include <GL/glew.h>

class FPSMeter {

public:

	FPSMeter();
	void startTask();
	void stopTask();
	void gainFPS();
	float getFPS();
	float getRenderingTime();
	float getTotalTime();
	void refreshTotalTime();

private:

	GLuint64 start, finish, total;
	unsigned int queryID[2];
	bool first;

};