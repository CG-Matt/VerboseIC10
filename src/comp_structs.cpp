#include <string>
#include <vector>
#include "comp_structs.h"
#include "utils.h"

ProgramLine::ProgramLine(const std::string& line, uint16_t line_idx)
{
    // Set the line_idx of the struct
    this->m_line_idx = line_idx;
    // The line is split into tokens
    std::vector<std::string> tokens = split_string(line, ' ');
    // First token becomes the command/directive and gets removed from the array
    std::string first = tokens[0];
    tokens.erase(tokens.begin());
    // If first starts with "#" remove it
    if(starts_with(first, "#")){ first.erase(first.begin()); }
    // Set first ands args member variables
    this->m_first = first;
    this->m_args = tokens;
}