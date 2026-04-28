#pragma once

#include "../Core/core.hpp"

namespace Platform {
	typedef void DLL;
	struct FileTime {
		#if defined(PLATFORM_WINDOWS)
			FILETIME time;
		#else
			time_t time;
		#endif
	};

	bool initialize();
	void shutdown();
	void sleep(u32 ms);
	double get_seconds_elapsed();
	bool file_exists(const char* path);
	/**
	 * @brief returns true if copy succeeded
	 * 
	 * @param source_path 
	 * @param dest_path 
	 * @param block_until_success
	 */
	bool copy_file(const char* source_path, const char* dest_path, bool block_until_success = true);
	u8* read_entire_file(Allocator a, const char* file_path, size_t& out_file_size, Error& error);
	DLL* load_dll(const char* dll_path, Error& error);
	DLL* free_dll(DLL* dll);
	void* get_function_address(DLL* dll, const char* proc_name, Error& error);

	char* get_executable_directory();
	char* get_asset_directory();

	FileTime get_file_modification_time(const char* file_path);

	/**
	 * @brief
	 *  if returns false: the file times are false
	 *  if returns true: The file times are equal
	 * 
	 * @param a 
	 * @param b
	 * @return bool 
	 */
	bool compare_file_modification_time(FileTime a, FileTime b);
	Allocator get_allocator();
}