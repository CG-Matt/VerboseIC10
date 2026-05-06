#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include "file_manager.hpp"

Buffer<char> ReadFile(const char* path)
{
    // Initialise buffer with file size as its size
    Buffer<char> buffer(std::filesystem::file_size(path));

    std::ifstream file(path, std::ios::binary);
    if(!file)
    {
        throw std::runtime_error(std::string("Failed to open file: ") + path);
    }

    if(!file.read(buffer.data, buffer.size))
    {
        throw std::runtime_error(std::string("Failed to read file: ") + path);
    }

    return buffer;
}

std::string StripName(const char* path)
{
    std::filesystem::path file_path(path);

    return file_path.stem().string();
}

bool WriteFile(const std::string& file_path, const std::string& file_name, const std::vector<std::string>& data)
{
    std::ofstream file(file_path + "/" + file_name + ".ic10");
    if(!file.is_open()) return false;

    for(auto& line : data)
        file << line << "\n";

    file.close();
    return true;
}

Config ReadConfigurationFile(const char* path)
{
    Config config {};

    std::ifstream file{ std::filesystem::path(path) };
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
