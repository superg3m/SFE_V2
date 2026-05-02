#include <chrono>
#include "platform.hpp"

std::chrono::steady_clock::time_point start = {};

namespace Platform {
	bool init() {
		start = std::chrono::steady_clock::now();
		LOG_DEBUG("Platform initalization!\n");

		return true;
	}

	void shutdown() {
		LOG_DEBUG("Platform Shutdown!\n");
	}

	void sleep(u32 ms) {
		Sleep((DWORD)ms);
	}

	double get_seconds_elapsed() {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		return (double)(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) / 1000.0;
	}

	bool file_exists(const char* path) {
		return (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
	}

	bool copy_file(const char* source_path, const char* dest_path, bool block_until_success) {
		if (block_until_success) {
			while (!CopyFileA(source_path, dest_path, FALSE)) {
				Platform::sleep(10);
			}

			return true;
		} 

		return CopyFileA(source_path, dest_path, FALSE) == 0;
	}

	u8* read_entire_file(Allocator a, const char* file_path, size_t& out_file_size, Error& error) {
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
			LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: platform_read_entire_file(%s)\n", file_path);
			error = Error::RESOURCE_NOT_FOUND;
			
			return nullptr;
		}

		LARGE_INTEGER large_int;
		Memory::zero(&large_int, sizeof(LARGE_INTEGER));
		BOOL success = GetFileSizeEx(file_handle, &large_int);
		if (!success) {
			LOG_ERROR("GetFileSizeEx() Failed to get size from file_handle: platform_read_entire_file(%s)\n", file_path);
			error = Error::RESOURCE_NOT_FOUND;
			CloseHandle(file_handle);
			return nullptr;
		}

		size_t file_size = (size_t)large_int.QuadPart;
		if (file_size > SIZE_MAX) {
			LOG_ERROR("File size is bigger than max size: platform_read_entire_file(%s)\n", file_path);
			error = Error::RESOURCE_TOO_BIG;
			CloseHandle(file_handle);

			return nullptr;
		}

		u8* file_data = (u8*)a.malloc(file_size + 1, alignof(char)); // +1 for null-terminator

		DWORD bytes_read = 0;
		success = ReadFile(file_handle, file_data, (DWORD)file_size, &bytes_read, nullptr);
		CloseHandle(file_handle);
		if (!success && bytes_read == file_size) {
			LOG_ERROR("ReadFile() Failed to get the file data or bytes read doesn't match file_size: platform_read_entire_file(%s)\n", file_path);
			error = Error::RESOURCE_NOT_FOUND;
			a.free(file_data);
			return nullptr;
		}

		out_file_size = (size_t)file_size;

		return file_data;
	}

	DLL* load_dll(const char* dll_path, Error& error)  {
		HMODULE library = LoadLibraryA(dll_path);
		if (!library) {
			LOG_ERROR("LoadLibraryA() failed: platform_load_dll(%s)\n", dll_path);
			error = Error::RESOURCE_NOT_FOUND;

			return nullptr;
		}

		return library;
	}

	DLL* free_dll(DLL* dll)  {
		RUNTIME_ASSERT(dll);
		FreeLibrary((HMODULE)dll);

		return nullptr;
	}

	void* get_function_address(DLL* dll, const char* proc_name) {
		RUNTIME_ASSERT(dll);

		void* proc = (void*)GetProcAddress((HMODULE)dll, proc_name);
		if (!proc) {
			LOG_ERROR("GetProcAddress() failed: platform_get_function_address(%s)\n", proc_name);
			return nullptr;
		}

		return proc;
	}

	FileTime get_file_modification_time(const char* file_path) {
		HANDLE file_handle = CreateFileA(
			file_path, 
			GENERIC_READ, 
			FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
			nullptr, 
			OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, 
			nullptr
		);

		FileTime ret = {0};
		if (file_handle == INVALID_HANDLE_VALUE) {
			// LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: platform_get_file_modification_time(%s)\n", file_path);
			return ret;
		}
		
		if(!GetFileTime(file_handle, nullptr, nullptr, &ret.time)) { 
			CloseHandle(file_handle);
			LOG_ERROR("Failed to get file modification time\n");

			return ret;
		}

		CloseHandle(file_handle);
		return ret;
	}

	// NOTE(Jovanni): For the purposes of hot reloading i'm just gonna say that if either are invalid then they are equal...
	bool compare_file_modification_time(FileTime a, FileTime b) {
		return CompareFileTime(&a.time, &b.time) != 1;
	}

	INTERNAL_LINKAGE void* win32_malloc(void* ctx, size_t allocation_size, size_t alignment) {
		UNUSED(ctx);
		return VirtualAlloc(nullptr, allocation_size, MEM_COMMIT, PAGE_READWRITE);
	}

	INTERNAL_LINKAGE void win32_free(void* ctx, void* data) {
		UNUSED(ctx);
		VirtualFree(data, 0, MEM_RELEASE);
	}

	Allocator get_allocator() {
		Allocator allocator = {};
		allocator.malloc_cb = win32_malloc;
		allocator.free_cb = win32_free;

		return allocator;
	}
}