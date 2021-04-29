#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "vboIndexer.hpp"
#include <string.h>

struct PackedVertex {
	glm::vec3 position;///<Pointer to left subtree of node
	bool operator<(const PackedVertex that) const {
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex)) > 0;
	};
};
bool getSimilarVertexIndex(
	PackedVertex& packed,
	std::map<PackedVertex, unsigned short>& VertexToOutIndex,
	unsigned short& result
) {
	std::map<PackedVertex, unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if (it == VertexToOutIndex.end()) {
		return false;
	}
	else {
		result = it->second;
		return true;
	}
}
void indexVBO(
	std::vector<glm::vec3>& in_vertices,
	//std::vector<glm::vec3>& in_normals,
	std::vector<unsigned short>& out_indices,
	std::vector<glm::vec3>& out_vertices
	//std::vector<glm::vec3>& out_normals
) {
	std::map<PackedVertex, unsigned short> VertexToOutIndex;

	for (unsigned int i = 0; i < in_vertices.size(); i++) {

		PackedVertex packed = { in_vertices[i] };
		unsigned short index;
		bool found = getSimilarVertexIndex(packed, VertexToOutIndex, index);
		if (found) { // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back(index);
		}
		else { // If not, it needs to be added in the output data.
			out_vertices.push_back(in_vertices[i]);
			unsigned short newindex = (unsigned short)out_vertices.size() - 1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed] = newindex;
		}
	}
}