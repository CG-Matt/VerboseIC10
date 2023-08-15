#include <string>
#include <vector>
#include <unordered_map>
#include "comp_structs.h"
#include "instructions.h"
#include "commands.h"
#include "utils.h"
#include "data_structs.h"
#include "constants.h"

namespace c_commands
{
    void dev(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(parser->flags.devices_initialised == true){ parser->errors.add_end(vmc::GenericError(idx, "Devices have already been initialised")); return; }
        if(parser->flags.available_devices <= 0){ parser->errors.add_end(vmc::GenericError(idx, "Max device limit reached")); return; }
        
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
            bool arr_error;
            vmc::string_array arr = parse_array(args.make_offset_view(), arr_error);
            if(arr_error){ parser->errors.add_end(vmc::GenericError(idx, arr[0])); return; }
            if(arr.size() > 6){ parser->errors.add_end(vmc::GenericError(idx, "Too many devices listed. (" + std::to_string(arr.size()) + "/6)")); return; }
            for(int i = 0; i < arr.size(); i++)
            {
                parser->globals.references.add(arr[i], available_devices[i], ParserReferences::ref_type::dev);
            }
            return;
        }

        parser->errors.add_end(vmc::GenericError(idx, "Device assignment error"));
    }
    void reg(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!args.contains_data()){ parser->errors.add_end(vmc::GenericError(idx, "Register names not provided")); return; }
        std::string& first = args.v_shift();
        if(starts_with(first, "["))
        {
            bool arr_error;
            vmc::string_array arr = parse_array(args, arr_error);
            if(arr_error){ parser->errors.add_end(vmc::GenericError(idx, arr[0])); return; }
            for(auto &entry : arr)
            {
                parser->globals.references.add(entry, parser->globals.references.get_free(), ParserReferences::ref_type::reg);
            }
            return;
        }

        std::string reg = parser->globals.references.get_free();
        parser->globals.references.add(first, reg, ParserReferences::ref_type::reg);
    }
    void set(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& reg = args.v_shift();
        args.v_shift(); // Discard next token
        std::string& value = args.v_shift();

        reg = parser->globals.references.get(reg);
        value = parse_value(value, parser->globals);

        parser->output.add_end(ins::move(reg, value));
    }
    void label(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& name = args.v_shift();
        if(name.size() < 1){ parser->errors.add_end(vmc::GenericError(idx, "Label name required")); return; }
        parser->globals.register_label(name);

        parser->output.add_end(ins::label(name));
    }
    void eport(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& target = args.v_shift();
        args.v_shift(); // Discard next token
        std::string& value = args.v_shift();
        auto dv_arr = split_string(target, '.');
        std::string device = dv_arr[0];
        std::string variable = dv_arr[1];

        value = parse_value(value, parser->globals);

        if(starts_with(device, "*"))
        {
            device.erase(device.begin());
            device = parser->globals.references.get(device);

            parser->output.add_end(ins::sb(device, variable, value));
            return;
        }

        device = parser->globals.references.get(device);

        parser->output.add_end(ins::s(device, variable, value));
    }
    void wait(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!args.contains_data()){ parser->output.add_end(ins::yld()); return; }
        // The first argument is the time to sleep
        parser->output.add_end(ins::sleep(args[0]));
    }
    void move(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& lines = args.v_shift();
        std::string& condition = args.v_shift();

        if(condition.size() < 1){ parser->output.add_end(ins::jr(lines)); return; }

        std::string& reg = args.v_shift();
        std::string& compare = args.v_shift();
        std::string& value = args.v_shift();

        reg = parser->globals.references.get(reg);
        parser->output.add_end(BR_compare(compare, reg, value, lines));
    }
    void math(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& reg = args.v_shift();
        args.v_shift(); // Discard next token
        std::string& var1 = args.v_shift();
        std::string& op = args.v_shift();
        std::string& var2 = args.v_shift();

        reg = parser->globals.references.get(reg);
        var1 = parse_value(var1, parser->globals);
        var2 = parse_value(var2, parser->globals);

        op = operations.at(op);

        parser->output.add_end(ins::math(op, reg, var1, var2));
    }
    void jump(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        args.v_shift(); // Discard first token
        std::string& label = args.v_shift();

        if(!parser->globals.label_exists(label)){ parser->errors.add_end(vmc::GenericError(idx, "Unregistered label \"" + label + "\" called")); return; }

        parser->output.add_end(ins::j(label));
    }
    void import(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        std::string& reg = args.v_shift();
        args.v_shift(); // Discard next token
        std::string& target = args.v_shift();
        auto dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];

        reg = parser->globals.references.get(reg);
        device = parser->globals.references.get(device);

        parser->output.add_end(ins::l(reg, device, variable));
    }
    void branch(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        args.v_shift(); // Discard first token
        std::string& label = args.v_shift();
        std::string& condition = args.v_shift();

        if(condition.size() < 1){ return; }

        std::string& var1 = args.v_shift();
        std::string& compare = args.v_shift();
        std::string& var2 = args.v_shift();

        var1 = parse_value(var1, parser->globals);
        var2 = parse_value(var2, parser->globals);

        parser->output.add_end(B_compare(compare, var1, var2, label));
    }
    void trans(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.using_carry){ parser->errors.add_end(vmc::GenericError(idx, "You must specify the use of carry to use the trans command (#using carry)")); return; }

        std::string& source = args.v_shift();
        args.v_shift();
        std::string& destination = args.v_shift();
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

        parser->output.add_end(eport2 + "\n" + eport);
    }
    void p_if(vmc::string_array& args, const uint16_t& idx, Parser* parser)
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
                return;
            }
            comparator = invert_comparator.at(comparator);
            list.add(B_compare(comparator, reg, value, fail_label));
            if(args.contains_data())
            {
                if(includes(combinators, args[0])){ args.shift(); }
            }
        }

        parser->flags.in_conditional = true;

        parser->output.add_end(list.concat());
    }
    void p_else(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.in_conditional){ parser->errors.add_end(vmc::GenericError(idx, "Conditional not initialised correctly")); return; }

        List list;
        parser->flags.is_conditional_else = true;

        list.add(ins::j(parser->globals.conditional.end_label));
        list.add(ins::label(parser->globals.conditional.fail_label));

        parser->output.add_end(list.concat());
    }
    void end(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(!parser->flags.in_conditional){ parser->errors.add_end(vmc::GenericError(idx, "Conditional not initialised correctly")); return; }

        if(parser->flags.is_conditional_else == false){ parser->output.add_end(ins::label(parser->globals.conditional.fail_label)); return; }

        parser->flags.is_conditional_else = false;
        parser->output.add_end(ins::label(parser->globals.conditional.end_label));
    }
    void xref(vmc::string_array& args, const uint16_t& idx, Parser* parser)
    {
        if(args.size() < 3){ parser->errors.add_end(vmc::InsufficientArgsError(idx, args.size(), 3)); return; }
        std::string& target = args.v_shift();
        auto dv_arr = split_string(target, '.');
        std::string& device = dv_arr[0];
        std::string& variable = dv_arr[1];
        std::string& direction = args.v_shift();
        if(direction != "<-" && direction != "->"){ parser->errors.add_end(vmc::GenericError(idx, "Invalid direction \"" + direction + "\" provided")); return; }
        std::string& reg = args.v_shift();

        if(direction == "->")
        {
            reg = parser->globals.references.get(reg);
            device = parser->globals.references.get(device);
            parser->output.add_end(ins::l(reg, device, variable));
            return;
        }
        // else direction == "->"

        reg = parse_value(reg, parser->globals);
        if(starts_with(device, "*"))
        {
            device.erase(device.begin());
            device = parser->globals.references.get(device);
            parser->output.add_end(ins::sb(device, variable, reg));
            return;
        }

        device = parser->globals.references.get(device);
        parser->output.add_end(ins::s(device, variable, reg));
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