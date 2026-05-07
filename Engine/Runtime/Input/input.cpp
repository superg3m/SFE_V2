#include "input_glfw.cpp"

InputAPI InputSystem::API() {
	return InputAPI(this->input_state);
}