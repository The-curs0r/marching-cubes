#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <vector>
#include <glm/glm.hpp>
#include "cube.hpp"

//A chunk of 32*32*32 cubes eventually
//Test with lower number
class chunk {
private:
	glm::vec3 startPoint = glm::vec3(0.0f);
	int numCubes = 0; //Cubes in a row
	float dim = 1.0f; //Total length of side. Keep multiple of numCubes pls
public:
	std::vector<cube> cubes;
	void generateCubes();
	void generateCubes(float *);
	//Constructors
	chunk(glm::vec3 coord, int cubes, float length) {
		startPoint = coord;
		numCubes = cubes;
		dim = length;
	}
};
#endif // !CHUNK_HPP
