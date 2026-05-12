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

const char* ToOutputPath(const char* cpath)
{
    static std::string output_path;
    output_path = std::filesystem::path(cpath).replace_extension("ic10");
    return output_path.c_str();
}

bool WriteFile(const char* cpath, const std::vector<std::string>& data)
{
    std::ofstream file{ cpath };        // File closed automatically on function exit
    if(!file.is_open()) return false;

    for(const auto& line : data) file << line << "\n";

    return true;
}
