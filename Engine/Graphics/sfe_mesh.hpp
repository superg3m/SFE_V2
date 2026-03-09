#pragma once

#include "sfe_vertex_layout.hpp"

namespace SFE {
	struct Mesh {
		Mesh(VertexLayout layout, DS::Vector<float> &vertices);
		Mesh(VertexLayout layout, DS::Vector<float> &vertices, DS::Vector<unsigned int> &indices);
		Mesh(Mesh &mesh) = delete;
		Mesh& operator=(Mesh &mesh) = delete;

		void Draw();

		unsigned int VAO = 0;
		unsigned int VBO = 0;
		unsigned int EBO = 0;

		VertexLayout layout;
		unsigned int vertex_count = 0;
		unsigned int index_count = 0;

	private:


	};
}