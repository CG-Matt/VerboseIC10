#include <string>
#include <vector>
#include <map>
#include <functional>
#include "comp_structs.h"
#include "instructions.h"
#include "commands.h"
#include "utils.h"
#include "data_structs.h"
#include "constants.h"

namespace c_commands
{
    std::string dev(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(parser->flags.devices_initialised == true){ parser->errors.add_end(vmc::GenericError(idx, "Devices have already been initialised")); return ""; }
        if(parser->flags.available_devices <= 0){ parser->errors.add_end(vmc::GenericError(idx, "Max device limit reached")); return ""; }
        
        std::string selector = args.shift();
        args.shift();

        if(includes(valid_devices, selector))
        {
            std::string name = args.shift();
            parser->globals.references.add(name, selector);
            parser->flags.available_devices--;
            return "";
        }

        if(selector == "*")
        {
            parser->flags.devices_initialised = true;
            vmc::string_array arr(parse_array(args));
            if(arr.size() > 6){ parser->errors.add_end(vmc::GenericError(idx, "Too many devices listed. (" + std::to_string(arr.size()) + "/6)")); return ""; }
            for(int i = 0; i < arr.size(); i++)
            {
                parser->globals.references.add(arr[i], parser->globals.references.m_available_devices[i]);
            }
            return "";
        }

        parser->errors.add_end(vmc::GenericError(idx, "Device assignment error"));
        return "";
    }
    std::string reg(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!args.contains_data()){ parser->errors.add_end(vmc::GenericError(idx, "Register names not provided")); return ""; }
        std::string& first = args[0];
        if(starts_with(first, "["))
        {
            vmc::string_array arr = parse_array(args);
            for(auto &entry : arr)
            {
                parser->globals.references.add(entry, parser->globals.references.get_free());
            }
            return "";
        }

        std::string reg = parser->globals.references.get_free();
        parser->globals.references.add(first, reg);

