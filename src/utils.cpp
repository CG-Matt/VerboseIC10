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

bool is_boolean(const std::string& variable)
{
    return variable == "true" || variable == "false";
}

bool is_reference(const std::string& variable)
{
    return !is_number(variable);
}

std::string parse_boolean(const std::string& variable)
{
    if(variable == "true"){ return "1"; }
    return "0";
}


bool starts_with(const std::string& data, const std::string& segment){ return data.rfind(segment, 0) == 0; }

bool ends_with(const std::string& data, const std::string& segment){ return data.rfind(segment) == data.size() - segment.size(); }

std::string B_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label)
{
    if(value == "0")
    {
        if(comparator == "=="){ return ins::beqz(reg, label); }
        if(comparator == "!="){ return ins::bnez(reg, label); }
        if(comparator == ">"){ return ins::bgtz(reg, label); }
        if(comparator == "<"){ return ins::bltz(reg, label); }
        if(comparator == ">="){ return ins::bgez(reg, label); }
        if(comparator == "<="){ return ins::blez(reg, label); }
    }
    else
    {
        if(comparator == "=="){ return ins::beq(reg, value, label); }
        if(comparator == "!="){ return ins::bne(reg, value, label); }
        if(comparator == ">"){ return ins::bgt(reg, value, label); }
        if(comparator == "<"){ return ins::blt(reg, value, label); }
        if(comparator == ">="){ return ins::bge(reg, value, label); }
        if(comparator == "<="){ return ins::ble(reg, value, label); }
    }

    return "ERROR: Not recognised comparator";
}

std::string BR_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& lines)
{
    if(value == "0")
    {
        if(comparator == "=="){ return ins::breqz(reg, lines); }
        if(comparator == "!="){ return ins::brnez(reg, lines); }
        if(comparator == ">"){ return ins::brgtz(reg, lines); }
        if(comparator == "<"){ return ins::brltz(reg, lines); }
        if(comparator == ">="){ return ins::brgez(reg, lines); }
        if(comparator == "<="){ return ins::brlez(reg, lines); }
    }
    else
    {
        if(comparator == "=="){ return ins::breq(reg, value, lines); }
        if(comparator == "!="){ return ins::brne(reg, value, lines); }
        if(comparator == ">"){ return ins::brgt(reg, value, lines); }
        if(comparator == "<"){ return ins::brlt(reg, value, lines); }
        if(comparator == ">="){ return ins::brge(reg, value, lines); }
        if(comparator == "<="){ return ins::brle(reg, value, lines); }
    }

    return "ERROR: Not recognised comparator";
}

std::string RA_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label)
{
    if(value == "0")
    {
        if(comparator == "=="){ return ins::beqzal(reg, label); }
        if(comparator == "!="){ return ins::bnezal(reg, label); }
        if(comparator == ">"){ return ins::bgtzal(reg, label); }
        if(comparator == "<"){ return ins::bltzal(reg, label); }
        if(comparator == ">="){ return ins::bgezal(reg, label); }
        if(comparator == "<="){ return ins::blezal(reg, label); }
    }
    else
    {
        if(comparator == "=="){ return ins::beqal(reg, value, label); }
        if(comparator == "!="){ return ins::bneal(reg, value, label); }
        if(comparator == ">"){ return ins::bgtal(reg, value, label); }
        if(comparator == "<"){ return ins::bltal(reg, value, label); }
        if(comparator == ">="){ return ins::bgeal(reg, value, label); }
        if(comparator == "<="){ return ins::bleal(reg, value, label); }
    }
    
    return "ERROR: Not recognised comparator";
}