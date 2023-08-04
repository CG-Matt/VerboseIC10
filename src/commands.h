#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "data_structs.h"
#include "parser.h"

typedef std::function<std::string(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags)> cmd_func;

namespace c_commands
{
    std::string dev(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string reg(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string set(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string label(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string eport(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string wait(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string move(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string math(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string jump(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string import(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string branch(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string trans(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string p_if(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string p_else(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
    std::string end(vmc::string_array& args, uint16_t idx, ParserGlobals& globals, ParserFlags& flags);
};

extern std::unordered_map<std::string, cmd_func> commands_map;