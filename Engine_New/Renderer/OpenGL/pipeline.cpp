#include "backend.hpp"

OpenGL::Pipeline OpenGL::Pipeline::create(PipelineDescriptor desc) {
	OpenGL::Pipeline ret = {};
	ret.depth = desc.depth;
	ret.blend = desc.blend;
	ret.rasterizer = desc.rasterizer;
	ret.layout = desc.layout;

	glGenVertexArrays(1, &ret.vao);
	glBindVertexArray(ret.vao);

	glBindVertexArray(0);
	return ret;
}