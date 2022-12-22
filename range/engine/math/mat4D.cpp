#pragma once

#include "mat4D.h"
#include <iostream>

// default constructor
M4::M4() {

}

// methods
void M4::makeIdentity() {
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;
}

M4 M4::getInverse() {
    // TODO: Refactor
    // from onelonecoder, only for rotation/translation (eg view matrix)
    M4 inverted;

    /*
    
    inverted[0][0] = -m[0][0];
    inverted[0][3] = -m[0][3];
    inverted[1][1] = -m[1][1];
    inverted[1][3] = -m[1][3];
    inverted[2][2] = -m[2][2];
    inverted[2][3] = -m[2][3];
    inverted[3][3] = 1;
    
    */
    
    

    return inverted;

}

void M4::setRow(int row, float x, float y, float z, float w) {
    // set row to the given values
    m[row][0] = x;
    m[row][1] = y;
    m[row][2] = z;
    m[row][3] = w;
}

void M4::print() {
    // method to print the matrix as a grid
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            std::cout << m[r][c] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// operator overloading
float* M4::operator[](int index) {
    return m[index];
}

M4 M4::operator*(M4& m2) {
    // multiply matrices, for combining transformations
    M4 out;

    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            out[r][c] = m[r][0] * m2[0][c] + m[r][1] * m2[1][c] + m[r][2] * m2[2][c] + m[r][3] * m2[3][c];
        }
    }

    return out;
}

V3 M4::operator*(const V3& v) {
    V3 out;
    out.x = (m[0][0] * v.x) + (m[0][1] * v.y) + (m[0][2] * v.z) + (m[0][3] * v.w);
    out.y = (m[1][0] * v.x) + (m[1][1] * v.y) + (m[1][2] * v.z) + (m[1][3] * v.w);
    out.z = (m[2][0] * v.x) + (m[2][1] * v.y) + (m[2][2] * v.z) + (m[2][3] * v.w);
    out.w = (m[3][0] * v.x) + (m[3][1] * v.y) + (m[3][2] * v.z) + (m[3][3] * v.w);

    return out;
}

M4 makeRotationMatrix(float yaw, float pitch, float roll) {
    M4 m;

    // precalculate trig to avoid multiple calculations
    float sin_pitch = sin(pitch);
    float sin_yaw = sin(yaw);
    float sin_roll = sin(roll);
    float cos_pitch = cos(pitch);
    float cos_yaw = cos(yaw);
    float cos_roll = cos(roll);

    // setup combined rotation matrix
    M4 rotation;
    rotation.makeIdentity();
    
    rotation[0][0] = cos_yaw;
    rotation[0][2] = -sin_yaw;
    rotation[1][0] = sin_pitch * sin_yaw;
    rotation[1][1] = cos_pitch;
    rotation[1][2] = sin_pitch * cos_yaw;
    rotation[2][0] = cos_pitch * sin_yaw;
    rotation[2][1] = -sin_pitch;
    rotation[2][2] = cos_pitch * cos_yaw;

    return rotation;
}
