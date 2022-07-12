#pragma once

#include "utils.h"

// function to split a string by a given delimeter
std::vector<std::string> splitStringBy(std::string str, char delim) {
	std::vector<std::string> tokens;		// create vector to store the parts
	std::string buffer;						// make a buffer string
	std::stringstream stream(str);			// create a stream from the string

	while (std::getline(stream, buffer, delim)) {
		// get each part of the string split by the delim
		tokens.push_back(buffer);
	}

	// return the parts
	return tokens;
}