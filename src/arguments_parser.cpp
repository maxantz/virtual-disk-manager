#include "arguments_parser.h"

bool parseArguments(int argc, char** argv, Options& opts) {
    opts.op = Operation::Help;
    opts.fileName.clear();
    opts.fileSize = std::string::npos;
    opts.mode = InitializeMode::MBR;
    opts.parseError.clear();

    if (argc < 3) return false;



    return true;
}

