#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include "file_manager.hpp"

Buffer<char> ReadFile(const char* path)
{
    if(!std::filesystem::exists(path))
        throw std::runtime_error(std::string("No such file or directory: ") + path);

    // Initialise buffer with file size as its size
    Buffer<char> buffer(std::filesystem::file_size(path));

    std::ifstream file(path, std::ios::binary);
    if(!file)
        throw std::runtime_error(std::string("Failed to open file: ") + path);

    if(!file.read(buffer.data, buffer.size))
        throw std::runtime_error(std::string("Failed to read file: ") + path);

    return buffer;
}

std::string StripName(const char* path)
{
    std::filesystem::path file_path(path);

    return file_path.stem().string();
}

bool WriteFile(std::string_view file_path, std::string_view file_name, const std::vector<std::string>& data)
{
    std::ofstream file{ std::filesystem::path(file_path).append(file_name).concat(".ic10") };    // File closed automatically on function exit
    if(!file.is_open()) return false;

    for(auto& line : data) file << line << "\n";

    return true;
}

Config ReadConfigurationFile(const char* path)
{
    Config config {};

    std::ifstream file{ std::filesystem::path(path) };          // File closed automatically on function exit
    if(!file.is_open())
    {
        throw std::runtime_error("Failed to open config file, please ensure \"parser.config\" is in the same directory as the executable");
    }

    std::string key;
    std::string value;

    while(file >> key >> value)
    {
        if(key == "LOG_OUTPUT")
            config.log_output = value == "TRUE";
        
        if(key == "LOG_REF_TABLE")
            config.log_ref_table = value == "TRUE";
        
        if(key == "OUT_FOLDER_PATH")
            config.out_folder_path = value;
    }

    return config;
}
