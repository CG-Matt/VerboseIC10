#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "data_structs.h"
#include "parser.h"

typedef std::function<void(vmc::string_array& args, Parser* parser)> cmd_func;

namespace c_commands
{
    void dev(vmc::string_array& args, Parser* parser);
    void reg(vmc::string_array& args, Parser* parser);
    void set(vmc::string_array& args, Parser* parser);
    void label(vmc::string_array& args, Parser* parser);
    void eport(vmc::string_array& args, Parser* parser);
    void wait(vmc::string_array& args, Parser* parser);
    void move(vmc::string_array& args, Parser* parser);
    void math(vmc::string_array& args, Parser* parser);
    void jump(vmc::string_array& args, Parser* parser);
    void import(vmc::string_array& args, Parser* parser);
    void branch(vmc::string_array& args, Parser* parser);
    void trans(vmc::string_array& args, Parser* parser);
    void p_if(vmc::string_array& args, Parser* parser);
    void p_else(vmc::string_array& args, Parser* parser);
    void end(vmc::string_array& args, Parser* parser);
    void xref(vmc::string_array& args, Parser* parser);
    void p_const(vmc::string_array& args, Parser* parser);
    void sub(vmc::string_array& args, Parser* parser);
    void call(vmc::string_array& args, Parser* parser);
};

static const std::unordered_map<std::string, cmd_func> commands_map =
{
    { "dev", c_commands::dev },
    { "reg", c_commands::reg },
    { "set", c_commands::set },
    { "label", c_commands::label },
    { "export", c_commands::eport },
    { "wait", c_commands::wait },
    { "move", c_commands::move },
    { "math", c_commands::math },
    { "jump", c_commands::jump },
    { "import", c_commands::import },
    { "branch", c_commands::branch },
    { "trans", c_commands::trans },
    { "if", c_commands::p_if },
    { "else", c_commands::p_else },
    { "end", c_commands::end },
    { "xref", c_commands::xref },
    { "const", c_commands::p_const },
    { "sub", c_commands::sub },
    { "call", c_commands::call }
};