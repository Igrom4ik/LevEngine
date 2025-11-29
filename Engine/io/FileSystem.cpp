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

#include <fstream>


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
        // Prefer executable-folder/assets if it exists
        auto execAssets = std::filesystem::weakly_canonical(GetExecutableFolder() / "assets");
        if (std::filesystem::exists(execAssets)) {
            return execAssets;
        }

        // Fallback: search upward for App/assets or assets in parent directories (dev convenience)
        auto cur = GetExecutableFolder();
        for (int i = 0; i < 6; ++i) {
            // Check for App/assets
            auto candidate1 = std::filesystem::weakly_canonical(
                cur / std::filesystem::path(std::string(i, '\\') + "../") / "App" / "assets");
            (void) candidate1; // placeholder to avoid unused warning in some builds
            try {
                auto up = cur;
                for (int j = 0; j <= i; ++j) up = up.parent_path();
                auto p1 = std::filesystem::weakly_canonical(up / "App" / "assets");
                if (std::filesystem::exists(p1)) return p1;
                auto p2 = std::filesystem::weakly_canonical(up / "assets");
                if (std::filesystem::exists(p2)) return p2;
            } catch (...) {
                // ignore and continue
            }
        }

        // Default to executable-folder/assets even if it doesn't exist (previous behavior)
        return std::filesystem::weakly_canonical(GetExecutableFolder() / "assets");
    }

    std::vector<char> FileSystem::LoadFile(const std::filesystem::path &path) const {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return {};
        }

        auto size = file.tellg();
        file.seekg(0);

        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            return {};
        }
        return buffer;
    }

    std::vector<char> FileSystem::LoadAssetFile(const std::string &relativePath) const {
        return LoadFile(GetAssetsFolder() / relativePath);
    }

    std::string FileSystem::LoadAssetTextFile(const std::string &relativePath) {
        auto buffer = LoadAssetFile(relativePath);
        return std::string(buffer.begin(), buffer.end());
    }
} // LEN
