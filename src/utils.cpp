#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <sstream>
#include "parser.h"
#include "instructions.h"
#include "utils.h"

/*Splits the string "str" at every "delim" char*/
std::vector<std::string> split_string(const std::string& str, char delim)
{
    std::stringstream line_stream(str);
    std::string segment;
    std::vector<std::string> out;
    if(std::find(str.begin(), str.end(), delim) == str.end()){ out.push_back(str); return out; }

    while(std::getline(line_stream, segment, delim))
    {
        out.push_back(segment);
    }

    return out;
}

/*Joins all entries in the view into a single string*/
std::string join_string(const vmc::string_array_view& view)
{
    std::stringstream ss;
    for(std::vector<std::string>::iterator it = view.start; it != view.end; it++)
    {
        ss << *it;
    }
    return ss.str();
}

/*Joins all string in the "data" vector into a single string*/
std::string join_string(const std::vector<std::string>& data)
{
    std::stringstream ss;
    for( auto& str : data){ ss << str; }
    return ss.str();
}

/*Joins all string in the "data" vector into a single string joined together with the "delim" char/string*/
std::string join_string(const std::vector<std::string>& data, std::string delim)
{
    std::stringstream ss;
    for(uint32_t i = 0; i < data.size(); i++)
    {
        if(i != 0){ ss << delim; }
        ss << data[i];
    }

    return ss.str();
}

/*Checks if a vector includes a "search_string"*/
bool includes(const std::vector<std::string>& data, const std::string& search_string)
{
    return std::find(data.begin(), data.end(), search_string) != data.end();
}

/*Checks if a string includes a "search_char"*/
bool includes(const std::string& data, char search_char)
{
    return std::find(data.begin(), data.end(), search_char) != data.end();
}

bool is_number(std::string_view str)
{
    if(str.front() == '-'){ str.remove_prefix(1); } 

    bool is_int = str.find_first_not_of("0123456789") == std::string_view::npos;
    if(is_int){ return true; }
    
    bool has_decimal = str.find_first_not_of("0123456789.") == std::string_view::npos;
    if(!has_decimal){ return false; }
    
    bool len_two = str.size() > 1;
    if(!len_two){ return false; }
    
    bool start_with_point = str.front() == '.';
    if(start_with_point){ return false; }
    
    unsigned int points = 0;
    for(auto &ch : str)
    {
        if(ch == '.'){ points++; }
    }
    if(points > 1){ return false; }

    return true;
}

bool is_boolean(const std::string& variable){ return variable == "true" || variable == "false"; }

bool is_reference(const std::string& variable){ return !is_number(variable); }

std::string parse_boolean(const std::string& variable){ return variable == "true" ? "1" : "0"; }

bool starts_with(const std::string& data, const std::string& segment){ return data.rfind(segment, 0) == 0; }

bool ends_with(const std::string& data, const std::string& segment){ return data.rfind(segment) == data.size() - segment.size(); }

std::string B_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label)
{
    std::string comp = comparators.at(comparator);

    return value == "0" ? ins::branch_z(comp, reg, label) : ins::branch(comp, reg, value, label);
}

std::string BR_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& lines)
{
    std::string comp = comparators.at(comparator);

    return value == "0" ? ins::branch_relative_z(comp, reg, lines) : ins::branch_relative(comp, reg, value, lines);
}

std::string RA_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label)
{
    std::string comp = comparators.at(comparator);

    return value == "0" ? ins::sub_call_z(comp, reg, label) : ins::sub_call(comp, reg, value, label);
}