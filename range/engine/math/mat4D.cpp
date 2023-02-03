#pragma once

#include "mat4D.h"
#include <vectormaths.h>
#include <iostream>

// default constructor
M4::M4() {

}

// methods
void M4::makeIdentity() {
    setRow(0, 1, 0, 0, 0);
    setRow(1, 0, 1, 0, 0);
    setRow(2, 0, 0, 1, 0);
    setRow(3, 0, 0, 0, 1);
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
    // multiply matrices for combining transformations
    M4 out;

    // TODO: Should these for loops be swapped around? Or does it not change anything?
    // TODO: look into the actual way to do this for cache misses
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            out[r][c] = m[r][0] * m2[0][c] + m[r][1] * m2[1][c] + m[r][2] * m2[2][c] + m[r][3] * m2[3][c];
        }
    }

    return out;
}

M4 makeRotationMatrix(float yaw, float pitch, float roll) {
    
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

    // TODO: See https://en.wikipedia.org/wiki/Rotation_matrix for testing
    
    /*
    so...
    x/z arae working, however, y is calculated from x and z?
    does this mean the rotation matrix is wrong?
    or the multiplication is wrong?? 
    is it not tranposed????
    
    
    */

    // roll only working on one axis atm for some reason.
    
    /*
    rotation[0][0] = cos_yaw * cos_roll;
    rotation[0][1] = cos_yaw * -sin_roll;
    rotation[0][2] = -sin_yaw;
    rotation[1][0] = sin_pitch * sin_yaw * cos_roll + cos_pitch * sin_roll;
    rotation[1][1] = cos_pitch * cos_roll - sin_pitch * sin_yaw * sin_roll;
    rotation[1][2] = sin_pitch * cos_yaw;
    rotation[2][0] = sin_yaw * cos_pitch * cos_roll - sin_pitch * sin_roll;
    rotation[2][1] = -sin_yaw * cos_pitch * sin_roll - sin_pitch * cos_roll;
    rotation[2][2] = cos_pitch * cos_yaw;
    */
    // TODO: everything seems fine except the y changing when moving in certain directions?

    /*
    rotation[0][0] = cos_yaw * cos_roll;
    rotation[1][0] = cos_yaw * -sin_roll;
    rotation[2][0] = -sin_yaw;
    rotation[0][1] = sin_pitch * sin_yaw * cos_roll + cos_pitch * sin_roll;
    rotation[1][1] = cos_pitch * cos_roll - sin_pitch * sin_yaw * sin_roll;
    rotation[2][1] = sin_pitch * cos_yaw;
    rotation[0][2] = sin_yaw * cos_pitch * cos_roll - sin_pitch * sin_roll;
    rotation[1][2] = -sin_yaw * cos_pitch * sin_roll - sin_pitch * cos_roll;
    rotation[2][2] = cos_pitch * cos_yaw;
    */

    
    rotation[0][0] = cos_yaw;
    rotation[0][2] = sin_yaw;
    rotation[2][0] = -sin_yaw;
    rotation[2][2] = cos_yaw;
    

    M4 yaw_rotation;
    yaw_rotation.makeIdentity();
    yaw_rotation[0][0] = cos_yaw;
    yaw_rotation[0][2] = sin_yaw;
    yaw_rotation[2][0] = -sin_yaw;
    yaw_rotation[2][2] = cos_yaw;

    M4 roll_rotation;
    roll_rotation.makeIdentity();
    roll_rotation[0][0] = cos_roll;
    roll_rotation[0][1] = -sin_roll;
    roll_rotation[1][0] = sin_roll;
    roll_rotation[1][1] = cos_roll;

    M4 pitch_rotation;
    pitch_rotation.makeIdentity();
    pitch_rotation[1][1] = cos_pitch;
    pitch_rotation[1][2] = -sin_pitch;
    pitch_rotation[2][1] = sin_pitch;
    pitch_rotation[2][2] = cos_pitch;


    return yaw_rotation * pitch_rotation;



}