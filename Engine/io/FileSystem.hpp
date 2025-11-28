#pragma once
#include <filesystem>
#include <vector>

namespace LEN {
    class FileSystem {
    public:
        std::filesystem::path GetExecutableFolder() const;

        std::filesystem::path GetAssetsFolder() const;

        std::vector<char> LoadFile(const std::filesystem::path &path) const;

        std::vector<char> LoadAssetFile(const std::string &relativePath) const;

        std::string LoadAssetTextFile(const std::string &relativePath);

    private:
    };
}

