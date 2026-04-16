#pragma once

#include <glad/glad.h>
#include <core.hpp>

void _GL_ERROR_CHECK(const char* file, int line);

#define ENABLE_GL_ERROR_CHECK
#if defined(ENABLE_GL_ERROR_CHECK)
    #define gl_error_check(glCall) glCall; _GL_ERROR_CHECK(__FILE__, __LINE__) 
#else
    #define gl_error_check(glCall) glCall
#endif