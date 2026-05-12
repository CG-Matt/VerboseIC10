#include <string>
#include <vector>
#include "file_manager.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "cli.hpp"

int main(int argc, char **argv)
{
    CLI::Args::Data args;

    if(!CLI::Args::Parse(args, argc, argv)) { CLI::ShowUsage(); return 1; }

    if(args.show_help){ CLI::ShowUsage(); return 0; }
    if(args.show_version){ CLI::ShowVersion(); return 0; }

    if(!args.input){ CLI::ShowUsage(); return 1; }

    // Read file
    Buffer<char> file_contents = ReadFile(args.input);

    // Parse file
    Parser::init();
    Parser::parse(file_contents);

    // Always log errors if present
    if(Parser::hasError())
    {
        std::puts("ERRORS:");
        for(const std::string& err : Parser::getErrors())
        {
            std::printf("  - %s\n", err.c_str());
        }

        return 1;
    }

    // Export file
    if(!WriteFile(args.output ? args.output : ToOutputPath(args.input), Parser::output))
        std::puts("Error writing to file");

    // Log data if required
    if(args.show_bindings)
    {
        std::puts("REF_TABLE:");
        for(auto& def : Parser::ident::getAll())
        {
            std::printf("  - Int: %s Ext: %s\n", def.second.target.c_str(), def.first.c_str());
        }
    }
    
    return 0;
}