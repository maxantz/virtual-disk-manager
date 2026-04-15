#include "vhdx.h"
#include <iostream>
#include <climits>
#include <fstream>

#include "crc32c/crc32c.h"

constexpr size_t BLOCK_SIZE { USHRT_MAX + 1 };  // 65536
constexpr char BLOC_64KB[BLOCK_SIZE] { 0 };
constexpr size_t MIN_FILESIZE { 4*1024*1024 };

uint32_t crc32_checksum(const void* data, size_t length) {
    return crc32c::Crc32c(static_cast<const uint8_t*>(data), length);
}

bool createVHDX(const std::filesystem::path& fileName, const size_t& fileSize) {
    if (std::filesystem::exists(fileName)) {
        std::cerr<<fileName<<" already exists"<<std::endl;
        return false;
    }

    if (fileSize < MIN_FILESIZE) {
        std::cerr<<"File size is too small (minimum size is : "<<MIN_FILESIZE<<" bytes)"<<std::endl;
        return false;
    }

    // Create file
    std::ofstream ofs(fileName, std::ios::binary);
    if (ofs.bad()) {
        std::cerr<<"Failed to create file"<<std::endl;
        return false;
    }

    /**************************************************************************
     * VHDX File Header         [0-1048575]         ->  [0x00000000-0x000FFFFF]
     *************************************************************************/

    // Write file identifier    [0-65535]           ->  [0x00000000-0x0000FFFF]
    struct VHDX_FILE_IDENTIFIER sVHDXFileIdentifier;
    ofs.write(reinterpret_cast<char*>(&sVHDXFileIdentifier), VHDX_FILE_IDENTIFIER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_FILE_IDENTIFIER_SIZE));

    // Write header 1           [65536-131071]      ->  [0x00010000-0x0001FFFF]
    struct VHDX_HEADER sVHDXHeader1;
    sVHDXHeader1.SequenceNumber = 0x0000000000000006;
    sVHDXHeader1.FileWriteGuid = { 0x6045FF0E, 0x89B3, 0x45E5, { 0xBC, 0xAC, 0x2A, 0xDD, 0x55, 0xAD, 0xC2, 0xB3 } };
    sVHDXHeader1.DataWriteGuid = { 0x7E749FAD, 0xE3D5, 0x42E3, { 0x96, 0xC6, 0xB0, 0xFD, 0x49, 0x4C, 0x3E, 0xD0 } };
    sVHDXHeader1.LogGuid = { 0xB686CA6C, 0xFF05, 0x4673, { 0x9B, 0xAE, 0x8A, 0x29, 0xA8, 0x25, 0x98, 0xDB } };
    sVHDXHeader1.LogVersion = 0x0000;
    sVHDXHeader1.Version = 0x0001;
    sVHDXHeader1.LogLength = 0x00100000;
    sVHDXHeader1.LogOffset= 0x0000000000100000;
    sVHDXHeader1.Checksum = crc32_checksum(&sVHDXHeader1, VHDX_HEADER_SIZE);

    ofs.write(reinterpret_cast<char*>(&sVHDXHeader1), VHDX_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));

    // Write header 2           [131072-196607]     ->  [0x00020000-0x0002FFFF]
    struct VHDX_HEADER sVHDXHeader2;
    sVHDXHeader2.SequenceNumber = 0x0000000000000007;
    sVHDXHeader2.FileWriteGuid = { 0x6045FF0E, 0x89B3, 0x45E5, { 0xBC, 0xAC, 0x2A, 0xDD, 0x55, 0xAD, 0xC2, 0xB3 } };
    sVHDXHeader2.DataWriteGuid = { 0x7E749FAD, 0xE3D5, 0x42E3, { 0x96, 0xC6, 0xB0, 0xFD, 0x49, 0x4C, 0x3E, 0xD0 } };
    sVHDXHeader2.LogGuid = { 0xB686CA6C, 0xFF05, 0x4673, { 0x9B, 0xAE, 0x8A, 0x29, 0xA8, 0x25, 0x98, 0xDB } };
    sVHDXHeader2.LogVersion = 0x0000;
    sVHDXHeader2.Version = 0x0001;
    sVHDXHeader2.LogLength = 0x00100000;
    sVHDXHeader2.LogOffset= 0x0000000000100000;
    sVHDXHeader2.Checksum = crc32_checksum(&sVHDXHeader2, VHDX_HEADER_SIZE);

    ofs.write(reinterpret_cast<char*>(&sVHDXHeader2), VHDX_HEADER_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));

    // Write region 1           [196608-262143]     ->  [0x00030000-0x0003FFFF]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion1;
    sVHDXRegion1.EntryCount = 0x000000002;
    sVHDXRegion1.Checksum = crc32_checksum(&sVHDXRegion1, VHDX_REGION_TABLE_HEADER_SIZE);

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion1), VHDX_REGION_TABLE_HEADER_SIZE);

    // Write Region 1 table entry 1 (BAT)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion1Entry1;
    sVHDXRegion1Entry1.Guid = { 0x2DC27766, 0xF623, 0x4200, { 0x9D, 0x64, 0x11, 0x5E, 0x9B, 0xFD, 0x4A, 0x08}};
    sVHDXRegion1Entry1.FileOffset = 0x0000000000300000;
    sVHDXRegion1Entry1.Length = 0x00100000;
    sVHDXRegion1Entry1.Required = true;

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion1Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);

    // Write Region 1 table entry 2 (Metadata)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion1Entry2;
    sVHDXRegion1Entry2.Guid = { 0x8B7CA206, 0x4790, 0x4B9A, { 0xB8, 0xFE, 0x57, 0x5F, 0x05, 0x0F, 0x88, 0x6E}};
    sVHDXRegion1Entry2.FileOffset = 0x0000000000200000;
    sVHDXRegion1Entry2.Length = 0x00100000;
    sVHDXRegion1Entry2.Required = true;

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion1Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - (VHDX_REGION_TABLE_HEADER_SIZE + (2 * VHDX_REGION_TABLE_ENTRY_SIZE))));

    // Write region 2           [262144-327679]     ->  [0x00040000-0x0004FFFF]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion2;
    sVHDXRegion2.EntryCount = 0x000000002;
    sVHDXRegion2.Checksum = crc32_checksum(&sVHDXRegion2, VHDX_REGION_TABLE_HEADER_SIZE);

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion2), VHDX_REGION_TABLE_HEADER_SIZE);

    // Write Region 2 table entry 1 (BAT)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion2Entry1;
    sVHDXRegion2Entry1.Guid = { 0x2DC27766, 0xF623, 0x4200, { 0x9D, 0x64, 0x11, 0x5E, 0x9B, 0xFD, 0x4A, 0x08}};
    sVHDXRegion2Entry1.FileOffset = 0x0000000000300000;
    sVHDXRegion2Entry1.Length = 0x00100000;
    sVHDXRegion2Entry1.Required = true;

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion2Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);

    // Write Region 2 table entry 2 (Metadata)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion2Entry2;
    sVHDXRegion2Entry2.Guid = { 0x8B7CA206, 0x4790, 0x4B9A, { 0xB8, 0xFE, 0x57, 0x5F, 0x05, 0x0F, 0x88, 0x6E}};
    sVHDXRegion2Entry2.FileOffset = 0x0000000000200000;
    sVHDXRegion2Entry2.Length = 0x00100000;
    sVHDXRegion2Entry2.Required = true;

    ofs.write(reinterpret_cast<char*>(&sVHDXRegion2Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);
    ofs.write(BLOC_64KB, (BLOCK_SIZE - (VHDX_REGION_TABLE_HEADER_SIZE + 2 * VHDX_REGION_TABLE_ENTRY_SIZE)));

    // Write reserved           [327680-1048575]    ->  [0x00050000-0x000FFFFF]
    for (size_t i = 5; i < 16; i++) {
        ofs.write(BLOC_64KB, BLOCK_SIZE);
    }

    /**************************************************************************
     * VHDX Log                 [1048576-]          ->  [0x00100000-0x001010FF]
     *************************************************************************/
    // Write log 1              [1048576-1052671]   ->  [0x00100000-0x00100FFF]
    struct VHDX_LOG_ENTRY_HEADER sVHDXLogEntry1;
    ofs.write(reinterpret_cast<char*>(&sVHDXLogEntry1), VHDX_LOG_ENTRY_HEADER_SIZE);

    // Write log 2              [1052672-1052735]   ->  [0x00101000-0x0010103F]
    struct VHDX_LOG_ENTRY_HEADER sVHDXLogEntry2;
    ofs.write(reinterpret_cast<char*>(&sVHDXLogEntry2), VHDX_LOG_ENTRY_HEADER_SIZE);

    // Write desc               [1052736-1056767]   ->  [0x00101040-0x00101FFF]
    struct VHDX_LOG_DATA_DESCRIPTOR sVHDXLogDataDescriptor;
    ofs.write(reinterpret_cast<char*>(&sVHDXLogDataDescriptor), VHDX_LOG_ZERO_DESCRIPTOR_SIZE);

    /**************************************************************************
     * VHDX BAT
     *************************************************************************/

    /**************************************************************************
     * VHDX Sector Bitmap
     *************************************************************************/

    /**************************************************************************
     * VHDX Data
     *************************************************************************/
    // Write data               [1056768-1122304]   ->  [0x00102000-0x00112000]
    struct VHDX_LOG_DATA_SECTOR sVHDXLogData;
    for (size_t i = 0; i < 16; i++) {
        ofs.write(reinterpret_cast<char*>(&sVHDXLogData), VHDX_LOG_DATA_SECTOR_SIZE);
    }

    /**************************************************************************
     * VHDX metadata
     *************************************************************************/
    // Write metadata           [-]   ->  [0x00200000-]
    struct VHDX_METADATA_TABLE_HEADER sVHDXMetadata;
    ofs.write(reinterpret_cast<char*>(&sVHDXMetadata), VHDX_METADATA_TABLE_HEADER_SIZE);

    // Close file
    ofs.close();

    return true;
}

