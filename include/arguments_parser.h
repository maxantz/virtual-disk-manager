#ifndef __ARGUMENTS_PARSER_H__
#define __ARGUMENTS_PARSER_H__

#include <string>

enum Operation {
    Help = 0,
    Create,
    Bind,
    Unbind,
    Initialize
};

enum InitializeMode {
    MBR = 0,
    GPT
};

struct Options {
   Operation		op;
   std::string		fileName;
   size_t		fileSize;
   InitializeMode	mode;
   std::string		parseError;
};

bool parseArguments(int argc, char** argv, Options& opts);

#endif

