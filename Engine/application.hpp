#pragma once

namespace SFE {
	struct Application {
		virtual bool Initalize() = 0;
		virtual void Update(float dt) = 0;
		virtual void Shutdown() = 0;

		void SetShouldClose(bool should_close);
		bool ShouldClose() const;

	private:
		bool should_close = false;
	};
}