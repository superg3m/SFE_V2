#include "editor.hpp"
#include "../../Vendor/vendor.hpp"

void initalize_imgui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	RUNTIME_ASSERT_MSG(ImGui_ImplGlfw_InitForOpenGL(window, true), "[IMGUI ERROR]: ImGui_ImplGlfw_InitForOpenGL() Failed\n");
	RUNTIME_ASSERT_MSG(ImGui_ImplOpenGL3_Init("#version 330"), "[IMGUI ERROR]: ImGui_ImplOpenGL3_Init(#version 330) Failed\n");

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	// ImGui::StyleColorsDark();
}

void Editor::init(Engine* engine) {
	AppState* app = (AppState*)engine->application_state;
	initalize_imgui((GLFWwindow*)engine->window.ctx);
}