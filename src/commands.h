#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "data_structs.h"
#include "parser.h"

typedef std::function<std::string(vmc::string_array& args, const uint16_t& idx, Parser* parser)> cmd_func;

namespace c_commands
{
    std::string dev(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string reg(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string set(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string label(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string eport(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string wait(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string move(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string math(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string jump(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string import(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string branch(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string trans(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string p_if(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string p_else(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string end(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    std::string xref(vmc::string_array& args, const uint16_t& idx, Parser* parser);
};

extern std::unordered_map<std::string, cmd_func> commands_map;