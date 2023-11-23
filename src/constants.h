#pragma once

#include <string>
#include <vector>
#include <unordered_map>

const std::vector<std::string> available_devices{ "d0", "d1", "d2", "d3", "d4", "d5" };
const std::vector<std::string> valid_devices{ "d0", "d1", "d2", "d3", "d4", "d5", "db" };
const std::vector<std::string> valid_registers{ "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
const std::vector<std::string> combinators{ "and", "or" };
const std::vector<std::string> special_commands{ "if", "else", "end" };
const std::vector<std::string> math_functions{ "sin", "cos", "tan", "asin", "acos", "atan", "abs", "ceil", "floor", "exp", "log", "sqrt", "round", "trunc" };
const std::unordered_map<std::string, std::string> operations =
{
    { "+", "add" },
    { "-", "sub" },
    { "*", "mul" },
    { "/", "div" },
    { "%", "mod"}
};
const std::unordered_map<std::string, std::string> invert_comparator =
{
    { "==", "!=" },
    { "!=", "==" },
    { ">", "<" },
    { ">=", "<=" },
    { "<", ">" },
    { "<=", ">=" }
};
const std::unordered_map<std::string, std::string> comparators =
{
    {"==", "eq"},
    {"!=", "ne"},
    {">", "gt"},
    {">=", "ge"},
    {"<", "lt"},
    {"<=", "le"} 
};
