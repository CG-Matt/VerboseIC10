#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "parser.h"
#include "data_structs.h"

// Type Checking
bool is_number(const std::string& str);
bool is_boolean(const std::string& variable);
bool is_reference(const std::string& variable);

// String helpers
bool starts_with(const std::string& data, const std::string& segment);
bool ends_with(const std::string& data, const std::string& segment);
bool includes(const std::string& data, char search_char);

// Vector helpers
std::string join_string(const vmc::string_array_view& view);
std::string join_string(const std::vector<std::string>& data);
std::string join_string(const std::vector<std::string>& data, std::string delim);
std::vector<std::string> split_string(const std::string& str, char delim);
std::string shift(std::vector<std::string>& data);
bool includes(const std::vector<std::string>& data, std::string search_string);

// Custom data parsing
std::string parse_boolean(const std::string& variable);
std::string parse_value(const std::string& variable, const ParserGlobals& globals);
std::vector<std::string> parse_array(const std::vector<std::string>& data, bool& error);
std::vector<std::string> parse_array(const vmc::string_array_view& view, bool& error);

// Comparing functions
std::string B_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label);
std::string BR_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& lines);
