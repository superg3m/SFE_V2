#include "sfe_vertex_layout.hpp"
struct Mesh {
	void Bind();
	void Draw();

private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	VertexLayout layout;
	unsigned int vertex_count = 0;
	unsigned int index_count = 0;
};