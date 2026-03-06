#pragma once

#include <glad/glad.h>

#include "./sfe_texture.hpp"
#include "../../Core/DataStructure/sfe_ds.hpp"
#include "../../Core/Math/sfe_math.hpp"

namespace SFE {
    struct UniformDesc {
        GLenum type;
        GLint location;
    };

    struct Shader {
        unsigned int program_id = 0;

        Shader() = default;
        Shader(DS::Vector<const char*> shader_paths);

        Shader(Shader &shader) = delete;
        Shader& operator=(Shader &shader) = delete;

        void use() const;
        void setModel(Math::Mat4 &model);
        void setView(Math::Mat4 &view);
        void setProjection(Math::Mat4 &projection);

        void setBool(const char* name, bool value);
        void setInt(const char* name, int value);
        void setFloat(const char* name, float value);
        void setTexture2D(const char* name, int texture_unit, Texture &texture);
        void setTexture3D(const char* name, int texture_unit, Texture &texture);
        void setVec2(const char* name, const Math::Vec2& value);
        void setVec2(const char* name, float x, float y);
        void setVec3(const char* name, const Math::Vec3& value);
        void setVec3(const char* name, float x, float y, float z);
        void setVec4(const char* name, const Math::Vec4& value);
        void setVec4(const char* name, float x, float y, float z, float w);
        void setMat4(const char* name, const Math::Mat4& mat);
    protected:
        DS::Vector<const char*> shader_paths;
        DS::Hashmap<const char*, UniformDesc> uniforms;

        GLenum typeFromPath(const char* path);
        void checkCompileError(unsigned int source_id, const char* path);
        unsigned int shaderSourceCompile(const char* path);
        unsigned int getUniformLocation(const char* name, GLenum type);

        void setBool(unsigned int location, bool value);
        void setInt(unsigned int location, int value);
        void setFloat(unsigned int location, float value);
        void setVec2(unsigned int location, const Math::Vec2& value);
        void setVec2(unsigned int location, float x, float y);
        void setVec3(unsigned int location, const Math::Vec3& value);
        void setVec3(unsigned int location, float x, float y, float z);
        void setVec4(unsigned int location, const Math::Vec4& value);
        void setVec4(unsigned int location, float x, float y, float z, float w);
        void setMat4(unsigned int location, const Math::Mat4& mat);
    };
}