#include "editor.hpp"

void initalize_imgui(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	RUNTIME_ASSERT_MSG(ImGui_ImplGlfw_InitForOpenGL(window, true), "[IMGUI ERROR]: ImGui_ImplGlfw_InitForOpenGL() Failed\n");
	RUNTIME_ASSERT_MSG(ImGui_ImplOpenGL3_Init("#version 330"), "[IMGUI ERROR]: ImGui_ImplOpenGL3_Init(#version 330) Failed\n");
}

void Editor::init(Engine* engine) {
	AppState* app = (AppState*)engine->application_state;
	initalize_imgui((GLFWwindow*)engine->window.ctx);
}