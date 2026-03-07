#include "sfe_graphics_api.hpp"
#include "sfe_shader.hpp"
#include "sfe_material.hpp"
#include "sfe_mesh.hpp"

namespace SFE {
	Shader* GraphicsAPI::CreateShader(DS::Vector<const char*> shader_paths) {
		return new Shader(shader_paths);
	}

	unsigned int GraphicsAPI::CreateVertexBuffer(DS::Vector<float>& vertices) {
		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.count() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vbo;
	}

	unsigned int GraphicsAPI::CreateIndexBuffer(DS::Vector<unsigned int>& indices) {
		unsigned int ebo = 0;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.count() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return ebo;
	}
	
	void GraphicsAPI::BindShaderProgram(Shader* shader) {
		if (shader) {
			shader->use();
		}
	}

	void GraphicsAPI::BindMaterial(Material* material) {
		if (material) {
			material->bind();
		}
	}

	void GraphicsAPI::BindMesh(Mesh* mesh) {
		if (mesh) {
			mesh->Bind();
		}
	}

	void GraphicsAPI::DrawMesh(Mesh* mesh) {
		if (mesh) {
			mesh->Draw();
		}
	}
}