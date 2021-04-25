#include "cube.hpp"
#include "chunk.hpp"
#include <vector>
#include <cmath>

void chunk::generateCubes() {
	float inc = dim / numCubes;
	/*std::cout << inc<<"\n";*/
	for (float z = 0;z < numCubes;z++) {
		for (float y = 0; y < numCubes;y++) {
			for (float x = 0;x < numCubes;x++) {
				cube tempCube = cube(glm::vec3(startPoint.x+ x*inc, startPoint.y+y * inc, startPoint.z+z * inc),inc);
				tempCube.generateTriangles();
				cubes.push_back(tempCube);
			}
		}
	}
	return;
}