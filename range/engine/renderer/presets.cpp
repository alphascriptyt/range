#include "presets.h"

// actually define the cube 
namespace Cube {
	std::vector<std::vector<float>> vertices = { {0,0,0}, {0,1,0}, {1,1,0}, {1,0,0}, {1,1,1}, {1,0,1}, {0,1,1}, {0,0,1} };
	std::vector<std::vector<int>> faces = { {0, 1, 2}, {0, 2, 3}, {3, 2, 4}, {3, 4, 5}, {5, 4, 6}, {5, 6, 7}, {7, 6, 1}, {7, 1, 0}, {1, 6, 4}, {1, 4, 2}, {5, 7, 0}, {3, 5, 0} };
}