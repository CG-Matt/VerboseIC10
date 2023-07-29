#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include "file_manager.h"

std::vector<std::string> read_file(const std::string& file_path, const std::string &file_name)
{
    std::ifstream file(file_path + "/" + file_name + ".vic10");
    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }

    std::vector<std::string> file_contents;
    std::string line;

    while(getline(file, line))
    {
        file_contents.push_back(line);
    }

    file.close();

    return file_contents;
}

int write_file(const std::string& file_path, const std::string& file_name, const std::vector<std::string>& data)
{
    std::ofstream file(file_path + "/" + file_name + ".ic10", std::ofstream::out);
    if(file.is_open())
    {
        for(auto& line : data)
        {
            file << line << "\n";
        }
        file.close();
        return 0;
    }
    else
    {
        return 1;
    }
}

Config read_config_file()
{
    Config config;

    std::ifstream file("./parser.config");
    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open config file, please ensure \"parser.config\" is in the same directory as the executable");
    }

    std::string key;
    std::string value;

    while(file >> key >> value)
    {
        if(key == "LOG_OUTPUT")
        {
            if(value == "TRUE"){ config.log_output = true; }
            else { config.log_output = false; }
        }
        else if(key == "LOG_REF_TABLE")
        {
            if(value == "TRUE"){ config.log_ref_table = true; }
            else { config.log_ref_table = false; }
        }
        else if(key == "IN_FOLDER_PATH")
        {
            config.in_folder_path = value;
        }
        else if(key == "OUT_FOLDER_PATH")
        {
            config.out_folder_path = value;
        }
        else if(key == "MAX_DEVICE_COUNT")
        {
            config.max_device_count = std::stoi(value);
        }
        else if(key == "MAX_REGISTER_COUNT")
        {
            config.max_register_count = std::stoi(value);
        }
    }

    file.close();

    return config;
}