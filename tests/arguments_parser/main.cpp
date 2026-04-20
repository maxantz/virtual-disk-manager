#include <gtest/gtest.h>
#include "arguments_parser.h"

class ArgumentsParserTest : public testing::Test
{
public:
    ArgumentsParserTest() { };
    ~ArgumentsParserTest() override { };
    void SetUp() override { };
    void TearDown() override { };
};

TEST(NoArgsTest, ArgumentsParserTest) {
    Options opts;

    char** argv = nullptr;
    argv = (char**)malloc(1 * sizeof(char*));
    argv[0] = (char*)malloc(12 * sizeof(char));
    snprintf(argv[0], 12, "program.exe");

    const bool result = parseArguments(1, argv, opts);

    free(argv[0]);
    free(argv);

    EXPECT_FALSE(result);
    EXPECT_EQ(Operation::Help, opts.op);
}

TEST(OneArgsTest, ArgumentsParserTest) {
    Options opts;

    char** argv = nullptr;
    argv = (char**)malloc(2 * sizeof(char*));
    argv[0] = (char*)malloc(12 * sizeof(char));
    argv[1] = (char*)malloc(3 * sizeof(char));
    snprintf(argv[0], 12, "program.exe");
    snprintf(argv[1], 3, "-H");

    const bool result = parseArguments(2, argv, opts);

    free(argv[1]);
    free(argv[0]);
    free(argv);

    EXPECT_FALSE(result);
    EXPECT_EQ(Operation::Help, opts.op);
}

TEST(TwoArgsTest, ArgumentsParserTest) {
    const std::vector<std::pair<std::string, Operation>> operationsToTest { { "-C", Operation::Create }, { "-B", Operation::Bind }, { "-U", Operation::Unbind }, { "-I", Operation::Initialize } }; 
    for (auto it : operationsToTest) {
        Options opts;

        char** argv = nullptr;
        argv = (char**)malloc(3 * sizeof(char*));
        argv[0] = (char*)malloc(12 * sizeof(char));
        argv[1] = (char*)malloc(3 * sizeof(char));
        argv[2] = (char*)malloc(10 * sizeof(char));
        snprintf(argv[0], 12, "program.exe");
        snprintf(argv[1], 3, "%s", it.first.c_str());
        snprintf(argv[2], 10, "test.vhdx");

        const bool result = parseArguments(3, argv, opts);

        free(argv[2]);
        free(argv[1]);
        free(argv[0]);
        free(argv);

        EXPECT_TRUE(result) << opts.parseError;
        EXPECT_EQ(it.second, opts.op) << opts.parseError;
        EXPECT_STREQ("test.vhdx", opts.fileName.c_str()) << opts.parseError;
    }
}

TEST(FourArgsCreateTest, ArgumentsParserTest) {
    Options opts;

    char** argv = nullptr;
    argv = (char**)malloc(5 * sizeof(char*));
    argv[0] = (char*)malloc(12 * sizeof(char));
    argv[1] = (char*)malloc(3 * sizeof(char));
    argv[2] = (char*)malloc(10 * sizeof(char));
    argv[3] = (char*)malloc(3 * sizeof(char));
    argv[4] = (char*)malloc(6 * sizeof(char));
    snprintf(argv[0], 12, "program.exe");
    snprintf(argv[1], 3, "-C");
    snprintf(argv[2], 10, "test.vhdx");
    snprintf(argv[3], 3, "-s");
    snprintf(argv[4], 6, "12345");

    const bool result = parseArguments(5, argv, opts);

    free(argv[4]);
    free(argv[3]);
    free(argv[2]);
    free(argv[1]);
    free(argv[0]);
    free(argv);

    EXPECT_TRUE(result) << opts.parseError;
    EXPECT_EQ(Operation::Create, opts.op) << opts.parseError;
    EXPECT_STREQ("test.vhdx", opts.fileName.c_str()) << opts.parseError;
    EXPECT_EQ(12345, opts.fileSize) << opts.parseError;
}

TEST(FourArgsInitializeTest, ArgumentsParserTest) {
    Options opts;

    char** argv = nullptr;
    argv = (char**)malloc(5 * sizeof(char*));
    argv[0] = (char*)malloc(12 * sizeof(char));
    argv[1] = (char*)malloc(3 * sizeof(char));
    argv[2] = (char*)malloc(10 * sizeof(char));
    argv[3] = (char*)malloc(3 * sizeof(char));
    argv[4] = (char*)malloc(6 * sizeof(char));
    snprintf(argv[0], 12, "program.exe");
    snprintf(argv[1], 3, "-I");
    snprintf(argv[2], 10, "test.vhdx");
    snprintf(argv[3], 3, "-m");
    snprintf(argv[4], 6, "GPT");

    const bool result = parseArguments(5, argv, opts);

    free(argv[4]);
    free(argv[3]);
    free(argv[2]);
    free(argv[1]);
    free(argv[0]);
    free(argv);

    EXPECT_TRUE(result) << opts.parseError;
    EXPECT_EQ(Operation::Initialize, opts.op) << opts.parseError;
    EXPECT_STREQ("test.vhdx", opts.fileName.c_str()) << opts.parseError;
    EXPECT_EQ(InitializeMode::GPT, opts.mode) << opts.parseError;
}

TEST(FiveArgsCreateTest, ArgumentsParserTest) {
    Options opts;

    char** argv = nullptr;
    argv = (char**)malloc(6 * sizeof(char*));
    argv[0] = (char*)malloc(12 * sizeof(char));
    argv[1] = (char*)malloc(3 * sizeof(char));
    argv[2] = (char*)malloc(10 * sizeof(char));
    argv[3] = (char*)malloc(3 * sizeof(char));
    argv[4] = (char*)malloc(6 * sizeof(char));
    argv[5] = (char*)malloc(3 * sizeof(char));
    snprintf(argv[0], 12, "program.exe");
    snprintf(argv[1], 3, "-C");
    snprintf(argv[2], 10, "test.vhdx");
    snprintf(argv[3], 3, "-s");
    snprintf(argv[4], 6, "12");
    snprintf(argv[5], 3, "TB");

    const bool result = parseArguments(6, argv, opts);

    free(argv[5]);
    free(argv[4]);
    free(argv[3]);
    free(argv[2]);
    free(argv[1]);
    free(argv[0]);
    free(argv);

    EXPECT_TRUE(result) << opts.parseError;
    EXPECT_EQ(Operation::Create, opts.op) << opts.parseError;
    EXPECT_STREQ("test.vhdx", opts.fileName.c_str()) << opts.parseError;
    EXPECT_EQ(105553116266496, opts.fileSize) << opts.parseError;
}
