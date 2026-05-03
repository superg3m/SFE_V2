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

	void init(Engine* engine);

	template<typename B>
	void render(Engine* engine, Renderer<B>* renderer) {
		AppState* app = (AppState*)engine->application_state;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		// === Full-screen dockspace host window ===
		{
			ImGuiWindowFlags flags = (
				ImGuiWindowFlags_NoTitleBar      |
				ImGuiWindowFlags_NoCollapse      |
				ImGuiWindowFlags_NoResize        |
				ImGuiWindowFlags_NoMove          |
				ImGuiWindowFlags_NoDocking       |
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

			// === Single dockspace ===
			ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
			ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

			// === Build layout once ===
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


				// Dock "Hierarchy" into the left panel
				ImGui::DockBuilderDockWindow("Hierarchy", left_id);
				ImGui::DockBuilderDockWindow("Assets", bottom_id);

				ImGui::DockBuilderFinish(dockspace_id);
			}

			ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockFlags);

			ImGui::End();
		}

		// === Hierarchy window ===
		{
			ImGui::Begin("Hierarchy");
			render_tree_hierarchy(&engine->scene, engine->scene.root, this->selected);
			ImGui::End();
		}

		// === Assets window ===
		{
			ImGui::Begin("Assets");
			Vector<Handle> texture_handles = renderer->backend.textures.handle_list(engine->memory.frame_allocator);
			for (Handle texture_handle : texture_handles) {
				auto texture = renderer->backend.textures.get(texture_handle);
				ImGui::Image(
					(ImTextureID)(uintptr_t)texture.id,
					ImVec2(64, 64),
					ImVec2(0, 1), ImVec2(1, 0)
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