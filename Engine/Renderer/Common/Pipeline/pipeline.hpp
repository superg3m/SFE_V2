#pragma once

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

struct Pipeline {
	RasterizerState rasterizer = {};
	DepthState depth = {};
	BlendState blend = {};
	
	bool operator==(Pipeline& other) const {
		return Memory::equal(this, sizeof(Pipeline), &other, sizeof(Pipeline));
	}
};