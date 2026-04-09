#include "arguments_parser.h"
#include <sstream>

size_t coefficientMultiplicateur(char* coefficient) {
    const int coefficientLength = strlen(coefficient);
    if (coefficientLength < 1 || coefficientLength > 2) {
        return std::string::npos;
    }
    size_t returnedValue { 1 };
    bool unitSet { false };
    bool decadeSet { false };
    for (int iter = 0; iter < coefficientLength; iter++) {
        switch (coefficient[iter]) {
        case 'T':
            if (unitSet || decadeSet) { return std::string::npos; }
            returnedValue *= 1024;
        case 'G':
            if (unitSet || decadeSet) { return std::string::npos; }
            returnedValue *= 1024;
        case 'M':
            if (unitSet || decadeSet) { return std::string::npos; }
            returnedValue *= 1024;
        case 'k':
            if (unitSet || decadeSet) { return std::string::npos; }
            returnedValue *= 1024;
	    decadeSet = true;
            break;
        case 'B':
            if (unitSet) { return std::string::npos; }
            returnedValue *= 8;
        case 'b':
            if (unitSet) { return std::string::npos; }
            unitSet = true;
            break;
        default:
            return std::string::npos;
            break;
        }
    }
    return returnedValue;
}

bool parseArguments(int argc, char** argv, Options& opts) {
    opts.op = Operation::Help;
    opts.fileName.clear();
    opts.fileSize = std::string::npos;
    opts.mode = InitializeMode::MBR;
    opts.parseError.clear();

    if (argc < 3) return false;

    for (int iter = 1; iter < argc; iter++) {
        if (2 != strlen(argv[iter])) {
            opts.parseError = "Invalid argument length";
            return false;
        }
        if ('-' == argv[iter][0]) {
            opts.parseError = "Invalid argument specifier";
            return false;
        }
        switch(argv[iter][1]) {
        case 'C':
            opts.op = Operation::Create;
	    if (iter+1 < argc) { opts.fileName = argv[iter++]; }
            break;
        case 'B':
            opts.op = Operation::Bind;
	    if (iter+1 < argc) { opts.fileName = argv[iter++]; }
            break;
        case 'U':
            opts.op = Operation::Unbind;
	    if (iter+1 < argc) { opts.fileName = argv[iter++]; }
            break;
        case 'I':
            opts.op = Operation::Initialize;
	    if (iter+1 < argc) { opts.fileName = argv[iter++]; }
            break;
        case 's':
            if (opts.op != Operation::Create) {
                opts.parseError = "invalid create argument";
                return false;
            }
	    if (iter+1 < argc) { std::stringstream ss; ss << argv[iter++]; ss >> opts.fileSize; if (ss.fail()) { opts.parseError = "invalide file size"; return false; } }
            if (iter+1 < argc && 0 < strlen(argv[iter+1]) && strlen(argv[iter+1]) < 3) { opts.fileSize *= coefficientMultiplicateur(argv[iter++]); }
            break;
        case 'm':
            if (opts.op != Operation::Initialize) {
                opts.parseError = "invalid initialize argument";
                return false;
            }
	    if (iter+1 < argc) { }
            break;
        case 'H':
        default:
            opts.op = Operation::Help;
	    return false;
            break;
        }
    }


    return true;
}

