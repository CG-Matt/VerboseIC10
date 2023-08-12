#pragma once

#include <string>
#include <vector>
#include "data_structs.h"

struct ConditionalInfo
{
    int idx = 0;
    std::string pass_label = "";
    std::string fail_label = "";
    std::string end_label = "";
};

struct ProgramLine
{
    uint16_t m_line_idx;
    std::string m_first;
    vmc::string_array m_args;

    ProgramLine(const std::string& line, uint16_t line_idx);
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