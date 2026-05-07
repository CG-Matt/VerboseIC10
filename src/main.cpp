#include <string>
#include <vector>
#include "file_manager.hpp"
#include "parser.hpp"
#include "utils.hpp"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::puts("Please provide the name of a file to parse");
        return 1;
    }

    const char* file_name = argv[1];

    // Read config file
    Config config = ReadConfigurationFile("./parser.config");

    // Read file
    Buffer<char> file_contents = ReadFile(file_name);

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
    if(!WriteFile(config.out_folder_path, StripName(file_name), Parser::output)){ std::puts("Error writing to file"); }

    // Log data if required
    if(config.log_ref_table)
    {
        std::puts("REF_TABLE:");
        for(auto& def : Parser::ident::getAll())
        {
            std::printf("  - Int: %s Ext: %s\n", def.second.target.c_str(), def.first.c_str());
        }
    }

    if(config.log_output)
    {
        // Some of the compiler errors might have newlines.
        // This ensures that every error gets printed correctly.
        std::puts("OUTPUT:");
        std::string output_str = JoinString(Parser::output, "\n");
        Parser::output = SplitString(output_str, '\n');
        for(auto &line : Parser::output)
        {
            std::printf("  - %s\n", line.c_str());
        }
    }
    
    return 0;
}