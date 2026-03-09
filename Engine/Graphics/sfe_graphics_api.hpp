#pragma once

#include "../Core/sfe_core.hpp"

namespace SFE {
	struct Shader;
	struct Material;
	struct Mesh;

	struct GraphicsAPI {
		Shader* CreateShader(DS::Vector<const char*> shader_paths);
		unsigned int CreateVertexBuffer(DS::Vector<float>& vertices);
		unsigned int CreateIndexBuffer(DS::Vector<unsigned int>& indices);

		void BindShaderProgram(Shader* shader);
		void BindMaterial(Material* material);
		void DrawMesh(Mesh* mesh);
	};
}
