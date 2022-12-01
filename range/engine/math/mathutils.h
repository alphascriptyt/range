#pragma once

// math constants
constexpr float TWO_PI = 6.28318530718;			// 360 degrees in radians
constexpr float PI = 3.14159265358979323846;	// 180 degrees in radians
constexpr float HALF_PI = 1.57079632679;		// 90 degrees in radians

// helper functions
inline float toRadians(float degrees) {
	return (degrees * PI) / 180.0f;
}