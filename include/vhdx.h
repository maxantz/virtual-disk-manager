#ifndef __VHDX_H__
#define __VHDX_H__

#include <filesystem>

/**
 *  ** Header **
 *     |----------|----------|----------|----------|----------|----------|
 *     |   File   |  Header  |  Header  |  Region  |  Region  | Reserved |
 *     |Identifier|    1     |    2     |  Table   |  Table   |          |
 *     |          |          |          |    1     |    2     |          |
 *     |----------|----------|----------|----------|----------|----------|
 *     0B       64kB       128kB      192kB      256kB      320kB       1MB
 *0x00000000 0x00010000 0x00020000 0x00030000 0x00040000 0x00050000 0x00100000
 */

/**
 *     |----------|----------|----------|----------|----------|----------|----------|----------|----------|
 *     |  Header  |   Log    |   BAT    |  Sector  |   Data   |          | Metadata |          |   Data   |
 *     |          |          |          |  Bitmap  |          |          |  Region  |          |          |
 *     |----------|----------|----------|----------|----------|----------|----------|----------|----------|
 *    0B         1MB
 *0x00000000 0x00010000                       0x00011000            0x00020000
 */

struct VHDX_GUID {                                                                      //   16 octets
    uint32_t  Data1 { 0 };                                                              //    4 octets
    uint16_t  Data2 { 0 };                                                              //    2 octets
    uint16_t  Data3 { 0 };                                                              //    2 octets
    uint8_t   Data4[8] { 0 };                                                           //    8 octets
};

struct VHDX_FILE_IDENTIFIER {                                                           //  520 octets
    uint64_t  Signature { 0x656C696678646876 }; // 0x656C696678646876 -> "vhdxfile"     //    8 octets
    uint16_t  Creator[256] { 0x0056, 0x0069,    // "Virtual Disk Manager\0"             //  512 octets
        0x0072, 0x0074, 0x0075, 0x0061, 0x006C, 0x0020, 0x0044, 0x0069, 0x0073, 0x006B,
        0x0020, 0x004D, 0x0061, 0x006E, 0x0061, 0x0067, 0x0065, 0x0072, 0x0000 };
};

const size_t VHDX_FILE_IDENTIFIER_SIZE { sizeof(struct VHDX_FILE_IDENTIFIER) };

struct VHDX_HEADER {                                                                    // 4096 octets
    uint32_t  Signature { 0x64616568 }; // 0x64616568 -> "head"                         //    4 octets
    uint32_t  Checksum { 0 };           // 0x40D8ECD8              0xFA7020C2           //    4 octets
    uint64_t  SequenceNumber { 0 };     // 0x0600000000000000      0x07000000000000000  //    8 octets
    VHDX_GUID FileWriteGuid { };        // 0x0EFF4560B389E545BCAC2ADD55ADC2B3           //   16 octets
    VHDX_GUID DataWriteGuid { };        // 0xAD9F747ED5E3E34296C6B0FD494C3ED0           //   16 octets
    VHDX_GUID LogGuid { };              // 0x6CCA86B605FF73469BAE8A29A82598DB           //   16 octets
    uint16_t  LogVersion { 0 };         // 0x0000                                       //    2 octets
    uint16_t  Version { 0 };            // 0x0100                                       //    2 octets
    uint32_t  LogLength { 0 };          // 0x00001000                                   //    4 octets
    uint64_t  LogOffset { 0 };          // 0x0000100000000000                           //    8 octets
    uint8_t   Reserved[4016] { 0 };                                                     // 4016 octets
};

const size_t VHDX_HEADER_SIZE { sizeof(struct VHDX_HEADER) };

struct VHDX_REGION_TABLE_HEADER {                                                       //   32 octets
    uint32_t  Signature { 0x69676572 }; // 0x69676572 -> "regi"                         //    4 octets
    uint32_t  Checksum { 0 };                                                           //    4 octets
    uint32_t  EntryCount { 0 };                                                         //    4 octets
    uint32_t  Reserved { 0 };                                                           //    4 octets
};

const size_t VHDX_REGION_TABLE_HEADER_SIZE { sizeof(struct VHDX_REGION_TABLE_HEADER) };

struct VHDX_REGION_TABLE_ENTRY {                                                        //   32 octets
    VHDX_GUID  Guid;                                                                    //   16 octets
    uint64_t   FileOffset { 0 };                                                        //    8 octets
    uint32_t   Length { 0 };                                                            //    4 octets
    uint32_t   Required:1;                                                              //    4 octets    1 bits
    uint32_t   Reserved:31;                                                             //    0 octet    31 bits
};

const size_t VHDX_REGION_TABLE_ENTRY_SIZE { sizeof(struct VHDX_REGION_TABLE_ENTRY) };

