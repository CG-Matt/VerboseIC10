#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "comp_structs.h"
#include "instructions.h"
#include "commands.h"
#include "utils.h"

namespace c_commands
{
    std::string dev(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string selector = shift(args);
        shift(args); // Discard next token
        
        if(flags.devices_initialised == true){ return "ERROR: Devices have already been initialised"; }

        if(includes(globals.references.m_valid_devices, selector))
        {
            std::string name = shift(args);
            globals.references.add(name, selector);
            return "";
        }

        if(selector == "*")
        {
            flags.devices_initialised = true;
            std::vector<std::string> arr = parse_array(args);
            for(int i = 0; i < arr.size(); i++)
            {
                std::string cur_tok = arr[i];

                globals.references.add(cur_tok, globals.references.m_available_devices[i]);
                // flags.avaiable_registers--; // Bug? Maybe should be available_devices???
            }
            return "";
        }

        return "ERROR: Device assignment error";
    }
    std::string reg(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        if(args.size() < 1){ return "ERROR: Register names not provided"; }
        std::string& first = args[0];
        if(first.rfind("[", 0) == 0)
        {
            std::vector<std::string> arr = parse_array(args);
            for(auto &entry : arr)
            {
                globals.references.add(entry, globals.references.get_free());
            }
            return "";
        }

        std::string reg = globals.references.get_free();
        globals.references.add(first, reg);

        return "";
    }
    std::string set(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string reg = shift(args);
        shift(args); // Discard next token
        std::string value = shift(args);

        reg = globals.references.get(reg);
        value = parse_value(value, globals);

        return ins::move(reg, value);
    }
    std::string label(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string name = shift(args);
        globals.register_label(name);

        return ins::label(name);
    }
    std::string eport(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string target = shift(args);
        shift(args); // Discard next token
        std::string value = shift(args);
        auto dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];

        value = parse_value(value, globals);

        if(device.rfind("*", 0) == 0)
        {
            device.erase(device.begin());
            device = globals.references.get(device);

            return ins::sb(device, variable, value);
        }

        device = globals.references.get(device);

        return ins::s(device, variable, value);
    }
    std::string wait(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        if(args.size() < 1){ return ins::yld(); } // Technically not needed because of shift

        std::string time = shift(args);

        if(time.rfind("#", 0) == 0){ return ins::yld(); }

        return ins::sleep(time);
    }
    std::string move(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string lines = shift(args);
        std::string condition = shift(args);
        std::string reg = shift(args);
        std::string compare = shift(args);
        std::string value = shift(args);

        if(condition.size() < 1){ return ins::jr(lines); }

        reg = globals.references.get(reg);
        return BR_compare(compare, reg, value, lines);
    }
    std::string math(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string reg = shift(args);
        shift(args); // Discard next token
        std::string var1 = shift(args);
        std::string op = shift(args);
        std::string var2 = shift(args);

        reg = globals.references.get(reg);
        var1 = parse_value(var1, globals);
        var2 = parse_value(var2, globals);

        op = operations.at(op);

        return ins::math(op, reg, var1, var2);
    }
    std::string jump(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        shift(args); // Discard first token
        std::string label = shift(args);

        return ins::j(label);
    }
    std::string import(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        std::string reg = shift(args);
        shift(args); // Discard next token
        std::string target = shift(args);
        auto dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];

        reg = globals.references.get(reg);
        device = globals.references.get(device);

        return ins::l(reg, device, variable);
    }
    std::string branch(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        shift(args); // Discard first token
        std::string label = shift(args);
        std::string condition = shift(args);
        std::string var1 = shift(args);
        std::string compare = shift(args);
        std::string var2 = shift(args);

        if(condition.size() < 1){ return ""; }

        var1 = parse_value(var1, globals);
        var2 = parse_value(var2, globals);

        return B_compare(compare, var1, var2, label);
    }
    std::string trans(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        if(!flags.using_carry){ return "You must specify the use of carry to use the trans command (#using carry)"; }

        std::string source = shift(args);
        shift(args);
        std::string destination = shift(args);
        auto sdv_arr = split_string(source, '.');
        std::string& s_device = sdv_arr[0];
        std::string& s_variable = sdv_arr[1];
        auto ddv_arr = split_string(destination, '.');
        std::string& d_device = ddv_arr[0];
        std::string& d_variable = ddv_arr[1];

        std::string eport;

        if(d_device.rfind("*", 0) == 0)
        {
            d_device.erase(d_device.begin());
            eport = ins::sb(globals.references.get(d_device), d_variable, globals.references.get("carry"));
        }
        else
        {
            eport = ins::s(globals.references.get(d_device), d_variable, globals.references.get("carry"));
        }

        std::string eport2 = ins::l(globals.references.get("carry"), globals.references.get(s_device), s_variable);

        return eport2 + "\n" + eport;
    }
    std::string p_if(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        List list;

        auto labels = globals.generate_conditional_labels();
        const std::string& fail_label = labels.fail_label;

        std::string reg;
        std::string comparator;
        std::string value;

        while(args.size() > 0)
        {
            reg = shift(args);
            comparator = shift(args);
            value = shift(args);
            if(reg.size() < 1){ break; }
            if(comparator.size() < 1){ break; }
            if(value.size() < 1){ break; }

            reg = globals.references.get(reg);
            value = parse_value(value, globals);
            comparator = invert_comparator.at(comparator);
            list.add(B_compare(comparator, reg, value, fail_label));
            if(args.size() > 0)
            {
                if(includes(combinators, args[0])){ shift(args); }
            }
        }

        return list.concat();
    }
    std::string p_else(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        List list;
        flags.is_conditional_else = true;

        list.add(ins::j(globals.conditional.end_label));
        list.add(ins::label(globals.conditional.fail_label));

        return list.concat();
    }
    std::string end(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)
    {
        if(flags.is_conditional_else == false){ return ins::label(globals.conditional.fail_label); }

        flags.is_conditional_else = false;
        return ins::label(globals.conditional.end_label);
    }
};

std::unordered_map<std::string, std::function<std::string(std::vector<std::string>& args, ParserGlobals& globals, ParserFlags& flags)>> commands_map =
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
    { "end", c_commands::end }
};