#pragma once

#include "mat4D.h"
#include <iostream>

// default constructor
M4::M4() {

}

// methods
void M4::setRow(int row, float x, float y, float z, float w) {
    // check valid row/index
    if (row >= length) {
        std::cout << "Invalid index given for setting row of matrix." << std::endl;
        exit(0);
    }

    // set row to the given values
    elements[row][0] = x;
    elements[row][1] = y;
    elements[row][2] = z;
    elements[row][3] = w;
}

void M4::print() {
    // method to print the matrix as a grid
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            std::cout << elements[r][c] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

// overload index operator, for accessing parts of matrices
float* M4::operator[](int index)
{
    // ensure valid index
    if (index >= length) {
        std::cout << "Invalid index given for accessing matrix." << std::endl;
        exit(0);
    }

    return elements[index];
}

