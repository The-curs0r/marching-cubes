#ifndef TRIANGLEFACE_HPP
#define TRIANGLEFACE_HPP

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class triangleFace {
public:
	int ID;
	int indices[3];
	glm::vec3 normal = glm::vec3(0.0f);

	//Constructors
	triangleFace(int id, int v1, int v2, int v3, std::vector<glm::vec3> vertices) {
		ID = id;
		indices[0] = v1;
		indices[1] = v2;
		indices[2] = v3;
		normal = glm::cross(vertices[v2] - vertices[v1], vertices[v3] - vertices[v1]);
	}
};

#endif