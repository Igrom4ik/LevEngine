
#include "FileSystem.hpp"
#include "config.h"

#if defined _WIN32
#include <windows.h>
#elif defined (__APPLE__)
#include <mach-o/dyld.h>
#elif defined (__linux__)
#include <unistd.h>
#include <limits.h>
#endif

namespace LEN {
    std::filesystem::path FileSystem::GetExecutableFolder() const {
#if defined _WIN32
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(NULL, buffer, MAX_PATH);
        return std::filesystem::path(buffer).remove_filename();
#elif defined (__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size); // Get the size needed
        std::string tmp(size, '\0');
        _NSGetExecutablePath(tmp.data(), &size);
        return std::filesystem::weakly_canonical(std::filesystem::path(tmp)).remove_filename();
#elif defined (__linux__)
        return std::filesystem::weakly_canonical(std::filesystem::read_symlink("/proc/self/exe")).remove_filename();
#else
        return std::filesystem::current_path();
#endif
    }

    std::filesystem::path FileSystem::GetAssetsFolder() const {
#if defined (ASSETS_ROOT)
        auto path = std::filesystem::path(std::string(ASSETS_ROOT));
        if (std::filesystem::exists(path)) {
            return path;
        }

#endif
        return std::filesystem::weakly_canonical(GetExecutableFolder() / "assets");
    }
} // LEN
