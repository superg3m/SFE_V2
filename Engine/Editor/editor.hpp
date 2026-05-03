#pragma once


#include "../../Game/game.hpp"
#include "../Runtime/Renderer/renderer.hpp"
#include "../../Vendor/vendor.hpp"

/*
inline void draw_entity_node(Entity* e, EntityHandle selected) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (e->children.count == 0) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (selected == e->handle) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx((void*)e->handle.id, flags, "%s", e->name);
    if (ImGui::IsItemClicked()) {
        selected = e->handle;
    }

    if (open) {
        for (Entity* child : e->children) {
            draw_entity_node(child, selected);
        }
        ImGui::TreePop();
    }
}
*/

struct Engine;
struct Editor {
	void init(Engine* engine);

	template<typename B>
	void render(Engine* engine, Renderer<B>* renderer) {
		AppState* app = (AppState*)engine->application_state;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

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

			// THIS is what actually removes the background
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

			ImGui::Begin("DockspaceRoot", nullptr, flags);
				ImGui::PopStyleColor();
				ImGui::PopStyleVar(2);

				ImGuiID dockspace_id = ImGui::GetID("Dockspace");
				ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
			ImGui::End();
		}

		/*
		{
			ImGui::Begin("Viewport");
				ImVec2 size = ImGui::GetContentRegionAvail();

				if (size.x > 0 && size.y > 0) {
					// renderer->resize_viewport((u32)size.x, (u32)size.y);
				}

				ImGui::Image(
					(ImTextureID)(uintptr_t)renderer->viewport_color_texture,
					size,
					ImVec2(0, 1),
					ImVec2(1, 0)
				);
			ImGui::End();
		}
		*/

		/*
		{
			ImGui::Begin("Hierarchy");
				auto draw_entity_node = [&](auto&& self, Entity* e) -> void {
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

					if (e->children.count == 0)
						flags |= ImGuiTreeNodeFlags_Leaf;

					if (app->selected_entity == e->handle)
						flags |= ImGuiTreeNodeFlags_Selected;

					bool open = ImGui::TreeNodeEx(
						(void*)(uintptr_t)e->handle.id,
						flags,
						"%s",
						e->name
					);

					if (ImGui::IsItemClicked())
						app->selected_entity = e->handle;

					if (open) {
						for (Entity* child : e->children)
							self(self, child);

						ImGui::TreePop();
					}
				};

				for (Entity* root : app->scene->roots) {
					draw_entity_node(draw_entity_node, root);
				}
			ImGui::End();
		}
		*/

		/*
		{
			ImGui::Begin("Inspector");

			if (app->selected_entity.valid()) {
				Entity* e = app->scene->get(app->selected_entity);

				if (e) {
					ImGui::Text("Name: %s", e->name);

					ImGui::Separator();

					ImGui::DragFloat3("Position", &e->transform.position.x, 0.1f);
					ImGui::DragFloat3("Rotation", &e->transform.rotation.x, 0.1f);
					ImGui::DragFloat3("Scale",    &e->transform.scale.x, 0.1f);
				}
			}

			ImGui::End();
		}
		*/

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

		if (app->show_demo_window) {
			ImGui::ShowDemoWindow(&app->show_demo_window);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};