struct VHDX_LOG_ENTRY_HEADER {                                                          //   64 octets
    uint32_t    Signature { 0x65676F6C };   // 0x65676F6C -> "loge"                     //    4 octets
    uint32_t    Checksum { 0 };                                                         //    4 octets
    uint32_t    EntryLength { 0 };                                                      //    4 octets
    uint32_t    Tail { 0 };                                                             //    4 octets
    uint64_t    SequenceNumber { 0 };                                                   //    8 octets
    uint32_t    DescriptorCount { 0 };                                                  //    4 octets
    uint32_t    Reserved { 0 };                                                         //    4 octets
    VHDX_GUID   LogGuid;                                                                //   16 octets
    uint64_t    FlushedFileOffset { 0 };                                                //    8 octets
    uint64_t    LastFileOffset { 0 };                                                   //    8 octets
};

const size_t VHDX_LOG_ENTRY_HEADER_SIZE { sizeof(struct VHDX_LOG_ENTRY_HEADER) };

struct VHDX_LOG_ZERO_DESCRIPTOR {                                                       //   32 octets
    uint32_t    ZeroSignature { 0x6F72657A };   // 0x6F72657A -> "zero"                 //    4 octets
    uint32_t    Reserved { 0 };                                                         //    4 octets
    uint64_t    ZeroLength { 0 };                                                       //    8 octets
    uint64_t    FileOffset { 0 };                                                       //    8 octets
    uint64_t    SequenceNumber { 0 };                                                   //    8 octets
};

const size_t VHDX_LOG_ZERO_DESCRIPTOR_SIZE { sizeof(struct VHDX_LOG_ZERO_DESCRIPTOR) };

struct VHDX_LOG_DATA_DESCRIPTOR {                                                       //   32 octets
    uint32_t    DataSignature { 0x63736564 };   // 0x63736564 -> "desc"                 //    4 octets
    uint32_t    TrailingBytes { 0 };                                                    //    4 octets
    uint64_t    LeadingBytes { 0 };                                                     //    8 octets
    uint64_t    FileOffset { 0 };                                                       //    8 octets
    uint64_t    SequenceNumber { 0 };                                                   //    8 octets
};

const size_t VHDX_LOG_DATA_DESCRIPTOR_SIZE { sizeof(struct VHDX_LOG_DATA_DESCRIPTOR) };

struct VHDX_LOG_DATA_SECTOR {                                                           // 4096 octets
    uint32_t    DataSignature { 0x61746164 };   // 0x61746164 -> "data"                 //    4 octets
    uint32_t    SequenceHigh { 0 };                                                     //    4 octets
    uint8_t     Data[4084] { 0 };                                                       // 4084 octets
    uint32_t    SequenceLow { 0 };                                                      //    4 octets
};

const size_t VHDX_LOG_DATA_SECTOR_SIZE { sizeof(struct VHDX_LOG_DATA_SECTOR) };

struct VHDX_BAT_ENTRY {                                                                 //    8 octets
    uint64_t    State:3;                                                                //    8 octets
    uint64_t    Reserved:17;                                                            //    0 octet
    uint64_t    FileOffsetMB:44;                                                        //    0 octet
};

const size_t VHDX_BAT_ENTRY_SIZE { sizeof(struct VHDX_BAT_ENTRY) };

#define PAYLOAD_BLOCK_NOT_PRESENT 0
#define PAYLOAD_BLOCK_UNDEFINED 1
#define PAYLOAD_BLOCK_ZERO 2
#define PAYLOAD_BLOCK_UNMAPPED 3
#define PAYLOAD_BLOCK_FULLY_PRESENT 6
#define PAYLOAD_BLOCK_PARTIALLY_PRESENT 7

#define SB_BLOCK_NOT_PRESENT 0
#define SB_BLOCK_PRESENT 6

struct VHDX_METADATA_TABLE_HEADER {                                                     //   32 octets
    uint64_t    Signature { 0x617461646174656D };   // 0x617461646174656D -> "metadata" //    8 octets
    uint16_t    Reserved { 0 };                                                         //    2 octets
    uint16_t    EntryCount { 0 };                                                       //    2 octets
    uint32_t    Reserved2[5] { 0 };                                                     //   20 octets
};

const size_t VHDX_METADATA_TABLE_HEADER_SIZE { sizeof(struct VHDX_METADATA_TABLE_HEADER) };

struct VHDX_METADATA_TABLE_ENTRY {                                                      //   32 octets
    VHDX_GUID   ItemId;                                                                 //   16 octets
    uint32_t    Offset { 0 };                                                           //    4 octets
    uint32_t    Length { 0 };                                                           //    4 octets
    uint32_t    IsUser:1;                                                               //    4 octets    1 bit
    uint32_t    IsVirtualDisk:1;                                                        //    0 octet     1 bit
    uint32_t    IsRequired:1;                                                           //    0 octet     1 bit
    uint32_t    Reserved:29;                                                            //    0 octet    29 bits
    uint32_t    Reserved2 { 0 };                                                        //    4 octets
};

const size_t VHDX_METADATA_TABLE_ENTRY_SIZE { sizeof(struct VHDX_METADATA_TABLE_ENTRY) };

