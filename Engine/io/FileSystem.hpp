#pragma once
#include <filesystem>

namespace LEN {
    class FileSystem {
    public:
        std::filesystem::path GetExecutableFolder() const;

        std::filesystem::path GetAssetsFolder() const;

    private:
    };
}

