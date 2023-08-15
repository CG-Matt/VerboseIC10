#pragma once

#include <vector>
#include <string>
#include "data_structs.h"

struct Config
{
    bool log_output;
    bool log_ref_table;
    std::string in_folder_path;
    std::string out_folder_path;
    uint8_t max_device_count;
    uint8_t max_register_count;
};

std::vector<vmc::Line> read_file(const std::string& file_path, const std::string &file_name);
int write_file(const std::string& file_path, const std::string& file_name, const std::vector<std::string>& data);

Config read_config_file();