struct VHDX_FILE_PARAMETERS {                                                           //    8 octets
    uint32_t    BlockSize { 0 };                                                        //    4 octets
    uint32_t    LeaveBlocksAllocated:1;                                                 //    4 octets    1 bit
    uint32_t    HasParent:1;                                                            //    0 octets    1 bit
    uint32_t    Reserved:30;                                                            //    0 octet    30 bits
};

const size_t VHDX_FILE_PARAMETERS_SIZE { sizeof(struct VHDX_FILE_PARAMETERS) };

struct VHDX_VIRTUAL_DISK_SIZE {                                                         //    8 octets
    uint64_t    VirtualDiskSize { 0 };                                                  //    8 octets
};

const size_t VHDX_VIRTUAL_DISK_SIZE_SIZE { sizeof(struct VHDX_VIRTUAL_DISK_SIZE) };

struct VHDX_PAGE83_DATA {                                                               //   16 octets
    VHDX_GUID    Page83Data;                                                            //   16 octets
};

const size_t VHDX_PAGE83_DATA_SIZE { sizeof(struct VHDX_PAGE83_DATA) };

struct VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE {                                          //    4 octets
    uint32_t    LogicalSectorSize { 0 };                                                //    4 octets
};

const size_t VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE_SIZE { sizeof(struct VHDX_VIRTUAL_DISK_LOGICAL_SECTOR_SIZE) };

struct VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE {                                         //    4 octets
    uint32_t    PhysicalSectorSize { 0 };                                               //    4 octets
};

const size_t VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE_SIZE { sizeof(struct VHDX_VIRTUAL_DISK_PHYSICAL_SECTOR_SIZE) };

struct VHDX_PARENT_LOCATOR_HEADER {                                                     //   20 octets
    VHDX_GUID   LocatorType;                                                            //   16 octets
    uint16_t    Reserved { 0 };                                                         //    2 octets
    uint16_t    KeyValueCount { 0 };                                                    //    2 octets
};

const size_t VHDX_PARENT_LOCATOR_HEADER_SIZE { sizeof(struct VHDX_PARENT_LOCATOR_HEADER) };

struct VHDX_PARENT_LOCATOR_ENTRY {                                                      //   12 octets
    uint32_t    KeyOffset { 0 };                                                        //    4 octets
    uint32_t    ValueOffset { 0 };                                                      //    4 octets
    uint16_t    KeyLength { 0 };                                                        //    2 octets
    uint16_t    ValueLength { 0 };                                                      //    2 octets
};

const size_t VHDX_PARENT_LOCATOR_ENTRY_SIZE { sizeof(struct VHDX_PARENT_LOCATOR_ENTRY) };

static const VHDX_GUID BAT_VHDX_GUID{ 0x2DC27766, 0xF623, 0x4200, {0x9D, 0x64, 0x11, 0x5E, 0x9B, 0xFD, 0x4A, 0x08 } };
static const VHDX_GUID METADATA_VHDX_GUID{ 0x8B7CA206, 0x4790, 0x4B9A, { 0xB8, 0xFE, 0x57, 0x5F, 0x05, 0x0F, 0x88, 0x6E } };
static const VHDX_GUID PARENT_LOCATOR_VHDX_GUID{ 0xB04AEFB7, 0xD19E, 0x4A81, { 0xB7, 0x89, 0x25, 0xB8, 0xE9, 0x44, 0x59, 0x13 } };

static const VHDX_GUID FILE_PARAMETERS{ 0xCAA16737, 0xFA36, 0x4D43, { 0xB3, 0xB6, 0x33, 0xF0, 0xAA, 0x44, 0xE7, 0x6B } };
static const VHDX_GUID VIRTUAL_DISK_SIZE{ 0x2FA54224, 0xCD1B, 0x4876, { 0xB2, 0x11, 0x5D, 0xBE, 0xD8, 0x3B, 0xF4, 0xB8 } };
static const VHDX_GUID PAGE_83_DATA{ 0xBECA12AB, 0xB2E6, 0x4523, { 0x93, 0xEF, 0xC3, 0x09, 0xE0, 0x00, 0xC7, 0x46 } };
static const VHDX_GUID LOGICAL_SECTOR_SIZE{ 0x8141BF1D, 0xA96F, 0x4709, { 0xBA, 0x47, 0xF2, 0x33, 0xA8, 0xFA, 0xAB, 0x5F } };
static const VHDX_GUID PHYSICAL_SECTOR_SIZE{ 0xCDA348C7, 0x445D, 0x4471, { 0x9C, 0xC9, 0xE9, 0x88, 0x52, 0x51, 0xC5, 0x56 } };
static const VHDX_GUID PARENTAL_LOCATOR{ 0xA8D35F2D, 0xB30B, 0x454D, { 0xAB, 0xF7, 0xD3, 0xD8, 0x48, 0x34, 0xAB, 0x0C } };

bool createVHDX(const std::filesystem::path& fileName, const size_t& fileSize);

#endif

