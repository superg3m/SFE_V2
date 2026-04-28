#include "backend.hpp"

OpenGL::Pipeline OpenGL::Pipeline::create(PipelineDescriptor desc) {
	OpenGL::Pipeline ret = {};
	ret.depth = desc.depth;
	ret.blend = desc.blend;
	ret.rasterizer = desc.rasterizer;

	return ret;
}