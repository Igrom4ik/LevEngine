#pragma once
#include <filesystem>

namespace LEN {
    class FileSystem {
        std::filesystem::path GetExecutableFolder() const;

        std::filesystem::path GetAssetsFolder() const;

    public:
    private:
    };
}

