#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "parser.h"

namespace c_commands
{
    std::string dev(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string reg(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string set(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string label(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string eport(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string wait(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string move(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string math(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string jump(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string import(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string branch(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string trans(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string p_if(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string p_else(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
    std::string end(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags);
};

extern std::unordered_map<std::string, std::function<std::string(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)>> commands_map;