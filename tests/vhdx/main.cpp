#include <gtest/gtest.h>
#include "vhdx.h"

class VHDXTest : public testing::Test
{
public:
    void SetUp() override {
        if (std::filesystem::exists(testFileName)) {
            std::filesystem::remove(testFileName);
        }
    }

    void TearDown() override { };

protected:
    const std::string testFileName { "test.vhdx" };
    const size_t testFileSize { 8 * 1024 * 1024 };
};

TEST_F(VHDXTest, CreateTest) {
    VHDXOperationResult result;

    result = createVHDX(testFileName, 0);
    EXPECT_FALSE(result.result);
    EXPECT_STREQ("File size is too small (minimum size is : 4194304 bytes)", result.error.c_str());

    result = createVHDX(testFileName, 1);
    EXPECT_FALSE(result.result);
    EXPECT_STREQ("File size is too small (minimum size is : 4194304 bytes)", result.error.c_str());

    result = createVHDX(testFileName, (4 * 1024 * 1024) - 1);
    EXPECT_FALSE(result.result);
    EXPECT_STREQ("File size is too small (minimum size is : 4194304 bytes)", result.error.c_str());

    result = createVHDX(testFileName, testFileSize);
    EXPECT_TRUE(result.result);
    EXPECT_EQ(0, result.error.length());
}
