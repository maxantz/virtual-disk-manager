#include "vhdx.h"
#include <iostream>
#include <climits>
#include <fstream>
#include <cstring>

#include "crc32c/crc32c.h"

constexpr size_t BLOCK_SIZE { USHRT_MAX + 1 };  // 65536
constexpr char BLOC_64KB[BLOCK_SIZE] { 0 };
constexpr size_t MIN_FILESIZE { 4*1024*1024 };

size_t writeData(std::ofstream& ofs, char* data, const size_t& dataLength) {
    ofs.write(data, dataLength);

    return dataLength;
}

size_t writeBlankData(std::ofstream& ofs, size_t bytesToWiteLength) {
    size_t writtenBytes { bytesToWiteLength };

    // fast write by 64kB block
    while (bytesToWiteLength > BLOCK_SIZE) {
        ofs.write(BLOC_64KB, BLOCK_SIZE);
        bytesToWiteLength -= BLOCK_SIZE;
    }
    // write remaining bytes
    if (bytesToWiteLength > 0) {
        ofs.write(BLOC_64KB, bytesToWiteLength);
        bytesToWiteLength = 0;
    }

    return writtenBytes;
}

uint32_t crc32_checksum(const void* data, size_t data_length, size_t total_length) {
    if (total_length <= data_length) {
        return crc32c::Crc32c(static_cast<const uint8_t*>(data), total_length);
    }

    uint32_t result { 0 };
    uint8_t *tmp = (uint8_t*)malloc(total_length);
    memset(tmp, 0x00, total_length);
    memcpy(tmp, data, data_length);
    result = crc32c::Crc32c(tmp, total_length);
    free(tmp);

    return result;
}

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

    size_t writtenBytes { 0 };

    /**************************************************************************
     * VHDX File Header         [0-1048575]         ->  [0x00000000-0x000FFFFF]
     *************************************************************************/

    // Write file identifier    [0-65535]           ->  [0x00000000-0x0000FFFF]
    struct VHDX_FILE_IDENTIFIER sVHDXFileIdentifier;
    // ofs.write(reinterpret_cast<char*>(&sVHDXFileIdentifier), VHDX_FILE_IDENTIFIER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXFileIdentifier), VHDX_FILE_IDENTIFIER_SIZE);

    // jump to end of 64KB
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_FILE_IDENTIFIER_SIZE));
    writtenBytes += writeBlankData(ofs, 0x00010000 - writtenBytes);

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

    // ofs.write(reinterpret_cast<char*>(&sVHDXHeader1), VHDX_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXHeader1), VHDX_HEADER_SIZE);

    // jump to end of 64KB
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));
    writtenBytes += writeBlankData(ofs, 0x00020000 - writtenBytes);

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

    // ofs.write(reinterpret_cast<char*>(&sVHDXHeader2), VHDX_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXHeader2), VHDX_HEADER_SIZE);

    // jump to end of 64KB
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - VHDX_HEADER_SIZE));
    writtenBytes += writeBlankData(ofs, 0x00030000 - writtenBytes);

    // Write region 1           [196608-262143]     ->  [0x00030000-0x0003FFFF]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion1;
    sVHDXRegion1.EntryCount = 0x000000002;
    sVHDXRegion1.Checksum = crc32_checksum(&sVHDXRegion1, VHDX_REGION_TABLE_HEADER_SIZE);

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion1), VHDX_REGION_TABLE_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion1), VHDX_REGION_TABLE_HEADER_SIZE);

    // Write Region 1 table entry 1 (BAT)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion1Entry1;
    sVHDXRegion1Entry1.Guid = { 0x2DC27766, 0xF623, 0x4200, { 0x9D, 0x64, 0x11, 0x5E, 0x9B, 0xFD, 0x4A, 0x08}};
    sVHDXRegion1Entry1.FileOffset = 0x0000000000300000;
    sVHDXRegion1Entry1.Length = 0x00100000;
    sVHDXRegion1Entry1.Required = true;

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion1Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion1Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);

    // Write Region 1 table entry 2 (Metadata)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion1Entry2;
    sVHDXRegion1Entry2.Guid = { 0x8B7CA206, 0x4790, 0x4B9A, { 0xB8, 0xFE, 0x57, 0x5F, 0x05, 0x0F, 0x88, 0x6E}};
    sVHDXRegion1Entry2.FileOffset = 0x0000000000200000;
    sVHDXRegion1Entry2.Length = 0x00100000;
    sVHDXRegion1Entry2.Required = true;

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion1Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion1Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);

    // jump to end of 64KB
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - (VHDX_REGION_TABLE_HEADER_SIZE + (2 * VHDX_REGION_TABLE_ENTRY_SIZE))));
    writtenBytes += writeBlankData(ofs, 0x00040000 - writtenBytes);

    // Write region 2           [262144-327679]     ->  [0x00040000-0x0004FFFF]
    struct VHDX_REGION_TABLE_HEADER sVHDXRegion2;
    sVHDXRegion2.EntryCount = 0x000000002;
    sVHDXRegion2.Checksum = crc32_checksum(&sVHDXRegion2, VHDX_REGION_TABLE_HEADER_SIZE);

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion2), VHDX_REGION_TABLE_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion2), VHDX_REGION_TABLE_HEADER_SIZE);

    // Write Region 2 table entry 1 (BAT)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion2Entry1;
    sVHDXRegion2Entry1.Guid = { 0x2DC27766, 0xF623, 0x4200, { 0x9D, 0x64, 0x11, 0x5E, 0x9B, 0xFD, 0x4A, 0x08}};
    sVHDXRegion2Entry1.FileOffset = 0x0000000000300000;
    sVHDXRegion2Entry1.Length = 0x00100000;
    sVHDXRegion2Entry1.Required = true;

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion2Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion2Entry1), VHDX_REGION_TABLE_ENTRY_SIZE);

    // Write Region 2 table entry 2 (Metadata)
    struct VHDX_REGION_TABLE_ENTRY sVHDXRegion2Entry2;
    sVHDXRegion2Entry2.Guid = { 0x8B7CA206, 0x4790, 0x4B9A, { 0xB8, 0xFE, 0x57, 0x5F, 0x05, 0x0F, 0x88, 0x6E}};
    sVHDXRegion2Entry2.FileOffset = 0x0000000000200000;
    sVHDXRegion2Entry2.Length = 0x00100000;
    sVHDXRegion2Entry2.Required = true;

    // ofs.write(reinterpret_cast<char*>(&sVHDXRegion2Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXRegion2Entry2), VHDX_REGION_TABLE_ENTRY_SIZE);

    // jump to end of 64KB
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - (VHDX_REGION_TABLE_HEADER_SIZE + 2 * VHDX_REGION_TABLE_ENTRY_SIZE)));

    // Write reserved           [327680-1048575]    ->  [0x00050000-0x000FFFFF]
    // for (size_t i = 5; i < 16; i++) {
    //     ofs.write(BLOC_64KB, BLOCK_SIZE);
    // }
    writtenBytes += writeBlankData(ofs, 0x00100000 - writtenBytes);

    /**************************************************************************
     * VHDX Log                 [1048576-]          ->  [0x00100000-0x001010FF]
     *************************************************************************/
    // Write log 1              [1048576-1052671]   ->  [0x00100000-0x00100FFF]
    struct VHDX_LOG_ENTRY_HEADER sVHDXLogEntry1;
    sVHDXLogEntry1.EntryLength = 0x00001000;    // Next entry
    sVHDXLogEntry1.Tail = 0x00000000;
    sVHDXLogEntry1.SequenceNumber = 0x74DE700C4F98FA7B;
    sVHDXLogEntry1.DescriptorCount = 0x0000;
    sVHDXLogEntry1.Reserved = 0x0000;
    sVHDXLogEntry1.LogGuid = { 0xB686CA6C, 0xFF05, 0x4673, { 0x9B, 0xAE, 0x8A, 0x29, 0xA8, 0x25, 0x98, 0xDB } };
    sVHDXLogEntry1.FlushedFileOffset = 0x0000000000400000;
    sVHDXLogEntry1.LastFileOffset = 0x0000000000400000;
    sVHDXLogEntry1.Checksum = crc32_checksum(&sVHDXLogEntry1, VHDX_LOG_ENTRY_HEADER_SIZE, sVHDXLogEntry1.EntryLength);
    
    // ofs.write(reinterpret_cast<char*>(&sVHDXLogEntry1), VHDX_LOG_ENTRY_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXLogEntry1), VHDX_LOG_ENTRY_HEADER_SIZE);

    // jump to end of EntryLength
    // ofs.write(BLOC_64KB, (sVHDXLogEntry1.EntryLength - (sVHDXLogEntry1.Tail + VHDX_LOG_ENTRY_HEADER_SIZE)));
    writtenBytes += writeBlankData(ofs, (sVHDXLogEntry1.EntryLength - (sVHDXLogEntry1.Tail + VHDX_LOG_ENTRY_HEADER_SIZE)));

    // Write log 2              [1048576-1052735]   ->  [0x00101000-0x0010103F]
    struct VHDX_LOG_ENTRY_HEADER sVHDXLogEntry2;
    sVHDXLogEntry2.EntryLength = 0x00002000;    // Next entry
    sVHDXLogEntry2.Tail = 0x00001000;           // Current Entry
    sVHDXLogEntry2.SequenceNumber = 0x74DE700C4F98FA7C;
    sVHDXLogEntry2.DescriptorCount = 0x0001;
    sVHDXLogEntry2.Reserved = 0x0000;
    sVHDXLogEntry2.LogGuid = { 0xB686CA6C, 0xFF05, 0x4673, { 0x9B, 0xAE, 0x8A, 0x29, 0xA8, 0x25, 0x98, 0xDB } };
    sVHDXLogEntry2.FlushedFileOffset = 0x0000000000400000;
    sVHDXLogEntry2.LastFileOffset = 0x0000000000400000;
    sVHDXLogEntry2.Checksum = crc32_checksum(&sVHDXLogEntry2, VHDX_LOG_ENTRY_HEADER_SIZE, sVHDXLogEntry1.EntryLength);
    
    // ofs.write(reinterpret_cast<char*>(&sVHDXLogEntry2), VHDX_LOG_ENTRY_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXLogEntry2), VHDX_LOG_ENTRY_HEADER_SIZE);
    
    // Write desc              [1052736-1052767]   ->  [0x00101040-0x0010105F]
    struct VHDX_LOG_DATA_DESCRIPTOR sVHDXLogDataDescriptor;
    sVHDXLogDataDescriptor.TrailingBytes = 0x00000000;
    sVHDXLogDataDescriptor.LeadingBytes = 0x0000000000000002;
    sVHDXLogDataDescriptor.FileOffset = 0x0000000000300000;
    sVHDXLogDataDescriptor.SequenceNumber = 0x74DE700C4F98FA7C;
    
    // ofs.write(reinterpret_cast<char*>(&sVHDXLogDataDescriptor), VHDX_LOG_DATA_DESCRIPTOR_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXLogDataDescriptor), VHDX_LOG_DATA_DESCRIPTOR_SIZE);

    // ofs.write(BLOC_64KB, (sVHDXLogEntry2.EntryLength - (sVHDXLogEntry2.Tail + VHDX_LOG_ENTRY_HEADER_SIZE)));
    writtenBytes += writeBlankData(ofs, (sVHDXLogEntry2.EntryLength - (sVHDXLogEntry2.Tail + VHDX_LOG_ENTRY_HEADER_SIZE + VHDX_LOG_DATA_DESCRIPTOR_SIZE)));

    /**************************************************************************
     * VHDX BAT
     *************************************************************************/

    /**************************************************************************
     * VHDX Sector Bitmap
     *************************************************************************/

    /**************************************************************************
     * VHDX Data
     *************************************************************************/
    // Write data               [1056768-1056767]   ->  [0x00102000-0x00102FFF]
    struct VHDX_LOG_DATA_SECTOR sVHDXLogDatas[16];
    sVHDXLogDatas[0].SequenceHigh = 0x74DE700C;
    sVHDXLogDatas[0].SequenceLow = 0x4F98FA7C;
    for (size_t iter = 1; iter < 16; ++iter) {
        sVHDXLogDatas[iter].SequenceHigh = 0x689286FB;
        sVHDXLogDatas[iter].SequenceLow = 0x0A80A312;
    }
    for (size_t iter = 0; iter < 16; ++iter) {
        // ofs.write(reinterpret_cast<char*>(&sVHDXLogDatas[iter]), VHDX_LOG_DATA_SECTOR_SIZE);
        writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXLogDatas[iter]), VHDX_LOG_DATA_SECTOR_SIZE);
    }

    // jump to end of 64K
    // ofs.write(BLOC_64KB, (BLOCK_SIZE - (VHDX_LOG_ENTRY_HEADER_SIZE + (16 * VHDX_LOG_DATA_SECTOR_SIZE))));
    writtenBytes += writeBlankData(ofs, 0x00200000 - writtenBytes);

    /**************************************************************************
     * VHDX metadata
     *************************************************************************/
    // Write metadata table      [2097152-2097183]   ->  [0x00200000-0x0020001F]
    struct VHDX_METADATA_TABLE_HEADER sVHDXMetadataTable;
    sVHDXMetadataTable.EntryCount = 0x0005;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataTable), VHDX_METADATA_TABLE_HEADER_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataTable), VHDX_METADATA_TABLE_HEADER_SIZE);
    
    struct VHDX_METADATA_TABLE_ENTRY sVHDXMetadataTableEntries[7];
    sVHDXMetadataTableEntries[0].ItemId = FILE_PARAMETERS;
    sVHDXMetadataTableEntries[0].Offset = 0x00010000;
    sVHDXMetadataTableEntries[0].Length = 0x00000008;
    sVHDXMetadataTableEntries[0].IsUser = false;
    sVHDXMetadataTableEntries[0].IsVirtualDisk = false;
    sVHDXMetadataTableEntries[0].IsRequired = true;
    sVHDXMetadataTableEntries[0].Reserved = 0;
    
    sVHDXMetadataTableEntries[1].ItemId = VIRTUAL_DISK_SIZE;
    sVHDXMetadataTableEntries[1].Offset = 0x00010008;
    sVHDXMetadataTableEntries[1].Length = 0x00000008;
    sVHDXMetadataTableEntries[1].IsUser = false;
    sVHDXMetadataTableEntries[1].IsVirtualDisk = true;
    sVHDXMetadataTableEntries[1].IsRequired = true;
    sVHDXMetadataTableEntries[1].Reserved = 0;
    
    sVHDXMetadataTableEntries[2].ItemId = LOGICAL_SECTOR_SIZE;
    sVHDXMetadataTableEntries[2].Offset = 0x00010010;
    sVHDXMetadataTableEntries[2].Length = 0x00000004;
    sVHDXMetadataTableEntries[2].IsUser = false;
    sVHDXMetadataTableEntries[2].IsVirtualDisk = true;
    sVHDXMetadataTableEntries[2].IsRequired = true;
    sVHDXMetadataTableEntries[2].Reserved = 0;
    
    sVHDXMetadataTableEntries[3].ItemId = PHYSICAL_SECTOR_SIZE;
    sVHDXMetadataTableEntries[3].Offset = 0x00010014;
    sVHDXMetadataTableEntries[3].Length = 0x00000004;
    sVHDXMetadataTableEntries[3].IsUser = false;
    sVHDXMetadataTableEntries[3].IsVirtualDisk = true;
    sVHDXMetadataTableEntries[3].IsRequired = true;
    sVHDXMetadataTableEntries[3].Reserved = 0;
    
    sVHDXMetadataTableEntries[4].ItemId = PAGE_83_DATA;
    sVHDXMetadataTableEntries[4].Offset = 0x00010018;
    sVHDXMetadataTableEntries[4].Length = 0x00000010;
    sVHDXMetadataTableEntries[4].IsUser = false;
    sVHDXMetadataTableEntries[4].IsVirtualDisk = true;
    sVHDXMetadataTableEntries[4].IsRequired = true;
    sVHDXMetadataTableEntries[4].Reserved = 0;
    
    sVHDXMetadataTableEntries[5].ItemId = PAGE_83_DATA;
    sVHDXMetadataTableEntries[5].Offset = 0x00010018;
    sVHDXMetadataTableEntries[5].Length = 0x00000010;
    sVHDXMetadataTableEntries[5].IsUser = false;
    sVHDXMetadataTableEntries[5].IsVirtualDisk = true;
    sVHDXMetadataTableEntries[5].IsRequired = true;
    sVHDXMetadataTableEntries[5].Reserved = 0;

    for (size_t iter = 0; iter < sVHDXMetadataTable.EntryCount; ++iter) {
        // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataTableEntries[iter]), VHDX_METADATA_TABLE_ENTRY_SIZE);
        writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataTableEntries[iter]), VHDX_METADATA_TABLE_ENTRY_SIZE);
    }
    // skip end of block
    for (size_t iter = sVHDXMetadataTable.EntryCount+1; iter < 2048; ++iter) {
        // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataTableEntries[6]), VHDX_METADATA_TABLE_ENTRY_SIZE);
        writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataTableEntries[6]), VHDX_METADATA_TABLE_ENTRY_SIZE);
    }

    // Write parameters file
    struct VHDX_FILE_PARAMETERS sVHDXMetadataFileParameters;
    sVHDXMetadataFileParameters.BlockSize = 0x02000000;
    sVHDXMetadataFileParameters.LeaveBlocksAllocated = false;
    sVHDXMetadataFileParameters.HasParent = false;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataFileParameters), VHDX_FILE_PARAMETERS_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataFileParameters), VHDX_FILE_PARAMETERS_SIZE);

    // Write virtual disk size
    struct VHDX_VIRTUAL_DISK_SIZE sVHDXMetadataVirtualDiskSize;
    sVHDXMetadataVirtualDiskSize.VirtualDiskSize = 0x0000000008000000;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskSize), VHDX_VIRTUAL_DISK_SIZE_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskSize), VHDX_VIRTUAL_DISK_SIZE_SIZE);

    // Write logical sector size
    struct VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE sVHDXMetadataVirtualDiskLogicalSectorSize;
    sVHDXMetadataVirtualDiskLogicalSectorSize.LogicalSectorSize = 0x00000200;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskLogicalSectorSize), VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskLogicalSectorSize), VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE_SIZE);

    // Write physical sector size
    struct VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE sVHDXMetadataVirtualDiskPhysicalSectorSize;
    sVHDXMetadataVirtualDiskPhysicalSectorSize.PhysicalSectorSize = 0x00001000;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskPhysicalSectorSize), VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataVirtualDiskPhysicalSectorSize), VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE_SIZE);

    // Write page 83
    struct VHDX_PAGE83_DATA sVHDXMetadataPage83_1;
    sVHDXMetadataPage83_1.Page83Data = { 0x7E749FAD, 0xE3D5, 0x42E3, { 0x96, 0xC6, 0xB0, 0xFD, 0x49, 0x4C, 0x3E, 0xD0 } };
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataPage83_1), VHDX_PAGE83_DATA_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataPage83_1), VHDX_PAGE83_DATA_SIZE);

    // Write page 83
    struct VHDX_PAGE83_DATA sVHDXMetadataPage83_2;
    // ofs.write(reinterpret_cast<char*>(&sVHDXMetadataPage83_2), VHDX_PAGE83_DATA_SIZE);
    writtenBytes += writeData(ofs, reinterpret_cast<char*>(&sVHDXMetadataPage83_2), VHDX_PAGE83_DATA_SIZE);

    // Fill file until 4MB
    writtenBytes += writeBlankData(ofs, 0x00400000 - writtenBytes);

    // Close file
    ofs.close();

    return true;
}

