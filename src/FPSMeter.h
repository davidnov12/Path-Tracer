/*
* Path tracing na GPU
* Bakalarska prace
* David Novak, xnovak1l
* FIT VUT Brno, 2016
*
* FPSMeter.h - meric FPS
*/

#pragma once


class FPSMeter {

public:

	FPSMeter();
	bool checkFPS(double currentTime);
	void reinit(double time);
	float getFPS();

private:

	float frames;
	double time;

};