        return "";
    }
    std::string set(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string reg = args.shift();
        args.shift(); // Discard next token
        std::string value = args.shift();

        reg = parser->globals.references.get(reg);
        value = parse_value(value, parser->globals);

        return ins::move(reg, value);
    }
    std::string label(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string name = args.shift();
        if(name.size() < 1){ parser->errors.add_end(vmc::GenericError(idx, "Label name required")); return ""; }
        parser->globals.register_label(name);

        return ins::label(name);
    }
    std::string eport(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string target = args.shift();
        args.shift(); // Discard next token
        std::string value = args.shift();
        auto dv_arr = split_string(target, '.');
        std::string device = dv_arr[0];
        std::string variable = dv_arr[1];

        value = parse_value(value, parser->globals);

        if(starts_with(device, "*"))
        {
            device.erase(device.begin());
            device = parser->globals.references.get(device);

            return ins::sb(device, variable, value);
        }

        device = parser->globals.references.get(device);

        return ins::s(device, variable, value);
    }
    std::string wait(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!args.contains_data()){ return ins::yld(); }
        // The first argument is the time to sleep
        return ins::sleep(args[0]);
    }
    std::string move(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string lines = args.shift();
        std::string condition = args.shift();

        if(condition.size() < 1){ return ins::jr(lines); }

        std::string reg = args.shift();
        std::string compare = args.shift();
        std::string value = args.shift();

        reg = parser->globals.references.get(reg);
        return BR_compare(compare, reg, value, lines);
    }
    std::string math(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string reg = args.shift();
        args.shift(); // Discard next token
        std::string var1 = args.shift();
        std::string op = args.shift();
        std::string var2 = args.shift();

        reg = parser->globals.references.get(reg);
        var1 = parse_value(var1, parser->globals);
        var2 = parse_value(var2, parser->globals);

        op = operations.at(op);

        return ins::math(op, reg, var1, var2);
    }
    std::string jump(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        args.v_shift(); // Discard first token
        std::string& label = args.v_shift();

        if(!parser->globals.label_exists(label)){ parser->errors.add_end(vmc::GenericError(idx, "Unregistered label \"" + label + "\" called")); return ""; }

        return ins::j(label);
    }
    std::string import(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string reg = args.shift();
        args.shift(); // Discard next token
        std::string target = args.shift();
        auto dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];

        reg = parser->globals.references.get(reg);
        device = parser->globals.references.get(device);

        return ins::l(reg, device, variable);
    }
    std::string branch(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        args.shift(); // Discard first token
        std::string label = args.shift();
        std::string condition = args.shift();

        if(condition.size() < 1){ return ""; }

        std::string var1 = args.shift();
        std::string compare = args.shift();
        std::string var2 = args.shift();

        var1 = parse_value(var1, parser->globals);
        var2 = parse_value(var2, parser->globals);

        return B_compare(compare, var1, var2, label);
    }
    std::string trans(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.using_carry){ return "You must specify the use of carry to use the trans command (#using carry)"; }

        std::string source = args.shift();
        args.shift();
        std::string destination = args.shift();
        auto sdv_arr = split_string(source, '.');
        std::string& s_device = sdv_arr[0];
        std::string& s_variable = sdv_arr[1];
        auto ddv_arr = split_string(destination, '.');
        std::string& d_device = ddv_arr[0];
        std::string& d_variable = ddv_arr[1];

        std::string eport;

        if(starts_with(d_device, "*"))
        {
            d_device.erase(d_device.begin());
            eport = ins::sb(parser->globals.references.get(d_device), d_variable, parser->globals.references.get("carry"));
        }
        else
        {
            eport = ins::s(parser->globals.references.get(d_device), d_variable, parser->globals.references.get("carry"));
        }

        std::string eport2 = ins::l(parser->globals.references.get("carry"), parser->globals.references.get(s_device), s_variable);

        return eport2 + "\n" + eport;
    }
    std::string p_if(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        List list;

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

            reg = parser->globals.references.get(reg);
            value = parse_value(value, parser->globals);
            if(invert_comparator.find(comparator) == invert_comparator.end())
            {
                parser->errors.add_end(vmc::GenericError(idx, "Unexpected comparator symbol \"" + comparator + "\""));
                return "";
            }
            comparator = invert_comparator.at(comparator);
            list.add(B_compare(comparator, reg, value, fail_label));
            if(args.contains_data())
            {
                if(includes(combinators, args[0])){ args.shift(); }
            }
        }

        parser->flags.in_conditional = true;

        return list.concat();
    }
    std::string p_else(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.in_conditional){ parser->errors.add_end(vmc::GenericError(idx, "Conditional not initialised correctly")); return ""; }

        List list;
        parser->flags.is_conditional_else = true;

        list.add(ins::j(parser->globals.conditional.end_label));
        list.add(ins::label(parser->globals.conditional.fail_label));

        return list.concat();
    }
    std::string end(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.in_conditional){ parser->errors.add_end(vmc::GenericError(idx, "Conditional not initialised correctly")); return ""; }

        if(parser->flags.is_conditional_else == false){ return ins::label(parser->globals.conditional.fail_label); }

        parser->flags.is_conditional_else = false;
        return ins::label(parser->globals.conditional.end_label);
    }
    std::string xref(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(args.size() < 3){ parser->errors.add_end(vmc::GenericError(idx, "Insufficient arguments provided. Got " + std::to_string(args.size()) + ", expected 3." )); return ""; }
        std::string target = args.shift();
        std::vector<std::string> dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];
        std::string direction = args.shift();
        if(direction != "<-" && direction != "->"){ parser->errors.add_end(vmc::GenericError(idx, "Invalid direction \"" + direction + "\" provided")); return ""; }
        std::string reg = args.shift();

        if(direction == "->")
        {
            reg = parser->globals.references.get(reg);
            device = parser->globals.references.get(device);
            return ins::l(reg, device, variable);
        }
        // else direction == "->"

        reg = parse_value(reg, parser->globals);
        if(starts_with(device, "*"))
        {
            device.erase(device.begin());
            device = parser->globals.references.get(device);
            return ins::sb(device, variable, reg);
        }

        device = parser->globals.references.get(device);
        return ins::s(device, variable, reg);
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
    { "xref", c_commands::xref }
};