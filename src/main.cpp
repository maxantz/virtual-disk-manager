#include <iostream>
#include "arguments_parser.h"
#include "vhdx.h"

void usage(const char* programName) {
    std::cerr<<std::endl;
    std::cerr<<"Usage:"<<std::endl;
    std::cerr<<"\t"<<programName<<" <required> [optional]"<<std::endl;
    std::cerr<<"\t"<<programName<<" <informational>"<<std::endl;
    std::cerr<<std::endl;
    std::cerr<<"Required:"<<std::endl;
    std::cerr<<"\t-C <filename>\t\tcreate virtual disk"<<std::endl;
    std::cerr<<"\t-B <filename>\t\tbind virtual disk"<<std::endl;
    std::cerr<<"\t-U <filename>\t\tunbind virtual disk"<<std::endl;
    std::cerr<<"\t-I <filename>\t\tinitialize virtual disk"<<std::endl;
    std::cerr<<std::endl;
    std::cerr<<"Optional:"<<std::endl;
    std::cerr<<"\tCreate virtual disk"<<std::endl;
    std::cerr<<"\t\t-s <filesize>\tvirtual disk size, accept size modifiers kb Mb Gb Tb B kB MB GB TB"<<std::endl;
    std::cerr<<std::endl;
    std::cerr<<"\tInitialize virtual disk"<<std::endl;
    std::cerr<<"\t\t-m MBR\t\tMBR mode"<<std::endl;
    std::cerr<<"\t\t-m GPT\t\tGUID partition mode"<<std::endl;
    std::cerr<<std::endl;
    std::cerr<<"Informational:"<<std::endl;
    std::cerr<<"\t-H\t\t\tthis help text"<<std::endl;
    std::cerr<<std::endl;
}

int main(int argc, char** argv) {
    struct Options opts;

    if (!parseArguments(argc, argv, opts)) {
        std::cerr<<std::endl<<opts.parseError<<std::endl;
        usage(argv[0]);
        return -1;
    }

    VHDXOperationResult result;

    switch(opts.op) {
    case Operation::Create:
        result = createVHDX(opts.fileName, opts.fileSize);
        if (!result.result) {
            std::cerr<<result.error<<std::endl;
        }
        break;
    case Operation::Bind:
        break;
    case Operation::Unbind:
        break;
    case Operation::Initialize:
        break;
    case Operation::Help:
    default:
        usage(argv[0]);
        break;
    }

    return 0;
}
