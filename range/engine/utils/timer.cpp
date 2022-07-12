#include "timer.h"
#include <thread>
#include <iostream>
#include <Windows.h>

// constructor
Timer::Timer() {
    start = std::chrono::high_resolution_clock::now();
}

// methods
float Timer::elapsed() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
}

void Timer::reset() {
    start = std::chrono::high_resolution_clock::now();
}

void Timer::print(std::string before, std::string after, std::string end) {
    std::cout << before << elapsed() << after << end;
}