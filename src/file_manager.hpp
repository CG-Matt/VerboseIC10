#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "data_structs.hpp"

/**
 * Reads the whole file at `path` into an owning byte buffer.
 * Throws `std::runtime_error` if the file cannot be opened or read.
 */
Buffer<char> ReadFile(const char* path);

/**
 * Returns the filename stem for `path`, without directories or extension.
 */
const char* ToOutputPath(const char* input_path);

/**
 * Writes each string in `data` as a line to `<file_path>/<file_name>.ic10`.
 * Returns false if the output file cannot be opened.
 */
bool WriteFile(const char* path, const std::vector<std::string>& data);
