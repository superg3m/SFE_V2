#pragma once

#include "../../Game/game.hpp"
#include "../Runtime/Renderer/renderer.hpp"
#include "../../Vendor/vendor.hpp"

// https://www.reddit.com/r/cpp_questions/comments/x0ypqt/imgui_tree_view_implementation/
inline void render_tree_hierarchy(Scene* scene, EntityHandle entity, EntityHandle& selected) {
	Entity& entity_slot = scene->entities.get(entity.handle); 

	ImGuiTreeNodeFlags flags = (
		ImGuiTreeNodeFlags_OpenOnArrow
	);

	bool is_leaf = entity_slot.children.count == 0;
	if (is_leaf) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	if (entity == selected) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool open = ImGui::TreeNodeEx((void*)((size_t)entity.handle.index), flags, "%s", entity_slot.name.data);
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		selected = entity;
	}

	if (ImGui::BeginDragDropSource()) {
		ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(EntityHandle));
			ImGui::Text("Being Dragged: %s", entity_slot.name.data);
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
			EntityHandle dropped = *(EntityHandle*)payload->Data;
			scene->set_parent(dropped, entity);
		}

		ImGui::EndDragDropTarget();
	}

	if (open && !is_leaf) {
		for (EntityHandle child : entity_slot.children) {
			render_tree_hierarchy(scene, child, selected);
		}
		ImGui::TreePop();
	}
}

struct Engine;
struct Editor {
	EntityHandle selected = EntityHandle::invalid();
	ImGuizmo::OPERATION current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;

	void init(Engine* engine);

	void render_texture_image(Engine* engine, Renderer<OpenGL>* renderer, TextureHandle texture) {
		OpenGL::Texture texture_slot = renderer->backend.textures.get(texture.handle);
		ImGui::Image(
			(ImTextureID)(uintptr_t)texture_slot.id,
			ImVec2(64, 64),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}

	template<typename B>
	void render(Engine* engine, Renderer<B>* renderer) {
		AppState* app = (AppState*)engine->application_state;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		Mat4 view = engine->get_view_matrix();
		Mat4 projection = engine->get_projection_matrix();

		{
			ImGuiWindowFlags flags = (
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoDocking |
				ImGuiWindowFlags_NoBringToFrontOnFocus |
				ImGuiWindowFlags_NoNavFocus
			);

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
			ImGui::Begin("DockspaceRoot", nullptr, flags);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);

				ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
				ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

				if (!ImGui::DockBuilderGetNode(dockspace_id)) {
					ImGui::DockBuilderRemoveNode(dockspace_id);
					ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
					ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

					ImGuiID right_id;
					ImGuiID left_id = ImGui::DockBuilderSplitNode(
						dockspace_id, ImGuiDir_Left, 0.25f, nullptr, &right_id
					);

					ImGuiID top_id;
					ImGuiID bottom_id = ImGui::DockBuilderSplitNode(
						dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &top_id
					);

					ImGui::DockBuilderDockWindow("Hierarchy", left_id);
					ImGui::DockBuilderDockWindow("Assets", bottom_id);
					ImGui::DockBuilderFinish(dockspace_id);
				}

				ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockFlags);
			ImGui::End();
		}

		{
			ImGui::Begin("Hierarchy");
				render_tree_hierarchy(&engine->scene, engine->scene.root, this->selected);
			ImGui::End();
		}

		{
			ImGui::Begin("Inspector");
				if (ImGui::RadioButton("Translate", current_gizmo_operation == ImGuizmo::TRANSLATE))
					current_gizmo_operation = ImGuizmo::TRANSLATE;
				ImGui::SameLine();
				if (ImGui::RadioButton("Rotate", current_gizmo_operation == ImGuizmo::ROTATE))
					current_gizmo_operation = ImGuizmo::ROTATE;
				ImGui::SameLine();
				if (ImGui::RadioButton("Scale", current_gizmo_operation == ImGuizmo::SCALE))
					current_gizmo_operation = ImGuizmo::SCALE;

				if (this->selected != EntityHandle::invalid()) {
					#define BUFFER_CAPACITY 256
					char buffer[BUFFER_CAPACITY];
					int ret = snprintf(buffer, BUFFER_CAPACITY, "Picked Index: %d", this->selected.handle.index);
					RUNTIME_ASSERT(ret >= 0);
					ImGui::Text("%s", buffer);

					Entity& entity_slot = engine->scene.entities.get(this->selected.handle);
					Transform& t = entity_slot.transform;

					ImGui::DragFloat3("Position", &t.position.x, 0.1f);
					ImGui::DragFloat3("Scale", &t.scale.x, 0.1f);

					for (const auto& entry : entity_slot.components) {
						Component* c = entry.value;
						if (MeshComponent* mesh_component = dynamic_cast<MeshComponent*>(c)) {
							if (ImGui::CollapsingHeader("MeshComponent")) {
								ImGui::Checkbox("Wireframe", &mesh_component->pipeline.rasterizer.fill);

								auto mesh_slot = renderer->backend.meshes.get(mesh_component->mesh.handle);
								Vector<Handle> material_handles = renderer->backend.materials.handle_list(engine->memory.frame_allocator);
								int index = 0; 
								for (Handle material_handle : material_handles) {
									ImGui::Text("Material %d", index++);
									if (ImGui::IsItemClicked()) {
										mesh_slot.entries[0].material.handle = material_handle;
									}
									ImGui::SameLine();
								}
							}
						} else {
							RUNTIME_ASSERT(false);
						}
					}
				}
			ImGui::End();
		}

		{
			ImGuiWindowFlags flags = (
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoScrollWithMouse |
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoMouseInputs |
				ImGuiWindowFlags_NoBackground
			);

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2((float)engine->window.WINDOW_WIDTH, (float)engine->window.WINDOW_HEIGHT));
			ImGui::Begin("GizmoOverlay", nullptr, flags);
				if (this->selected != EntityHandle::invalid()) {
					Entity& entity_slot = engine->scene.entities.get(this->selected.handle);

					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetRect(0, 0, (float)engine->window.WINDOW_WIDTH, (float)engine->window.WINDOW_HEIGHT);

					Mat4 viewT       = view.transpose();
					Mat4 projT       = projection.transpose();
					Mat4 newTransformT = entity_slot.get_world_transform(&engine->scene).transpose();

					bool is_manipulated = ImGuizmo::Manipulate(
						&viewT.v[0].x, &projT.v[0].x,
						current_gizmo_operation, ImGuizmo::WORLD,
						&newTransformT.v[0].x
					);

					if (is_manipulated) {
						Mat4 newTransformWorld = newTransformT.transpose();
						entity_slot.set_world_transform(&engine->scene, newTransformWorld);
					}
				}
			ImGui::End();
		}

		{
			ImGui::Begin("Assets");
				Vector<Handle> texture_handles = renderer->backend.textures.handle_list(engine->memory.frame_allocator);
				for (Handle texture_handle : texture_handles) {
					auto texture = renderer->backend.textures.get(texture_handle);
					ImGui::Image(
						(ImTextureID)(uintptr_t)texture.id,
						ImVec2(64, 64),
						ImVec2(0, 1),
						ImVec2(1, 0)
					);
					if (ImGui::IsItemClicked()) {
						app->face_texture.handle = texture_handle;
					}
					ImGui::SameLine();
				}
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};