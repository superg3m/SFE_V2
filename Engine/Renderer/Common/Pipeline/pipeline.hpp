struct DepthState {
	bool depth_testing = true;
	bool depth_write = true;
	// func
};

struct BlendState {
	bool enabled = false;
	// source = src_alpha;
	// destination = one_minus_source_alpha;
};

struct RasterizerState {
	bool cull_enabled = true;
	bool cull_face_back = true;
	bool ccw_winding = true;
	bool fill = true;
};

struct PipelineDescriptor {
	RasterizerState rasterizer;
	DepthState depth;
	BlendState blend;

	PipelineDescriptor(RasterizerState rasterizer, DepthState depth, BlendState blend) {
		this->rasterizer = rasterizer;
		this->depth = depth;
		this->blend = blend;
	}
};