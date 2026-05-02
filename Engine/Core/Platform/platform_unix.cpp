#include "platform.hpp"
#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

#include <chrono>
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
		struct timespec ts;
		ts.tv_sec = 0;           // seconds
		ts.tv_nsec = ms * 1000000L; // 1 million nanoseconds = 1ms
		nanosleep(&ts, NULL);
	}

	double get_seconds_elapsed() {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		return (double)(std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count()) / 1000.0;
	}

	bool file_exists(const char* path) {
		FILE *fptr = fopen(path, "r");

		if (fptr == nullptr) {
			return false;
		}

		fclose(fptr);

		return true;
	}

	bool copy_file(const char* source_path, const char* dest_path, bool block_until_success) {
		const size_t BUFFER_SIZE = 4096;

		while (true) {
			FILE* in = fopen(source_path, "rb");
			if (!in) {
				if (!block_until_success) return false;
				Platform::sleep(10);
				continue;
			}

			FILE* out = fopen(dest_path, "wb");
			if (!out) {
				fclose(in);
				if (!block_until_success) return false;
				Platform::sleep(10);
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

			if (success) return true;
			if (!block_until_success) return false;
			Platform::sleep(10);
		}
	}

	u8* read_entire_file(Allocator a, const char* file_name, size_t& out_file_size, Error& error) {
		FILE* file_handle = fopen(file_name, "r");
		if (file_handle == nullptr) {
			LOG_ERROR("Invalid file_handle, the file_name/path is likely wrong: read_entire_file(%s)\n", file_name);
			error =Error::RESOURCE_NOT_FOUND;

			return nullptr;
		}

		if (fseek(file_handle, 0L, SEEK_END) != 0) {
			LOG_ERROR("fseek failed: read_entire_file(%s)\n", file_name);
			error = Error::RESOURCE_NOT_FOUND;
			fclose(file_handle);
			return nullptr;
		}

		out_file_size = ftell(file_handle);
		if (out_file_size == -1L) {
			LOG_ERROR("ftell failed: read_entire_file(%s)\n", file_name);
			error = Error::RESOURCE_NOT_FOUND;
			fclose(file_handle);
			return nullptr;
		}

		rewind(file_handle);
		if (ferror(file_handle)) {
			LOG_ERROR("rewind() failed: read_entire_file(%s)\n", file_name);
			error = Error::RESOURCE_NOT_FOUND;
			fclose(file_handle);
			
			return nullptr;
		}

		u8* file_data = (u8*)a.malloc((size_t)out_file_size + 1, alignof(char)); // +1 for null terminator
		if (fread(file_data, out_file_size, 1, file_handle) != 1) {
			LOG_ERROR("fread() failed: read_entire_file(%s)\n", file_name);
			error = Error::RESOURCE_NOT_FOUND;
			a.free(file_data);
			fclose(file_handle);

			return nullptr;
		}

		fclose(file_handle);

		return file_data;
	}

	DLL* load_dll(const char* dll_path, Error& error)  {
		DLL* library = dlopen(dll_path, RTLD_LAZY);
		if (!library) {
			LOG_ERROR("dlopen() failed: platform_load_dll(%s)\n", dll_path);
			error = Error::RESOURCE_NOT_FOUND;

			return nullptr;
		}

		return library;
	}

	DLL* free_dll(DLL* dll)  {
		RUNTIME_ASSERT(dll);
		dlclose(dll);

		return nullptr;
	}

	void* get_function_address(DLL* dll, const char* proc_name) {
		RUNTIME_ASSERT(dll);

		void* proc = dlsym(dll, proc_name);
		if (!proc) {
			LOG_ERROR("dlsym() failed: get_function_address(%s)\n", proc_name);

			return nullptr;
		}

		return proc;
	}

	FileTime get_file_modification_time(const char* file_path) {
		FileTime file_time = {}; 

		if (!Platform::file_exists(file_path)) {
			// LOG_ERROR("file_name/path is likely wrong: get_file_modification_time(%s)\n", file_path);
			return file_time;
		}

		struct stat attr;
		if (stat(file_path, &attr) == -1) {
			LOG_ERROR("Error getting file stats\n");
			return file_time;
		}

		// NOTE(Jovanni): attr.st_mtime holds the last modification time
		time_t file_timestamp = attr.st_mtime;
		file_time.time = file_timestamp;
		return file_time;
	}

	bool compare_file_modification_time(FileTime a, FileTime b) {
		return a.time == b.time;
	}

	INTERNAL_LINKAGE void* unix_malloc(void* ctx, size_t allocation_size, size_t alignment) {
		UNUSED(ctx);
		UNUSED(alignment);
		return malloc(allocation_size);
	}

	INTERNAL_LINKAGE void unix_free(void* ctx, void* data) {
		UNUSED(ctx);
		free(data);
	}

	INTERNAL_LINKAGE void* unix_realloc(void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment) {
		UNUSED(ctx);
		UNUSED(old_allocation_size);
		UNUSED(alignment);
		return realloc(data, new_allocation_size);
	}

	Allocator get_allocator() {
		Allocator allocator = {};
		allocator.ctx = nullptr;
		allocator.malloc_cb = unix_malloc;
		allocator.free_cb = unix_free;

		return allocator;
	}
}