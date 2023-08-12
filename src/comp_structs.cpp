#include <string>
#include <vector>
#include "data_structs.h"
#include "constants.h"
#include "comp_structs.h"
#include "utils.h"

RawDirective::RawDirective(std::vector<std::string> line, uint16_t line_idx)
{
    this->line_idx = line_idx;
    // Check if line is empty first (causes seg fault if empty)
    if(line.size() < 1)
    {
        m_directive = "";
        return;
    }
    // First element of the line becomes the directive
    std::string directive = line[0];
    line.erase(line.begin());
    // Removing the "#" from the beginning of the directive
    directive.erase(0, 1);
    m_directive = directive;
    // Setting the arguments to be the rest of the line
    m_arguments = line;
}



// Possibly filter command to not include empty lines
RawCommand::RawCommand(std::vector<std::string> line, uint16_t line_idx)
{
    this->line_idx = line_idx;
    // Check if line is empty first (causes seg fault if empty)
    if(line.size() < 1)
    {
        m_command = "";
        return;
    }
    // First element of the line becomes the command
    m_command = line[0];
    line.erase(line.begin());
    // Setting the arguments to be the rest of the line
    m_arguements = line;
}

ProgramLine::ProgramLine(const std::string& line, uint16_t line_idx)
{
    // Set the line_idx of the struct
    this->m_line_idx = line_idx;
    // Check if line is empty first (causes seg fault if empty)
    if(line.size() < 1){ this->m_first = ""; return; }
    // The line is split into tokens
    std::vector<std::string> tokens = split_string(line, ' ');
    // First token becomes the command/directive and gets removed from the array
    std::string first = tokens[0];
    tokens.erase(tokens.begin());
    // If first starts with "#" remove it
    if(starts_with(first, "#")){ first.erase(first.begin()); }
    // Set first ands args member variables
    this->m_first = first;
    this->m_args.set(tokens);
}

unsigned int List::get_length() const { return m_length; }
void List::add(const std::string& line)
{
    m_lines.push_back(line);
    m_length++;
}

std::string List::concat()
{
    return join_string(m_lines, "\n");
}