#include <cstdio>
#include <string_view>
#include "cli.hpp"

namespace CLI
{
    const char* _program_name = nullptr;
}

bool CLI::Args::Parse(Args::Data& args_dest, int argc, char** args)
{
    _program_name = args[0];

    for(int i = 1; i < argc; i++)
    {
        std::string_view argument = args[i];

        if(argument == "--help")
        {
            args_dest.show_help = true;
            return true;
        }
        if(argument == "-o")
        {
            if(i >= argc - 1) return false;

            args_dest.output = args[++i];
            continue;
        }
        if(argument == "-B")
        {
            args_dest.show_bindings = true;
            continue;
        }
        if(argument == "--version")
        {
            args_dest.show_version = true;
            continue;
        }

        if(i == argc - 1)
        {
            args_dest.input = args[i];
            continue;
        }
        else
            return false;
    }

    return true;
}

void CLI::ShowUsage(void)
{
    std::printf("Usage: %s [options]... input\n", _program_name);
    std::printf("  --help\t\tDisplay this information.\n");
    std::printf("  --version\t\tDisplay the current version of the software.\n");
    std::printf("  -o <filename>\t\tSet the output file path.\n");
    std::printf("\t\t\tDefaults to input file name with extension set to \".ic10\".\n");
    std::printf("  -B\t\t\tDisplay the output identifier to register / device bindings.\n");
}

void CLI::ShowVersion(void)
{
    std::printf("victc (Verbose IC Ten Compiler) 0.1.6\n");
}
