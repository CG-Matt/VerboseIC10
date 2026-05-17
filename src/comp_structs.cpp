#include <string>
#include <vector>
#include "comp_structs.hpp"
#include "utils.hpp"

ProgramLine::ProgramLine(std::size_t line_number, std::string_view line)
    : line_number(line_number)
{
    // Set up some parsing flags
    bool first_occupied = false;
    std::size_t length = 0;

    // Split the line into tokens split on whitespace
    while(!line.empty())
    {
        if(line.size() == length || std::isspace(static_cast<unsigned char>(line[length])))
        {
            if(length)
            {
                if(!first_occupied)
                {
                    this->first = std::string(line.data(), length);
                    first_occupied = true;
                }
                else
                    this->args.push_back(std::string(line.data(), length));
                line.remove_prefix(length);
                length = 0;
            }
            else
                line.remove_prefix(1);
        }
        else
        {
            length++;
        }
    }

    // If the first token starts with a '#' remove it.
    if(!this->first.empty() && this->first.front() == '#') this->first.erase(0, 1);
}

Device::Device(std::string_view target, std::string_view variable, bool is_prefabhash)
    : target(target), variable(variable), is_prefabhash(is_prefabhash)
{}

Device::Device(std::string_view target, std::string_view variable)
    : Device(target, variable, false)
{}