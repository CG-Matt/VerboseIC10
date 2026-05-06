#include <string>
#include <unordered_map>
#include "commands.hpp"
#include "syntax.hpp"
#include "instructions.hpp"
#include "parser.hpp"

namespace Utilities
{
    std::string B_compare(std::string_view comparator, const std::string& reg, const std::string& value, const std::string& label)
    {
        std::string comp = std::string(syntax::logic::toInstruction(comparator));

        return value == "0" ? ins::branch_z(comp, reg, label) : ins::branch(comp, reg, value, label);
    }
    std::string BR_compare(std::string_view comparator, const std::string& reg, const std::string& value, const std::string& lines)
    {
        std::string comp = std::string(syntax::logic::toInstruction(comparator));

        return value == "0" ? ins::branch_relative_z(comp, reg, lines) : ins::branch_relative(comp, reg, value, lines);
    }
    std::string RA_compare(std::string_view comparator, const std::string& reg, const std::string& value, const std::string& label)
    {
        std::string comp = std::string(syntax::logic::toInstruction(comparator));

        return value == "0" ? ins::sub_call_z(comp, reg, label) : ins::sub_call(comp, reg, value, label);
    }
}

namespace c_commands
{
    void dev(vmc::ArgumentList args)
    {
        // Ensure that devices aren't already initialised
        if(Parser::flags.devices_initialised == true)
            throw vmc::ParserError("Reinitialisation of devices not allowed.");

        // Ensure that we have devices available
        if(!Parser::device::available())
            throw vmc::ParserError("Max device limit reached.");
        
        const std::string& target = args.next_checked("Missing target device.");

        if(syntax::devices::isValid(target))
        {
            const std::string& name = args.next_checked("Missing device name.");
            if(Parser::ident::exists(name)) throw vmc::ParserError("Device with name \"" + name + "\" already exists.");
            Parser::registerIdentifier(Identifier::Type::DEVICE, target, name);
            Parser::device::take(target);
            return;
        }
        else if(target == "*")
        {
            if(!args.expect("="))
                throw vmc::ParserError("Expected '=' after device wildcard.");

            Parser::flags.devices_initialised = true;
            std::vector<std::string> arr = Parser::Utilities::ParseArray(args);
            if(Parser::hasError()){ return; }
            if(arr.size() > 6) throw vmc::ParserError("Too many devices listed. (" + std::to_string(arr.size()) + "/6)");
            for(std::size_t i = 0; i < arr.size(); i++)
            {
                if(Parser::ident::exists(arr[i])){ throw vmc::DuplicateReferenceError(arr[i]); }
                Parser::registerIdentifier(Identifier::Type::DEVICE, Parser::device::next(), arr[i]);
            }
            return;
        }

        throw vmc::ParserError(std::string("Unknown device target '") + target + "'.");
    }
    void reg(vmc::ArgumentList args)
    {
        if(args.isEmpty()){ throw vmc::ParserError("Expected register declaration: reg <name> or reg [name1, name2, ...]"); }
        const std::string& first = args.peek();
        if(!first.empty() && first.front() == '[')
        {
            std::vector<std::string> arr = Parser::Utilities::ParseArray(args);
            if(Parser::hasError()){ return; }
            for(auto &entry : arr)
            {
                std::string free_reg = Parser::ident::getNextFreeRegister();
                if(Parser::ident::exists(entry)){ throw vmc::DuplicateReferenceError(entry); }
                Parser::registerIdentifier(Identifier::Type::REGISTER, free_reg, entry);
            }
            return;
        }

        if(Parser::ident::exists(first)){ throw vmc::DuplicateReferenceError(first); }
        std::string reg = Parser::ident::getNextFreeRegister();
        Parser::registerIdentifier(Identifier::Type::REGISTER, reg, first);
    }
    void set(vmc::ArgumentList args)
    {
        std::string reg = args.next();
        if(!args.expect("=")){ throw vmc::ParserError("Expected '=' in assignment."); }
        std::string value = args.next();

        if(!Parser::ident::exists(reg))
        {
            throw vmc::UnknownIdentifier(reg);
        }

        if(value.empty())
        {
            throw vmc::MissingValueError();
        }

        reg = Parser::ident::getTarget(reg);
        value = Parser::Utilities::ParseValue(value);

        Parser::output.push_back(ins::move(reg, value));
    }
    void label(vmc::ArgumentList args)
    {
        const std::string& name = args.next();
        if(name.empty()){ throw vmc::ParserError("Label name required"); }
        Parser::registerLabel(name);

        Parser::output.push_back(ins::label(name));
    }
    void eport(vmc::ArgumentList args)
    {
        Device target = Parser::Utilities::ParseDevice(args.next());
        if(Parser::hasError()){ return; }
        if(!args.expect("=")){ throw vmc::ParserError("Expected '=' in export command."); }
        std::string value = args.next();
        
        if(value.empty())
        {
            throw vmc::MissingValueError();
        }

        value = Parser::Utilities::ParseValue(value);

        if(target.is_prefabhash)
        {
            Parser::output.push_back(ins::sb(target.name, target.variable, value));
            return;
        }

        Parser::output.push_back(ins::s(target.name, target.variable, value));
    }
    void wait(vmc::ArgumentList args)
    {
        if(args.isEmpty())
            Parser::output.push_back(ins::yld());
        else
            // The first argument is the time to sleep
            Parser::output.push_back(ins::sleep(args[0]));
    }
    void move(vmc::ArgumentList args)
    {
        const std::string& lines = args.next_checked("Number of lines to move not found.");

        if(!args.hasNext()){ Parser::output.push_back(ins::jr(lines)); return; }
        if(!args.expect("if")){ throw vmc::ParserError("Expected 'if' in move condition."); }

        std::string reg = args.next();
        const std::string& compare = args.next();
        const std::string& value = args.next();

        if(!Parser::ident::exists(reg))
        {
            throw vmc::UnknownIdentifier(reg);
        }

        if(!syntax::logic::isValidOperation(compare))
        {
            throw vmc::InvalidComparatorError(compare);
        }

        if(value.empty())
        {
            throw vmc::ParserError("Missing constant.");
        }

        reg = Parser::ident::getTarget(reg);
        Parser::output.push_back(Utilities::BR_compare(compare, reg, value, lines));
    }
    void math(vmc::ArgumentList args)
    {
        std::string reg = args.next();
        const std::string& set_op = args.next();
        std::string var1 = args.next();
        std::string op;
        std::string var2;
        bool is_math_function = false;

        if(set_op == "+=" || set_op == "-=" || set_op == "*=" || set_op == "/=")
        {
            op = set_op[0];
            var2 = var1;
            var1 = reg;
        }
        else if(set_op == "=")
        {
            if(!syntax::math::isFunction(var1))
            {
                op = args.next();
                var2 = args.next();
            }
            else
            {
                op = var1;
                if(!args.expect("<-")){ throw vmc::ParserError("Invalid math function call"); }
                var1 = args.next();
                is_math_function = true;
            }
        }
        else
        {
            throw vmc::InvalidMathOpError(set_op);
        }

        if(is_math_function && var1.empty())
        {
            throw vmc::MissingValueError();
        }

        if(!is_math_function && var2.empty())
        {
            throw vmc::MissingValueError();
        }

        if(!Parser::ident::exists(reg))
        {
            throw vmc::UnknownIdentifier(reg);
        }

        reg = Parser::ident::getTarget(reg);
        var1 = Parser::Utilities::ParseValue(var1);

        if(!is_math_function)
        {
            var2 = Parser::Utilities::ParseValue(var2);
            if(!syntax::math::isValidSymbol(op)) throw vmc::InvalidMathOpError(op);
            op = syntax::math::toInstruction(op);
            Parser::output.push_back(ins::math(op, reg, var1, var2));
            return;
        }

        Parser::output.push_back(ins::math_func(op, reg, var1));
    }
    void jump(vmc::ArgumentList args)
    {
        if(!args.expect("=>")){ throw vmc::ParserError("Expected '=>' in jump command."); }
        const std::string& label = args.next();

        if(!Parser::label::exists(label)){ Parser::label::expect(label); }

        Parser::output.push_back(ins::j(label));
    }
    void import(vmc::ArgumentList args)
    {
        std::string reg = args.next();
        if(!args.expect("=")){ throw vmc::ParserError("Expected '=' in import command."); }
        Device target = Parser::Utilities::ParseDevice(args.next());
        if(Parser::hasError()){ return; }

        if(!Parser::ident::exists(reg))
        {
            throw vmc::UnknownIdentifier(reg);
        }

        reg = Parser::ident::getTarget(reg);

        Parser::output.push_back(ins::l(reg, target.name, target.variable));
    }
    void branch(vmc::ArgumentList args)
    {
        if(!args.expect("=>"))
            throw vmc::ParserError("Expected '=>' in branch command.");

        const std::string& label = args.next_checked("Missing branch destination label.");
        if(!args.expect("if"))
            throw vmc::ParserError("Expected 'if' in branch command.");

        std::string var1 = args.next();
        const std::string& compare = args.next();
        std::string var2 = args.next();

        if(var1.empty())
        {
            throw vmc::MissingValueError();
        }

        if(var2.empty())
        {
            throw vmc::MissingValueError();
        }

        var1 = Parser::Utilities::ParseValue(var1);
        var2 = Parser::Utilities::ParseValue(var2);

        if(!syntax::logic::isValidOperation(compare))
        {
            throw vmc::InvalidComparatorError(compare);
        }

        if(!Parser::label::exists(label)){ Parser::label::expect(label); }

        Parser::output.push_back(Utilities::B_compare(compare, var1, var2, label));
    }
    void trans(vmc::ArgumentList args)
    {
        if(!Parser::flags.using_carry){ throw vmc::ParserError("You must specify the use of carry to use the trans command (#using carry)"); }

        Device source = Parser::Utilities::ParseDevice(args.next());
        if(Parser::hasError()){ return; }

        if(!args.expect("=>")){ throw vmc::ParserError("Expected '=>' in trans command."); }

        Device destination = Parser::Utilities::ParseDevice(args.next());
        if(Parser::hasError()){ return; }

        std::string eport;

        if(destination.is_prefabhash)
        {
            eport = ins::sb(destination.name, destination.variable, Parser::ident::getTarget("carry"));
        }
        else
        {
            eport = ins::s(destination.name, destination.variable, Parser::ident::getTarget("carry"));
        }

        std::string eport2 = ins::l(Parser::ident::getTarget("carry"), source.name, source.variable);

        Parser::output.push_back(eport2 + "\n" + eport);
    }
    void p_if(vmc::ArgumentList args)
    {
        if(Parser::flags.in_conditional)
        {
            throw vmc::ParserError("Nested conditions currently unsupported.");
        }

        ConditionalInfo labels = Parser::label::generateConditionals();

        std::string reg;
        std::string comparator;
        std::string value;

        if(!args.hasNext()) throw vmc::ParserError("'if' keyword must be followed by a condition.");

        while(args.hasNext())
        {
            reg = args.next_checked("Missing variable to check.");
            comparator = args.next_checked("Missing comparator.");
            value = args.next_checked("Missing value to compare against.");

            if(!Parser::ident::exists(reg))
            {
                throw vmc::UnknownIdentifier(reg);
            }

            reg = Parser::ident::getTarget(reg);
            value = Parser::Utilities::ParseValue(value);
            if(!syntax::logic::isValidOperation(comparator))
            {
                throw vmc::InvalidComparatorError(comparator);
            }
            comparator = syntax::logic::invertRelation(comparator);
            Parser::output.push_back(Utilities::B_compare(comparator, reg, value, labels.fail_label));
            if(args.hasNext())
            {
                if(syntax::logic::isValidCombinator(args.peek()))
                    args.next();
            }
        }

        Parser::flags.in_conditional = true;
    }
    void p_else(vmc::ArgumentList args)
    {
        if(!args.isEmpty())
            throw vmc::ParserError("Unexpected data after 'else' keyword.");

        if(!Parser::flags.in_conditional){ throw vmc::ParserError("Conditional not initialised correctly"); }

        Parser::flags.is_conditional_else = true;

        Parser::output.push_back(ins::j(Parser::label::getCurrentConditionals().end_label));
        Parser::output.push_back(ins::label(Parser::label::getCurrentConditionals().fail_label));
    }
    void end(vmc::ArgumentList args)
    {
        const std::string& end_type = args.next_checked("Expected block type after 'end' keyword.");

        if(end_type == "if")
        {
            if(!Parser::flags.in_conditional)
                throw vmc::ParserError("Conditional not initialised correctly");

            Parser::output.push_back(ins::label(
                Parser::flags.is_conditional_else ?
                Parser::label::getCurrentConditionals().end_label :
                Parser::label::getCurrentConditionals().fail_label
            ));

            Parser::flags.in_conditional = false;
            Parser::flags.is_conditional_else = false;
        }
        else if(end_type == "sub")
        {
            if(!Parser::flags.in_subroutine)
                throw vmc::ParserError("Subroutine not initialised correctly");
            
            Parser::flags.in_subroutine = false;
            Parser::output.push_back(ins::j("ra"));
        }
        else
        {
            throw vmc::ParserError("Unexpected identifier after 'end' keyword.");
        }
    }
    void xref(vmc::ArgumentList args)
    {
        if(args.size() < 3){ throw vmc::ParserError("Expected xref command: xref <device.variable> <-|-> <register-or-value>"); }
        
        Device target = Parser::Utilities::ParseDevice(args.next());
        if(Parser::hasError()){ return; }
        
        const std::string& direction = args.next();
        if(direction != "<-" && direction != "->"){ throw vmc::ParserError("Invalid direction \"" + direction + "\" provided"); }
        std::string reg = args.next();

        if(direction == "->")
        {
            if(!Parser::ident::exists(reg))
            {
                throw vmc::UnknownIdentifier(reg);
            }

            reg = Parser::ident::getTarget(reg);
            Parser::output.push_back(ins::l(reg, target.name, target.variable));
            return;
        }
        // else direction == "<-"

        reg = Parser::Utilities::ParseValue(reg);
        if(target.is_prefabhash)
        {
            Parser::output.push_back(ins::sb(target.name, target.variable, reg));
            return;
        }

        Parser::output.push_back(ins::s(target.name, target.variable, reg));
    }
    void p_const(vmc::ArgumentList args)
    {
        // Const will create a reference to a value without using a register
        if(args.size() < 3){ throw vmc::ParserError("Expected const declaration: const <name> = <value>"); }
        
        const std::string& const_name = args.next();
        if(!args.expect("=")){ throw vmc::ParserError("Expected '=' in const command."); }
        const std::string& value = args.next();

        if(Parser::ident::exists(const_name)){ throw vmc::DuplicateReferenceError(const_name); }
        Parser::registerIdentifier(Identifier::Type::CONSTANT, value, const_name);
    }
    void sub(vmc::ArgumentList args)
    {
        if(args.isEmpty()){ throw vmc::ParserError("Expected subroutine declaration: sub <label>"); }

        const std::string& label_name = args.next();

        if(Parser::label::exists(label_name)){ throw vmc::ParserError("Label with name \"" + label_name + "\" already exists."); }

        Parser::flags.in_subroutine = true;        
        Parser::registerLabel(label_name);
        Parser::output.push_back(ins::label(label_name));
    }
    void call(vmc::ArgumentList args)
    {
        if(args.isEmpty()){ throw vmc::ParserError("Expected subroutine call: call <label> or call <label> if <left> <comparator> <right>"); }

        const std::string& label_name = args.next();

        if(!Parser::label::exists(label_name)){ Parser::label::expect(label_name); }

        if(args.size() < 2){ Parser::output.push_back(ins::jal(label_name)); return; }

        if(!args.expect("if")){ throw vmc::ParserError("Expected 'if' in call condition."); }

        std::string var1 = args.next();
        const std::string& compare = args.next();
        std::string var2 = args.next();

        if(!syntax::logic::isValidOperation(compare)) { throw vmc::InvalidComparatorError(compare); }

        if(var1.empty())
        {
            throw vmc::MissingValueError();
        }

        if(var2.empty())
        {
            throw vmc::MissingValueError();
        }

        var1 = Parser::Utilities::ParseValue(var1);
        var2 = Parser::Utilities::ParseValue(var2);

        Parser::output.push_back(Utilities::RA_compare(compare, var1, var2, label_name));
    }
};

static const std::unordered_map<std::string_view, Cmd::Function> commands_map =
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

bool Cmd::Exists(std::string_view cmd_str) noexcept
{
    return commands_map.find(cmd_str) != commands_map.end();
}

const Cmd::Function& Cmd::Get(std::string_view cmd_str)
{
    return commands_map.at(cmd_str);
}
