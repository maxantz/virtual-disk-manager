#include "vhdx.h"
#include <iostream>
#include <climits>
#include <fstream>

constexpr size_t BLOCK_SIZE { USHRT_MAX + 1 };
constexpr char BLOC_64KB[BLOCK_SIZE] { };
constexpr size_t MIN_FILESIZE { 4*1024*1024 };

bool createVHDX(const std::filesystem::path& fileName, const size_t& fileSize) {
    if (std::filesystem::exists(fileName)) {
        std::cerr<<fileName<<" already exists"<<std::endl;
        return false;
    }

    if (fileSize < MIN_FILESIZE) {
        std::cerr<<"File size is too small"<<std::endl;
        return false;
    }

    // Create file
    std::ofstream ofs(fileName, std::ios::binary);
    if (ofs.bad()) {
        std::cerr<<"Failed to create file"<<std::endl;
        return false;
    }

    // Write file identifier [0-65535]
    struct VHDX_FILE_IDENTIFIER sVHDXFileIdentifier;
    ofs.write(reinterpret_cast<char*>(&sVHDXFileIdentifier), VHDX_FILE_IDENTIFIER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_FILE_IDENTIFIER_SIZE));

    // Write header 1 [65536-131071]
    struct VHDX_HEADER sVHDXHeader1;
    ofs.write(reinterpret_cast<char*>(&sVHDXHeader1), VHDX_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));

    // Write header 2 [131072-196607]
    struct VHDX_HEADER sVHDXHeader2;
    ofs.write(reinterpret_cast<char*>(&sVHDXHeader2), VHDX_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));

    // Write region 1 [196608-262143]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion1;
    ofs.write(reinterpret_cast<char*>(&sVHDXRegion1), VHDX_REGION_TABLE_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_REGION_TABLE_HEADER_SIZE));

    // Write region 2 [262144-327679]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion2;
    ofs.write(reinterpret_cast<char*>(&sVHDXRegion2), VHDX_REGION_TABLE_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_REGION_TABLE_HEADER_SIZE));

    // Write reserved [327680-1048575]
    for (size_t i = 5; i < 16; i++) {
        ofs.write(BLOC_64KB, BLOCK_SIZE);
    }

    // Close file
    ofs.close();

    return true;
}

