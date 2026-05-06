#pragma once

#include <string>
#include <string_view>
#include <vector>

/**
 * Joins the strings in the provided `data` vector with `delim` placed between them.
 * Copies the data and creates a new string.
 */
std::string JoinString(const std::vector<std::string>& data, const std::string& delim);

/**
 * Splits the provided string view by the delimiter character.
 * Copies the data and creates a new vector of strings.
 */
std::vector<std::string> SplitString(std::string_view str, char delim);
