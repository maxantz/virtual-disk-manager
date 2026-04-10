#include "vhdx.h"
#include <iostream>

constexpr size_t MIN_FILESIZE{ 4*1024*1024 };
bool createVHDX(const std::filesystem::path& fileName, const size_t& fileSize) {
    if (std::filesystem::exists(fileName)) {
        std::cerr<<fileName<<" already exists"<<std::endl;
        return false;
    }

    if (fileSize < MIN_FILESIZE) {
        std::cerr<<"File size is too small"<<std::endl;
        return false;
    }

    return true;
}

