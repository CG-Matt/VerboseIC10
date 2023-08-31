#include <string>
#include <unordered_map>
#include "comp_structs.h"
#include "instructions.h"
#include "commands.h"
#include "utils.h"
#include "data_structs.h"
#include "constants.h"

namespace c_commands
{
    void dev(vmc::string_array& args, Parser* parser)
    {
        if(parser->flags.devices_initialised == true){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Devices have already been initialised")); return; }
        if(parser->flags.available_devices <= 0){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Max device limit reached")); return; }
        
        std::string& selector = args.v_shift();
        args.v_shift();

        if(includes(valid_devices, selector))
        {
            std::string& name = args.v_shift();
            parser->globals.references.add(name, selector, ParserReferences::ref_type::dev);
            parser->flags.available_devices--;
            return;
        }

        if(selector == "*")
        {
            parser->flags.devices_initialised = true;
            vmc::string_array arr = parser->utils.parse_array(args.make_offset_view());
            if(parser->has_error()){ return; }
            if(arr.size() > 6){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Too many devices listed. (" + std::to_string(arr.size()) + "/6)")); return; }
            for(uint32_t i = 0; i < arr.size(); i++)
            {
                parser->globals.references.add(arr[i], available_devices[i], ParserReferences::ref_type::dev);
            }
            return;
        }

        parser->set_error(vmc::GenericError(parser->get_current_line(), "Device assignment error"));
    }
    void reg(vmc::string_array& args, Parser* parser)
    {
        if(!args.contains_data()){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Register names not provided")); return; }
        std::string& first = args.v_shift();
        if(starts_with(first, "["))
        {
            vmc::string_array arr = parser->utils.parse_array(args);
            if(parser->has_error()){ return; }
            for(auto &entry : arr)
            {
                parser->globals.references.add(entry, parser->globals.references.get_free(), ParserReferences::ref_type::reg);
            }
            return;
        }

        std::string reg = parser->globals.references.get_free();
        parser->globals.references.add(first, reg, ParserReferences::ref_type::reg);
    }
    void set(vmc::string_array& args, Parser* parser)
    {
        std::string& reg = args.v_shift();
        args.v_shift(); // Discard next token
        std::string& value = args.v_shift();

        reg = parser->get_ref(reg);
        value = parser->utils.parse_value(value);

        parser->output.add_end(ins::move(reg, value));
    }
    void label(vmc::string_array& args, Parser* parser)
    {
        std::string& name = args.v_shift();
        if(name.size() < 1){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Label name required")); return; }
        parser->globals.register_label(name);

        parser->output.add_end(ins::label(name));
    }
    void eport(vmc::string_array& args, Parser* parser)
    {
        Device target = parser->utils.parse_device(args.v_shift());
        if(parser->has_error()){ return; }
        args.v_shift(); // Discard next token
        std::string& value = args.v_shift();

        value = parser->utils.parse_value(value);

        if(target.is_prefabhash)
        {
            parser->output.add_end(ins::sb(target.name, target.variable, value));
            return;
        }

        parser->output.add_end(ins::s(target.name, target.variable, value));
    }
    void wait(vmc::string_array& args, Parser* parser)
    {
        if(!args.contains_data()){ parser->output.add_end(ins::yld()); return; }
        // The first argument is the time to sleep
        parser->output.add_end(ins::sleep(args[0]));
    }
    void move(vmc::string_array& args, Parser* parser)
    {
        std::string& lines = args.v_shift();
        std::string& condition = args.v_shift();

        if(condition.size() < 1){ parser->output.add_end(ins::jr(lines)); return; }

        std::string& reg = args.v_shift();
        std::string& compare = args.v_shift();
        std::string& value = args.v_shift();

        reg = parser->get_ref(reg);
        parser->output.add_end(BR_compare(compare, reg, value, lines));
    }
    void math(vmc::string_array& args, Parser* parser)
    {
        std::string& reg = args.v_shift();
        std::string& set_op = args.v_shift();
        std::string& var1 = args.v_shift();
        std::string op;
        std::string var2;

        if(set_op == "+=" || set_op == "-=" || set_op == "*=" || set_op == "/=")
        {
            op = set_op[0];
            var2 = var1;
            var1 = reg;
        }
        else if(set_op == "=")
        {
            op = args.v_shift();
            var2 = args.v_shift();
        }
        else
        {
            parser->set_error(vmc::InvalidMathOpError(parser->get_current_line(), set_op));
            return;
        }

        reg = parser->get_ref(reg);
        var1 = parser->utils.parse_value(var1);
        var2 = parser->utils.parse_value(var2);

        if(operations.find(op) == operations.end()){ parser->set_error(vmc::InvalidMathOpError(parser->get_current_line(), op)); return; }
        op = operations.at(op);

        parser->output.add_end(ins::math(op, reg, var1, var2));
    }
    void jump(vmc::string_array& args, Parser* parser)
    {
        args.v_shift(); // Discard first token
        std::string& label = args.v_shift();

        if(!parser->globals.label_exists(label)){ parser->globals.unresolved_labels.push_back(vmc::Line{parser->get_current_line(), label}); }

        parser->output.add_end(ins::j(label));
    }
    void import(vmc::string_array& args, Parser* parser)
    {
        std::string& reg = args.v_shift();
        args.v_shift(); // Discard next token
        Device target = parser->utils.parse_device(args.v_shift());
        if(parser->has_error()){ return; }

        reg = parser->get_ref(reg);

        parser->output.add_end(ins::l(reg, target.name, target.variable));
    }
    void branch(vmc::string_array& args, Parser* parser)
    {
        args.v_shift(); // Discard first token
        std::string& label = args.v_shift();
        std::string& condition = args.v_shift();

        if(condition.size() < 1){ return; }

        std::string& var1 = args.v_shift();
        std::string& compare = args.v_shift();
        std::string& var2 = args.v_shift();

        var1 = parser->utils.parse_value(var1);
        var2 = parser->utils.parse_value(var2);

        parser->output.add_end(B_compare(compare, var1, var2, label));
    }
    void trans(vmc::string_array& args, Parser* parser)
    {
        if(!parser->flags.using_carry){ parser->set_error(vmc::GenericError(parser->get_current_line(), "You must specify the use of carry to use the trans command (#using carry)")); return; }

        Device source = parser->utils.parse_device(args.v_shift());
        if(parser->has_error()){ return; }

        args.v_shift();

        Device destination = parser->utils.parse_device(args.v_shift());
        if(parser->has_error()){ return; }

        std::string eport;

        if(destination.is_prefabhash)
        {
            eport = ins::sb(destination.name, destination.variable, parser->get_ref("carry"));
        }
        else
        {
            eport = ins::s(destination.name, destination.variable, parser->get_ref("carry"));
        }

        std::string eport2 = ins::l(parser->get_ref("carry"), source.name, source.variable);

        parser->output.add_end(eport2 + "\n" + eport);
    }
    void p_if(vmc::string_array& args, Parser* parser)
    {
        auto labels = parser->globals.generate_conditional_labels();
        const std::string& fail_label = labels.fail_label;

        std::string reg;
        std::string comparator;
        std::string value;

        while(args.contains_data())
        {
            reg = args.shift();
            comparator = args.shift();
            value = args.shift();
            if(reg.size() < 1 || comparator.size() < 1 || value.size() < 1){ break; }

            reg = parser->get_ref(reg);
            value = parser->utils.parse_value(value);
            if(invert_comparator.find(comparator) == invert_comparator.end())
            {
                parser->set_error(vmc::GenericError(parser->get_current_line(), "Invalid comparator symbol \"" + comparator + "\""));
                return;
            }
            comparator = invert_comparator.at(comparator);
            parser->output.add_end(B_compare(comparator, reg, value, fail_label));
            if(args.contains_data())
            {
                if(includes(combinators, args[0])){ args.shift(); }
            }
        }

        parser->flags.in_conditional = true;
    }
    void p_else(vmc::string_array& args, Parser* parser)
    {
        if(!parser->flags.in_conditional){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Conditional not initialised correctly")); return; }

        parser->flags.is_conditional_else = true;

        parser->output.add_end(ins::j(parser->globals.conditional.end_label));
        parser->output.add_end(ins::label(parser->globals.conditional.fail_label));
    }
    void end(vmc::string_array& args, Parser* parser)
    {
        if(args.size() < 1){ parser->set_error(vmc::InsufficientArgsError(parser->get_current_line(), args.size(), 1)); return; }

        if(args[0] == "if")
    {
        if(!parser->flags.in_conditional){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Conditional not initialised correctly")); return; }

        if(parser->flags.is_conditional_else == false){ parser->output.add_end(ins::label(parser->globals.conditional.fail_label)); return; }

            parser->flags.in_conditional = false;
        parser->flags.is_conditional_else = false;
        parser->output.add_end(ins::label(parser->globals.conditional.end_label));
    }
        else if(args[0] == "sub")
        {
            if(!parser->flags.in_subroutine){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Subroutine not initialised correctly")); return; }
            
            parser->flags.in_subroutine = false;
            parser->output.add_end(ins::j("ra"));
        }
    }
    void xref(vmc::string_array& args, Parser* parser)
    {
        if(args.size() < 3){ parser->set_error(vmc::InsufficientArgsError(parser->get_current_line(), args.size(), 3)); return; }
        
        Device target = parser->utils.parse_device(args.v_shift());
        if(parser->has_error()){ return; }
        
        std::string& direction = args.v_shift();
        if(direction != "<-" && direction != "->"){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Invalid direction \"" + direction + "\" provided")); return; }
        std::string& reg = args.v_shift();

        if(direction == "->")
        {
            reg = parser->get_ref(reg);
            parser->output.add_end(ins::l(reg, target.name, target.variable));
            return;
        }
        // else direction == "<-"

        reg = parser->utils.parse_value(reg);
        if(target.is_prefabhash)
        {
            parser->output.add_end(ins::sb(target.name, target.variable, reg));
            return;
        }

        parser->output.add_end(ins::s(target.name, target.variable, reg));
    }
    void p_const(vmc::string_array& args, Parser* parser)
    {
        // Const will create a reference to a value without using a register
        if(args.size() < 3){ parser->set_error(vmc::InsufficientArgsError(parser->get_current_line(), args.size(), 3)); return; }
        
        std::string& const_name = args.v_shift();
        args.v_shift();
        std::string& value = args.v_shift();

        parser->globals.references.add(const_name, value, ParserReferences::ref_type::constant);
    }
    void sub(vmc::string_array& args, Parser* parser)
    {
        if(args.size() < 1){ parser->set_error(vmc::InsufficientArgsError(parser->get_current_line(), args.size(), 1)); return; }

        std::string& label_name = args.v_shift();

        if(parser->globals.label_exists(label_name)){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Label with name \"" + label_name + "\" already exists.")); return; }

        parser->flags.in_subroutine = true;        
        parser->globals.register_label(label_name);
        parser->output.add_end(ins::label(label_name));
    }
    void call(vmc::string_array& args, Parser* parser)
    {
        if(args.size() < 1){ parser->set_error(vmc::InsufficientArgsError(parser->get_current_line(), args.size(), 1)); return; }

        std::string& label_name = args.v_shift();

        if(!parser->globals.label_exists(label_name)){ parser->globals.unresolved_labels.push_back(vmc::Line{parser->get_current_line(), label_name}); }

        if(args.size() < 2){ parser->output.add_end(ins::jal(label_name)); return; }

        args.v_shift(); // return "if"

        std::string& var1 = args.v_shift();
        std::string& compare = args.v_shift();
        std::string& var2 = args.v_shift();

        if(!includes(comparators, compare)){ parser->set_error(vmc::GenericError(parser->get_current_line(), "Invalid comparator symbol \"" + compare + "\"")); return; }

        var1 = parser->utils.parse_value(var1);
        var2 = parser->utils.parse_value(var2);

        parser->output.add_end(RA_compare(compare, var1, var2, label_name));
    }
};

std::unordered_map<std::string, cmd_func> commands_map =
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