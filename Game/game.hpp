#include <sfe.hpp>

struct Game : public SFE::Application {
	bool Initalize() override;
	void Update(float dt) override;
	void Shutdown() override;

	
private:
	SFE::Material material;
	SFE::Mesh* mesh;
};