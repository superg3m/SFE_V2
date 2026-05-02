#pragma once


#include "../Runtime/Renderer/renderer.hpp"
#include "../../Game/game.hpp"

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
				ImGui::Begin("Inspector");
					ImGui::Checkbox("Demo Window", &app->show_demo_window);
				ImGui::End();

				ImGuiWindowFlags flags = (
					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
					ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | 
					ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground
				);

				Vector<Handle> texture_handles = renderer->backend.textures.handle_list(engine->memory.frame_allocator);
				for (Handle texture_handle : texture_handles) {
					auto texture = renderer->backend.textures.get(texture_handle);
					ImGui::Image(texture.id, ImVec2(64, 64));
					if (ImGui::IsItemClicked()) {
						app->face_texture.handle = texture_handle;
					}
					ImGui::SameLine();
				}

				if (app->show_demo_window) {
					ImGui::ShowDemoWindow(&app->show_demo_window);
				}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};