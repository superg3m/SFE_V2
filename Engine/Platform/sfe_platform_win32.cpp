#include "sfe_platform.hpp"

#if defined(PLATFORM_WINDOWS)
    #if !defined(NOMINMAX)
        #define NOMINMAX
    #endif

    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #undef CopyFile

    #include <windowsx.h>
    #include <timeapi.h>

    #include "../Core/Common/sfe_logger.hpp"
    #include "../Core/Common/sfe_assert.hpp"
    #include "../Core/Memory/sfe_memory.hpp"
    #include "../Core/Error/sfe_error.hpp"

    namespace Platform {
        double g_frequency = {0};
        double g_start_time = {0};
        TIMECAPS g_device_time_caps;

        bool Init() {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            g_frequency = (double)frequency.QuadPart;

            LARGE_INTEGER start_time;
            QueryPerformanceCounter(&start_time);
            g_start_time = (double)start_time.QuadPart;
            
            if (timeGetDevCaps(&g_device_time_caps, sizeof(TIMECAPS)) != MMSYSERR_NOERROR) {
                MessageBoxA(0, "Failed to get timecaps!", "Error!", MB_ICONEXCLAMATION | MB_OK);

                return false;
            }

            LOG_DEBUG("Platform initalization!\n");
            return true;
        }

        void Shutdown() {
            timeEndPeriod(g_device_time_caps.wPeriodMin);
            LOG_DEBUG("Platform Shutdown!\n");
        }

        bool FilePathExists(const char* path) {
            return (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
        }

        bool CopyFile(const char* source_path, const char* dest_path, bool block_until_success) {
            if (block_until_success) {
                while (!CopyFileA(source_path, dest_path, FALSE)) {
                    Platform::SleepForMilliseconds(10);
                }

                return true;
            } 

            return CopyFileA(source_path, dest_path, FALSE) == 0;
         }

        void SleepForMilliseconds(u32 ms) {
            Sleep((DWORD)ms);
        }

        double GetSecondsElapsed() {
            LARGE_INTEGER counter = {0};
            QueryPerformanceCounter(&counter);
            double time_in_seconds = (double)(counter.QuadPart - g_start_time) / (double)g_frequency;
        
            return time_in_seconds;
        }

        /*
        void* win32_malloc(const Memory::BaseAllocator** allocator, size_t allocation_size) {
            (void)allocator;
            return VirtualAlloc(nullptr, allocation_size, MEM_COMMIT, PAGE_READWRITE);
        }

        void win32_free(const Memory::BaseAllocator** allocator, void* data) {
            (void)allocator;
            VirtualFree(data, 0, MEM_RELEASE);
        }
        */

        u8* ReadEntireFile( const char* file_path, size_t& out_file_size, Error& error) {
            HANDLE file_handle = CreateFileA(
                file_path, 
                GENERIC_READ, 
                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
                nullptr, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL, 
                nullptr
            );
            if (file_handle == INVALID_HANDLE_VALUE) {
                LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: ReadEntireFile(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                
                return nullptr;
            }

            LARGE_INTEGER large_int;
            Memory::Zero(&large_int, sizeof(LARGE_INTEGER));
            BOOL success = GetFileSizeEx(file_handle, &large_int);
            if (!success) {
                LOG_ERROR("GetFileSizeEx() Failed to get size from file_handle: ReadEntireFile(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                CloseHandle(file_handle);
                return nullptr;
            }

            size_t file_size = (size_t)large_int.QuadPart;
            if (file_size > SIZE_MAX) {
                LOG_ERROR("File size is bigger than max size: ReadEntireFile(%s)\n", file_path);
                error = Error::RESOURCE_TOO_BIG;
                CloseHandle(file_handle);

                return nullptr;
            }

            u8* file_data = (u8*)Memory::Malloc(file_size + 1); // +1 for null-terminator

            DWORD bytes_read = 0;
            success = ReadFile(file_handle, file_data, (DWORD)file_size, &bytes_read, nullptr);
            CloseHandle(file_handle);
            if (!success && bytes_read == file_size) {
                LOG_ERROR("ReadFile() Failed to get the file data or bytes read doesn't match file_size: ReadEntireFile(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                Memory::Free(file_data);
                return nullptr;
            }

            out_file_size = (size_t)file_size;

            return file_data;
        }

        DLL LoadDLL(const char* dll_path, Error& error)  {
            HMODULE library = LoadLibraryA(dll_path);
            if (!library) {
                LOG_ERROR("LoadLibraryA() failed: LoadDLL(%s)\n", dll_path);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL freeDLL(DLL dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            FreeLibrary((HMODULE)dll);

            return nullptr;
        }

        void* GetProcAddress(DLL dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = (void*)GetProcAddress((HMODULE)dll, proc_name);
            if (!proc) {
                LOG_ERROR("GetProcAddress() failed: GetProcAddress(%s)\n", proc_name);
                error = Error::RESOURCE_NOT_FOUND;
                return nullptr;
            }

            return proc;
        }
    }
#endif