#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "parser.h"
#include "instructions.h"

/*Splits the string "str" at every "delim" char*/
std::vector<std::string> split_string(const std::string& str, char delim)
{
    std::stringstream line_stream(str);
    std::string segment;
    std::vector<std::string> out;

    while(std::getline(line_stream, segment, delim))
    {
        out.push_back(segment);
    }

    return out;
}

/*Removes and returns the first element of the "data" vector.
Returns empty string if it reaches the end of the vector*/
std::string shift(std::vector<std::string>& data)
{
    if(data.size() < 1){ return ""; }

    std::string segment = data[0];
    data.erase(data.begin());
    return segment;
}

/*Joins all string in the "data" vector into a single string joined together with the "delim" char/string*/
std::string join_string(const std::vector<std::string>& data, std::string delim)
{
    std::stringstream ss;
    for(int i = 0; i < data.size(); i++)
    {
        if(i != 0){ ss << delim; }
        ss << data[i];
    }

    return ss.str();
}

/*Checks if a vector includes a "search_string"*/
bool includes(const std::vector<std::string>& data, std::string search_string)
{
    auto it = std::find_if(data.begin(), data.end(), [&search_string](const std::string token)
    {
        return token == search_string;
    });
    if(it == data.end()){ return false; }

    return true;
}

std::vector<std::string> parse_array(const std::vector<std::string>& data)
{
    std::string line = join_string(data, "");
    line.erase(line.begin());
    line.erase(line.end() - 1);
    return split_string(line, ',');
}

bool is_number(const std::string& str)
{
    bool is_int = str.find_first_not_of("0123456789") == std::string::npos;
    if(is_int){ return true; }
    
    bool has_decimal = str.find_first_not_of("0123456789.") == std::string::npos;
    if(!has_decimal){ return false; }
    
    bool len_two = str.size() > 1;
    if(!len_two){ return false; }
    
    bool start_with_point = str.rfind(".", 0) == 0;
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

std::string parse_value(const std::string& variable, const ParserGlobals& globals)
{
    if(is_boolean(variable)){ return parse_boolean(variable); }
    if(is_reference(variable)){ return globals.references.get(variable); }
    return variable;
}

bool starts_with(const std::string& data, const std::string& segment)
{
    return data.rfind(segment, 0) == 0;
}

std::string B_compare(const std::string& comparator, const std::string& reg, const std::string& value, const std::string& label)
{
    if(value == "0")
    {
        if(comparator == "=="){ return ins::beqz(reg, label); }
        if(comparator == "!="){ return ins::bnez(reg, label); }
        if(comparator == ">"){ return ins::bgtz(reg, label); }
        if(comparator == "<"){ return ins::bltz(reg, label); }
    }
    else
    {
        if(comparator == "=="){ return ins::beq(reg, value, label); }
        if(comparator == "!="){ return ins::bne(reg, value, label); }
        if(comparator == ">"){ return ins::bgt(reg, value, label); }
        if(comparator == "<"){ return ins::blt(reg, value, label); }
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
    }
    else
    {
        if(comparator == "=="){ return ins::breq(reg, value, lines); }
        if(comparator == "!="){ return ins::brne(reg, value, lines); }
        if(comparator == ">"){ return ins::brgt(reg, value, lines); }
        if(comparator == "<"){ return ins::brlt(reg, value, lines); }
    }

    return "ERROR: Not recognised comparator";
}