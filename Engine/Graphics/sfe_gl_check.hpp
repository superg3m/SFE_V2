#pragma once

void glCheckError_(const char *file, int line);
#define GL_ERROR_CHECK
#if defined(GL_ERROR_CHECK)
    #define glCheckError(glCall) glCall; glCheckError_(__FILE__, __LINE__) 
#else
    #define glCheckError(glCall) glCall
#endif