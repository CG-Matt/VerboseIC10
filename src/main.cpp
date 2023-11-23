#include <iostream>
#include <string>
#include <vector>
#include "file_manager.h"
#include "parser.h"
#include "utils.h"
#include "data_structs.h"

int main(int argc, char **argv)
{
    std::string file_name;

    if(argc > 1 && sizeof(argv[1]) < 256){ file_name = argv[1]; }

    if(file_name.empty())
    {
        std::cout << "Please provide the name of a file to parse" << std::endl;
        return 1;
    }

    // Read config file
    Config config = read_config_file();

    // Read file
    std::vector<vmc::Line> file_contents = read_file(config.in_folder_path, file_name);

    // Parse file
    Parser parser(file_contents);
    parser.parse();

    // Always log errors if present
    if(parser.has_error())
    {
        std::cout << "ERRORS:" << std::endl;
        for(auto& err : parser.get_errors())
        {
            std::cout << "   - " + err << std::endl;
        }

        return 1;
    }

    // Export file
    int write_err = write_file(config.out_folder_path, file_name, parser.output);
    if(write_err > 0){ std::cout << "Error writing to file"; }

    // Log data if required
    if(config.log_ref_table)
    {
        std::cout << "REF_TABLE:" << std::endl;
        for(auto& def : parser.globals.references.get_all())
        {
            std::cout << "   - " << "Int: " << def.second.m_value << " Ext: " << def.first << std::endl;
        }
    }

    if(config.log_output)
    {
        std::cout << "OUTPUT:" << std::endl;
        std::string output_str = join_string(parser.output, "\n");
        parser.output = split_string(output_str, '\n');
        for(auto &line : parser.output)
        {
            std::cout << "   - " + line << std::endl;
        }
    }
    
    return 0;
}