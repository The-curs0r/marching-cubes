#ifndef CUBE_HPP
#define CUBE_HPP

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "SimplexNoise.h"
class cube {
private:
	std::vector<float> vertexNoise;
	std::vector<glm::vec3> vertexCoord;
public:
	std::vector<glm::vec3> triangles;
	void generateTriangles();
	//Constructors
	cube(std::vector<glm::vec3> coord, std::vector<float> noise){
		vertexNoise = noise;
		vertexCoord = coord;
	}
	cube(glm::vec3 coord, float dim, std::vector<float> noise) {
		vertexNoise = noise;
		vertexCoord.push_back(coord);
		vertexCoord.push_back(glm::vec3(coord.x, coord.y + dim, coord.z));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y + dim, coord.z));
		vertexCoord.push_back(glm::vec3(coord.x+ dim,coord.y,coord.z));
		vertexCoord.push_back(glm::vec3(coord.x, coord.y, coord.z+dim));
		vertexCoord.push_back(glm::vec3(coord.x, coord.y + dim, coord.z + dim));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y + dim, coord.z + dim));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y, coord.z + dim));
	}
	cube(glm::vec3 coord, float dim) {
		vertexCoord.push_back(coord);
		vertexCoord.push_back(glm::vec3(coord.x, coord.y + dim, coord.z));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y + dim, coord.z));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y, coord.z));
		vertexCoord.push_back(glm::vec3(coord.x, coord.y, coord.z + dim));
		vertexCoord.push_back(glm::vec3(coord.x, coord.y + dim, coord.z + dim));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y + dim, coord.z + dim));
		vertexCoord.push_back(glm::vec3(coord.x + dim, coord.y, coord.z + dim));
		std::vector<float> noise;
		SimplexNoise simplexNoise;
		for (int i = 0;i < 8;i++) {
			int ind = vertexCoord.size() - 1 - i;
			//std::cout << vertexCoord[ind].x << " " << vertexCoord[ind].y << " " << vertexCoord[ind].z << "\n";
			noise.push_back(simplexNoise.noise(vertexCoord[ind].x, vertexCoord[ind].y, vertexCoord[ind].z));
		}
		//std::cout << "\n\n";
		std::reverse(noise.begin(), noise.end());
		vertexNoise = noise;
	}
};
#endif // !CUBE_HPP
