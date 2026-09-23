#ifndef _ImagingIO_Transaction_h_
#define _ImagingIO_Transaction_h_

// Private transaction primitives; not part of the ImagingIO public API.
#include <Core/Core.h>
#include <filesystem>
#ifndef PLATFORM_WIN32
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Upp { namespace Imaging { namespace IOTransaction {

inline bool Reserve(const std::filesystem::path& path, std::error_code& error)
{
#ifdef PLATFORM_WIN32
	HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
	                          CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(file == INVALID_HANDLE_VALUE) {
		error = std::error_code(GetLastError(), std::system_category());
		return false;
	}
	CloseHandle(file);
#else
	int file = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
	if(file < 0) {
		error = std::error_code(errno, std::generic_category());
		return false;
	}
	close(file);
#endif
	error.clear();
	return true;
}

inline bool ReserveTemporary(const std::filesystem::path& destination,
                             const String& extension,
                             std::filesystem::path& temporary,
                             std::error_code& error)
{
	for(int attempt = 0; attempt < 16; ++attempt) {
#ifdef PLATFORM_WIN32
		auto process = GetCurrentProcessId();
#else
		auto process = getpid();
#endif
		String unique = AsString(Uuid::Create());
		temporary = destination;
		temporary += ".imagingio-" + std::to_string(process) + "-" +
		             unique.Begin() + extension.Begin();
		if(Reserve(temporary, error))
			return true;
		if(error != std::errc::file_exists)
			return false;
	}
	return false;
}

inline bool Promote(const std::filesystem::path& temporary,
                    const std::filesystem::path& destination,
                    std::error_code& error)
{
	// Both names are in the same directory. Never stage/delete the destination
	// first, and never fall back to a cross-volume copy/delete operation.
#ifdef PLATFORM_WIN32
	if(!MoveFileExW(temporary.c_str(), destination.c_str(), MOVEFILE_REPLACE_EXISTING)) {
		error = std::error_code(GetLastError(), std::system_category());
		return false;
	}
	error.clear();
#else
	std::filesystem::rename(temporary, destination, error);
#endif
	return !error;
}

}}}
#endif
