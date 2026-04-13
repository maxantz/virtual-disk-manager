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
   EXPECT_FALSE(createVHDX(testFileName, 0));
   EXPECT_FALSE(createVHDX(testFileName, 1));
   EXPECT_FALSE(createVHDX(testFileName, (4 * 1024 * 1024) - 1));
   EXPECT_TRUE(createVHDX(testFileName, testFileSize));
}

