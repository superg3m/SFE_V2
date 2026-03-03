#pragma once

enum class Error : int {
    SUCCESS = 0,
    RESOURCE_NOT_FOUND,
    RESOURCE_TOO_BIG,
    NULL_PARAMETER,
    INVALID_PARAMETER,
    COUNT
};

/**
 * @brief returns a string literal of the error code
 * 
 * @param error_code 
 * @return const char*
 */
const char* getErrorString(Error error_code);