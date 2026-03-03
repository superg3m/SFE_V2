#include "sfe_platform.hpp"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_APPLE)
    #include <unistd.h>
    #include <dlfcn.h>
    #include <stdio.h>

    #include "../Core/Common/sfe_logger.hpp"
    #include "../Core/Common/sfe_assert.hpp"
    #include "../Core/Error/sfe_error.hpp"
    #include "../Core/Memory/sfe_memory.hpp"
    
    namespace Platform {
        bool Init() { return true; }
        void Shutdown() {}
        double GetSecondsElapsed() { return 0.0; }

        void SleepForMilliseconds(u32 ms) {
            usleep(ms * 1000);
        }

        bool FilePathExists(const char* path) {
            FILE *fptr = fopen(path, "r");

            if (fptr == nullptr) {
                return false;
            }

            fclose(fptr);

            return true;
        }

        int CopyFile(const char* source_path, const char* dest_path, int block_until_success) {
            const size_t BUFFER_SIZE = 4096;

            for (;;) {
                FILE* in = fopen(source_path, "rb");
                if (!in) {
                    if (!block_until_success) return 0;
                    SleepForMilliseconds(10);
                    continue;
                }

                FILE* out = fopen(dest_path, "wb");
                if (!out) {
                    fclose(in);
                    if (!block_until_success) return 0;
                    SleepForMilliseconds(10);
                    continue;
                }

                char buffer[BUFFER_SIZE];
                size_t bytes;
                int success = 1;

                while ((bytes = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
                    if (fwrite(buffer, 1, bytes, out) != bytes) {
                        success = 0;
                        break;
                    }
                }

                fclose(in);
                fclose(out);

                if (success) return 1;
                if (!block_until_success) return 0;
                SleepForMilliseconds(10);
            }
        }

        u8* ReadEntireFile( const char* file_name, size_t& out_file_size, Error& error) {
            FILE* file_handle = fopen(file_name, "r");
            if (file_handle == nullptr) {
                LOG_ERROR("Invalid file_handle, the file_name/path is likely wrong: ReadEntireFile(%s)\n", file_name);
                error =Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            if (fseek(file_handle, 0L, SEEK_END) != 0) {
                LOG_ERROR("fseek failed: ReadEntireFile(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return nullptr;
            }

            out_file_size = ftell(file_handle);
            if (out_file_size == -1L) {
                LOG_ERROR("ftell failed: ReadEntireFile(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return nullptr;
            }

            rewind(file_handle);
            if (ferror(file_handle)) {
                LOG_ERROR("rewind() failed: ReadEntireFile(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                
                return nullptr;
            }

            u8* file_data = (u8*)Memory::Malloc((size_t)out_file_size + 1); // +1 for null terminator
            if (fread(file_data, out_file_size, 1, file_handle) != 1) {
                LOG_ERROR("fread() failed: ReadEntireFile(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                Memory::Free(file_data);
                fclose(file_handle);

                return nullptr;
            }

            fclose(file_handle);

            out_file_size = out_file_size + 1;

            return file_data;
        }

        DLL LoadDLL(const char* dll_path, Error& error)  {
            DLL library = dlopen(dll_path, RTLD_LAZY);
            if (!library) {
                LOG_ERROR("dlopen() failed: LoadDLL(%s)\n", dll_path);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL freeDLL(DLL dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            dlclose(dll);

            return nullptr;
        }

        void* GetProcAddress(DLL dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = dlsym(dll, proc_name);
            if (!proc) {
                LOG_ERROR("dlsym() failed: ckg_os_GetProcAddress(%s)\n", proc_name);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return proc;
        }
    }
#endif