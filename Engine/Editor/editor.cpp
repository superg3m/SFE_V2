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
	bool show_demo_window = true;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		ImGui::ShowDemoWindow(&show_demo_window);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Editor* editor = nullptr;