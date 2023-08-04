#pragma once

#include <string>
#include <vector>

struct ConditionalInfo
{
    int idx = 0;
    std::string pass_label = "";
    std::string fail_label = "";
    std::string end_label = "";
};

struct RawDirective
{
    uint16_t line_idx;
    std::string m_directive;
    std::vector<std::string> m_arguments;

    RawDirective(std::vector<std::string> line, uint16_t line_idx);
};

struct RawCommand
{
    uint16_t line_idx;
    std::string m_command;
    std::vector<std::string> m_arguements;

    RawCommand(std::vector<std::string> line, uint16_t line_idx);
};

class List
{
    private:
        unsigned int m_length = 0;

    public:
        std::vector<std::string> m_lines;

    unsigned int get_length() const;
    void add(const std::string& line);
    std::string concat();
};