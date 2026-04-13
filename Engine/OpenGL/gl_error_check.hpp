#pragma once

#include <glad/glad.h>
#include <core.hpp>

void _GL_CHECK_ERROR(const char* file, int line);

#define GL_ERROR_CHECK
#if defined(GL_ERROR_CHECK)
    #define gl_check_error(glCall) glCall; _GL_CHECK_ERROR(__FILE__, __LINE__) 
#else
    #define gl_check_error(glCall) glCall
#endif