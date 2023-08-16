#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "data_structs.h"
#include "parser.h"

typedef std::function<void(vmc::string_array& args, const uint16_t& idx, Parser* parser)> cmd_func;

namespace c_commands
{
    void dev(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void reg(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void set(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void label(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void eport(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void wait(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void move(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void math(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void jump(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void import(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void branch(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void trans(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void p_if(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void p_else(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void end(vmc::string_array& args, const uint16_t& idx, Parser* parser);
    void xref(vmc::string_array& args, const uint16_t& idx, Parser* parser);
};

extern std::unordered_map<std::string, cmd_func> commands_map;