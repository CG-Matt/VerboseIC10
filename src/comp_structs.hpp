#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

struct Identifier
{
    enum class Type
    {
        CONSTANT = 0,
        REGISTER,
        DEVICE
    };

    Type type;
    std::string target;
};

/**
 * Labels generated for a single high-level conditional block.
 */
struct ConditionalInfo
{
    std::size_t idx = 0;
    std::string pass_label;
    std::string fail_label;
    std::string end_label;
};

/**
 * A parsed source line split into its command/directive name and arguments.
 */
struct ProgramLine
{
    std::size_t line_number;
    std::string first;
    std::vector<std::string> args;

    /**
     * Tokenises one source line into its leading command and remaining arguments.
     */
    ProgramLine(std::size_t line_number, std::string_view line);
};

/**
 * A resolved device reference used by load/store instructions.
 */
struct Device
{
    std::string name;
    std::string variable;
    bool is_prefabhash = false;
};
