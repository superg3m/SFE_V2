#include "backend.hpp"

#include "pipeline.cpp"
#include "texture.cpp"
#include "command_buffer.cpp"
#include "shader.cpp"

void _GL_ERROR_CHECK(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char* error = nullptr;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        
        RUNTIME_ASSERT_MSG(false, "%s | %s:%d\n", error, file, line);
    }
}

/*
void OpenGL::draw(Vector<RenderObject>& objects) {
	OpenGL::CommandBuffer cmd = renderer.BeginFrame();

	for (auto& obj : objects) {
		cmd.BindPipeline(obj.pipeline);
		cmd.BindVertexBuffer(obj.vbo);
		cmd.BindIndexBuffer(obj.ibo);
		cmd.BindTexture(0, obj.albedo);
		cmd.SetUniform("uModel", obj.model);

		cmd.DrawIndexed(36);
	}

	renderer.EndFrame(cmd);
}
*/