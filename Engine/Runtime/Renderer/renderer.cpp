#include "renderer.hpp"
#include "Common/common.cpp"

/*
void Renderer::draw(Allocator& frame_allocator) {
	// Mat4 view = Mat4::identity(); // camera.get_view_matrix();
	// Mat4 projection = Mat4::identity(); // camera.get_view_matrix();

	CommandBufferHandle cmd = this->begin_frame();
		this->bind_pipeline(cmd, this->opaque_pipeline);
		for (RenderCommand& command : this->opaque_commands) {
			this->bind_vertex_array(cmd, command.vao);
			this->bind_vertex_buffer(cmd, command.main_vbo);
			for (VertexBufferHandle vbo : command.extra_vbos) {
				this->bind_vertex_buffer(cmd, vbo);
			}
			this->bind_index_buffer(cmd, command.ebo);
			this->material_set_uniforms(command.material, Hashmap<const char*, BindingValue>({
				{"uModel", command.model},
				{"uView", command.view},
				{"uProjection", command.projection},
			}, frame_allocator));
			this->draw_mesh_entry(cmd, command.vao, command.mesh_entry, command.instance_count);
		}

		this->bind_pipeline(cmd, this->opaque_wireframe_pipeline);
		for (RenderCommand& command : this->opaque_wireframe_commands) {
			this->bind_vertex_array(cmd, command.vao);
			this->bind_vertex_buffer(cmd, command.main_vbo);
			for (VertexBufferHandle vbo : command.extra_vbos) {
				this->bind_vertex_buffer(cmd, vbo);
			}
			this->bind_index_buffer(cmd, command.ebo);
			this->material_set_uniforms(command.material, Hashmap<const char*, BindingValue>({
				{"uModel", command.model},
				{"uView", command.view},
				{"uProjection", command.projection},
			}, frame_allocator));
			this->draw_mesh_entry(cmd, command.vao, command.mesh_entry, command.instance_count);
		}
	this->end_frame(cmd);

	cmd = renderer.begin_frame();
		renderer.bind_pipeline(cmd, this->shadow_pipeline);
		for (RenderCommand& command : this->shadow_commands) {
			renderer.bind_vertex_buffer(cmd, command.vbo);
			renderer.bind_index_buffer(cmd, command.ebo);
			MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
			renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
				{"uModel", command.model},
				{"uView", view},
				{"uProjection", projection},
			}, allocator));
			renderer.draw_mesh_entry(command.mesh_entry_handle);
		}
	renderer.end_frame(cmd);

	cmd = renderer.begin_frame();
		renderer.bind_pipeline(cmd, this->post_effects_pipeline);
		for (RenderCommand& command : this->post_effects_commands) {
			renderer.bind_vertex_buffer(cmd, command.vbo);
			renderer.bind_index_buffer(cmd, command.ebo);
			MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
			renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
				{"uModel", command.model},
				{"uView", view},
				{"uProjection", projection},
			}, allocator));
			renderer.draw_mesh_entry(command.mesh_entry_handle);
		}
	renderer.end_frame(cmd);

	cmd = renderer.begin_frame();
		renderer.bind_pipeline(cmd, this->skybox_pipeline);
		for (RenderCommand& command : this->skybox_commands) {
			Mat4 skybox_view = view;
			skybox_view.v[0].w = 0.0f;
			skybox_view.v[1].w = 0.0f;
			skybox_view.v[2].w = 0.0f;

			renderer.bind_vertex_buffer(cmd, command.vbo);
			renderer.bind_index_buffer(cmd, command.ebo);
			MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
			renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
				{"uModel", command.model},
				{"uView", skybox_view},
				{"uProjection", projection},
			}, allocator));
			renderer.draw_mesh_entry(command.mesh_entry_handle);
		}
	renderer.end_frame(cmd);

	cmd = renderer.begin_frame();
		renderer.bind_pipeline(cmd, this->transparent_pipeline);
		for (RenderCommand& command : this->transparent_commands) {
			Mat4 skybox_view = view;
			skybox_view.v[0].w = 0.0f;
			skybox_view.v[1].w = 0.0f;
			skybox_view.v[2].w = 0.0f;

			renderer.bind_vertex_buffer(cmd, command.vbo);
			renderer.bind_index_buffer(cmd, command.ebo);
			MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
			renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
				{"uModel", command.model},
				{"uView", skybox_view},
				{"uProjection", projection},
			}, allocator));
			renderer.draw_mesh_entry(command.mesh_entry_handle);
		}
	renderer.end_frame(cmd);
	
	this->opaque_commands.clear();
	this->opaque_wireframe_commands.clear();
	this->shadow_commands.clear();
	this->post_effects_commands.clear();
	this->skybox_commands.clear();
	this->transparent_commands.clear();
}
*/