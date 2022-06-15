#pragma once
#include <chrono>
#include <string>

class Timer {
	std::chrono::high_resolution_clock::time_point start; // the start time point

public:
	// constructor
	Timer();			// set start time

	// methods
	float elapsed();	// get elapsed time in ms
	void reset();       // sets elapsed to 0
	void print(std::string before = "", std::string after = "", std::string end = "\n"); // pretty formatting of elapsed time
};