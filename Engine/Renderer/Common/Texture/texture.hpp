enum class TextureSamplerType {
	SAMPLER_2D,
	CUBEMAP_3D
};

struct TextureDescription {
    bool pixel_perfect = false;
    bool vertical_flip = true;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
};