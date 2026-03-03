#include "../Common/sfe_common.hpp"

#include "sfe_error.hpp"

static const char* ERROR_STRINGS[(int)Error::COUNT] = {
    stringify(ERROR_SUCCESS),
    stringify(ERROR_RESOURCE_NOT_FOUND),
    stringify(ERROR_RESOURCE_TOO_BIG),
    stringify(ERROR_NULL_PARAMETER),
    stringify(ERROR_INVALID_PARAMETER),
};

const char* getErrorString(Error error_code) {
    RUNTIME_ASSERT(((int)error_code >= 0) && ((int)error_code < (int)Error::COUNT));

    return ERROR_STRINGS[(int)error_code];
}