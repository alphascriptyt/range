#pragma once

// 4D matrix class
class M4 {
private:
	int length = 4;
	float elements[4][4] = { {0} };	// initialize matrix entries to 0

public:
	// constructors
	M4();

	// methods
	void setRow(int row, float x, float y, float z, float w);
	void print(); // for debugging

    // operator overloading
    float* operator[](int index); // accessing via index
};