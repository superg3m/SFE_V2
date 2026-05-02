#include "backend.hpp"

void OpenGL::Texture::flip_vertically_in_place(u8* data, int width, int height) {
	for (int y = 0; y < height / 2; y++) {
		for (int x = 0; x < width; x++) {
			std::swap(data[y * width + x], data[(height - 1 - y) * width + x]);
		}
	}
}

OpenGL::Texture OpenGL::Texture::load_from_file(u32 texture_unit, String path, TextureDescription desc) {
	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
	RUNTIME_ASSERT_MSG(Platform::file_exists(path.data), "Texture path: '%s' doesn't exist!\n", path);

	GLenum TEXTURE_VERTICAL_FLIP = desc.vertical_flip;

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(TEXTURE_VERTICAL_FLIP);

	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
	u8* data = stbi_load(path.data, &width, &height, &nrChannels, 0);
	stbi_set_flip_vertically_on_load(false);

	Texture ret = Texture::load_from_memory(texture_unit, data, width, height, nrChannels, desc);

	stbi_image_free(data);
	data = nullptr;

	if (ret.id == 0) {
		LOG_ERROR("TextureLoader | id is invalid!\n");
	}
	
	return ret;
}

OpenGL::Texture OpenGL::Texture::load_from_memory(u32 texture_unit, const u8* data, int width, int height, int nrChannels, TextureDescription desc) {
	if (!data || width <= 0 || height <= 0 || nrChannels == 0) {
		RUNTIME_ASSERT_MSG(false, "TextureLoader | Invalid input data for loadTextureFromMemory!\n");
	}

	GLenum MIPMAP_TYPE = desc.pixel_perfect ? GL_NEAREST : GL_LINEAR;

	u32 texture = 0;
	gl_error_check(glGenTextures(1, &texture));
	gl_error_check(glBindTexture(GL_TEXTURE_2D, texture));
	gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIPMAP_TYPE));
	gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MIPMAP_TYPE));

	GLenum internal_format = 0;
	GLenum pixel_format = 0;
	if (nrChannels == 1) {
		internal_format = GL_R8;
		pixel_format = GL_RED;
		gl_error_check(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED));
		gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED));
		gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED));
		gl_error_check(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));
	} else if (nrChannels == 3) {
		internal_format = GL_RGB8;
		pixel_format = GL_RGB;
	} else if (nrChannels == 4) {
		internal_format = GL_RGBA8;
		pixel_format = GL_RGBA;
	} else {
		RUNTIME_ASSERT_MSG(false, "Unsupported channel count");
	}

	gl_error_check(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data));
	gl_error_check(glGenerateMipmap(GL_TEXTURE_2D));

	if (texture == 0) {
		RUNTIME_ASSERT_MSG(false, "TextureLoader | id is invalid for memory texture!\n");
	}

	Texture ret;
	ret.texture_unit = texture_unit;
	ret.id = texture;
	ret.width = width;
	ret.height = height;
	ret.type = desc.type;

	gl_error_check(glBindTexture(GL_TEXTURE_2D, 0));

	return ret;
}

// {right, left, top, bottom, front, back}
OpenGL::Texture OpenGL::Texture::load_cube_map(u32 texture_unit, Vector<String> texture_paths) {
	u32 texture = 0;
	gl_error_check(glGenTextures(1, &texture));
	gl_error_check(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
	gl_error_check(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	gl_error_check(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	gl_error_check(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	gl_error_check(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	gl_error_check(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	unsigned char *data = nullptr;  
	for(int i = 0; i < texture_paths.count; i++) {
		INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(texture_paths[i]);
		RUNTIME_ASSERT_MSG(Platform::file_exists(texture_paths[i].data), "Texture path: '%s' doesn't exist!\n", texture_paths[i].data);
		int width, height, nrChannels;
		data = stbi_load(texture_paths[i].data, &width, &height, &nrChannels, 0);
		
		GLenum format = 0;
		if (nrChannels == 1) {
			format = GL_RED;
		} else if (nrChannels == 3) {
			format = GL_RGB;
		} else if (nrChannels == 4) {
			format = GL_RGBA;
		} else {
			LOG_ERROR("TextureLoader | Failed to pick a stb format, most likely related to assimp, try to link your libraries in a different order\n");
		}

		if (data) {
			gl_error_check(glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
			));
		} else {
			LOG_ERROR("TextureLoader | Failed to load texture\n");
		}

		stbi_image_free(data);
	}

	Texture ret;
	ret.texture_unit = texture_unit;
	ret.id = texture;
	ret.type = TextureSamplerType::CUBEMAP_3D;

	gl_error_check(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

	return ret;
}