#include "application.hpp"

namespace SFE {
	void Application::SetShouldClose(bool should_close) {
		this->should_close = should_close;
	}
	
	bool Application::ShouldClose() const {
		return this->should_close;
	}
}