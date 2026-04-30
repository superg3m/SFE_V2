#include "editor.hpp"
#include "../../Game/game.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

void initalize_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	RUNTIME_ASSERT_MSG(ImGui_ImplGlfw_InitForOpenGL(window, true), "[IMGUI ERROR]: ImGui_ImplGlfw_InitForOpenGL() Failed\n");
	RUNTIME_ASSERT_MSG(ImGui_ImplOpenGL3_Init("#version 330"), "[IMGUI ERROR]: ImGui_ImplOpenGL3_Init(#version 330) Failed\n");
}

void Editor::init(Engine* engine) {
	AppState* app = (AppState*)engine->application_state;
	initalize_imgui(engine->window);
}

void Editor::render(Engine* engine) {
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

			Vector<Handle> texture_handles = engine->renderer.backend.textures.handle_list(engine->frame_allocator);
			for (Handle texture_handle : texture_handles) {
				OpenGL::Texture texture = engine->renderer.backend.textures.get(texture_handle);
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

Editor* editor = nullptr;