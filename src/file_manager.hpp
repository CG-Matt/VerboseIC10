#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "data_structs.hpp"

/**
 * Runtime parser configuration loaded from `parser.config`.
 */
struct Config
{
    /** Prints generated IC10 output when compilation succeeds. */
    bool log_output;

    /** Prints identifier bindings when compilation succeeds. */
    bool log_ref_table;

    /** Directory where generated `.ic10` files are written. */
    std::string out_folder_path;
};

/**
 * Reads the whole file at `path` into an owning byte buffer.
 * Throws `std::runtime_error` if the file cannot be opened or read.
 */
Buffer<char> ReadFile(const char* path);

/**
 * Returns the filename stem for `path`, without directories or extension.
 */
std::string StripName(const char* path);

/**
 * Writes each string in `data` as a line to `<file_path>/<file_name>.ic10`.
 * Returns false if the output file cannot be opened.
 */
bool WriteFile(std::string_view file_path, std::string_view file_name, const std::vector<std::string>& data);

/**
 * Loads parser settings from a whitespace-delimited key/value config file.
 * Throws `std::runtime_error` if the config file cannot be opened.
 */
Config ReadConfigurationFile(const char* path);
