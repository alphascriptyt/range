#pragma once

// 4D matrix class
class M4 {
private:
	int length = 4;
	float m[4][4] = { {0} };	// initialize matrix entries to 0

public:
	// constructors
	M4();

	// methods
	void makeIdentity();
	M4 getInverse();
	M4 transpose();

	void setRow(int row, float x, float y, float z, float w);
	void print(); // for debugging

    // operator overloading
    float* operator[](int index); // accessing via index
	M4 operator*(M4& m2);
};

M4 makeRotationMatrix(float yaw, float pitch, float roll);