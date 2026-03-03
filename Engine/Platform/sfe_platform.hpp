#pragma once

#include "../Core/Common/sfe_common.hpp"
#include "../Core/Error/sfe_error.hpp"
// Date: August 04, 2025
// NOTE(Jovanni): Im cheating here a bit I should have like win32 platform and linux platform but im just gonna do glfw and win32

namespace Platform {
    typedef void* DLL;

    bool Init();
    void Shutdown();
    void SleepForMilliseconds(u32 ms);
    double GetSecondsElapsed();
    bool FilePathExists(const char* path);
    /**
     * @brief returns true if copy succeeded
     * 
     * @param source_path 
     * @param dest_path 
     * @param block_until_success
     */
    bool CopyFile(const char* source_path, const char* dest_path, bool block_until_success = true);
    u8* ReadEntireFile( const char* file_path, size_t& out_file_size, Error& error);
    DLL LoadDLL(const char* dll_path, Error& error);
    DLL freeDLL(DLL dll, Error& error);
    void* GetProcAddress(DLL dll, const char* proc_name, Error& error);
}