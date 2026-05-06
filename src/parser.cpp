#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include "data_structs.hpp"
#include "commands.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include "syntax.hpp"

Device Parser::Utilities::ParseDevice(std::string_view device_in)
{
    Device device_out;

    if(device_in.empty())
    {
        Parser::setError(vmc::GenericError("Cannot parse empty device string"));
        return device_out;
    }

    if(device_in.front() == '@') // Alias for PrefabHash
    {
        if(device_in.find('.') != device_in.npos)
        {
            Parser::setError(vmc::GenericError("A device cannot have a prefix and a variable name defined. (Excluding * for PrefabHash use)"));
            return device_out;
        }
        device_in.remove_prefix(1);

        if(!Parser::ident::exists(device_in) || Parser::ident::getType(device_in) != Identifier::Type::DEVICE)
            throw vmc::UnknownIdentifier(std::string(device_in));

        device_out.name = Parser::ident::getTarget(device_in);
        device_out.variable = "PrefabHash";
        return device_out;
    }
    if(device_in.front() == '*') // Use prefabhash
    {
        device_in.remove_prefix(1);
        device_out.is_prefabhash = true;
    }

    std::vector<std::string> data = SplitString(device_in, '.');
    
    if(data.size() != 2)
    {
        Parser::setError(vmc::GenericError("Invalid device \"" + std::string(device_in) + "\""));
        return device_out;
    }

    if(!Parser::ident::exists(data[0]))
    {
        Parser::setError(vmc::GenericError("Invalid device \"" + std::string(device_in) + "\""));
        return device_out;
    }

    device_out.name = Parser::ident::getTarget(data[0]);
    if(device_out.name.empty()) throw vmc::UnknownIdentifier(data[0]);
    device_out.variable = data[1];
    return device_out;
}
std::vector<std::string> Parser::Utilities::ParseArray(vmc::ArgumentList arguments)
{
    std::string line;

    // Turn the array into a string
    while(arguments.hasNext()) line += arguments.next();
    
    if(line.empty())
    {
        Parser::setError(vmc::GenericError("No array data to parse."));
        return std::vector<std::string>();
    }

    if(line.front() != '[') // Return error if first char does not open array
    {
        Parser::setError(vmc::GenericError("First character does not open array."));
        return std::vector<std::string>();
    }

    // Technically this has different functionality to the original,
    // but this needs to be reworked anyways because it does not function properly.
    if(line.back() != ']') // Return error if array does not end with a ']' character.
    {
        Parser::setError(vmc::GenericError("No array terminator found at end. \"]\" expected."));
        return std::vector<std::string>();
    }

    // Should also perform checks to see if we have more than 1 opening and closing brackets

    size_t terminator_idx = line.find_first_of(']');
    if(terminator_idx != line.size())
    {
        line.erase(terminator_idx + 1);
    }

    line.erase(line.begin());
    line.erase(line.end() - 1);
    return SplitString(line, ',');
}

std::string Parser::Utilities::ParseValue(const std::string& value)
{
    if(syntax::isBoolean(value)){ return ParseBoolean(value); }
    if(syntax::isNumber(value)){ return value; }
    
    // This throws if value is not an identifier.
    return Parser::ident::getTarget(value);
}

std::string Parser::Utilities::ParseBoolean(std::string_view boolean){ return boolean == "true" ? "1" : "0"; }

namespace Parser::state
{
    static std::size_t syntax_version = 0;
    static std::unordered_set<std::string_view> _available_devices;
    static std::unordered_set<std::string_view> _available_registers;
};

namespace Parser::ident
{
    static std::unordered_map<std::string, Identifier> _output_bindings;
};

namespace Parser::label
{
    static std::unordered_set<std::string> _registered_labels;
    static std::vector<vmc::IndexedStr> _unresolved_labels;
    static std::size_t _conditional_label_index = 0;
    static ConditionalInfo _conditional;
};

namespace Parser
{
    static std::vector<std::string> errors;
    static std::vector<ProgramLine> directives;
    static std::vector<ProgramLine> input;
    static std::size_t current_line = 1;
    static const std::string _empty_str = "";

    static void _ParseFile(const Buffer<char>& file_contents)
    {
        const char* start = file_contents.data;
        std::size_t length = 0;
        std::size_t line_index = 1;

        for(std::size_t i = 0; i < file_contents.size; i++)
        {
            // Skip any blank characters
            while(i + 1 < file_contents.size && std::isblank(file_contents[i])) i++;

            // Look for next line
            if(file_contents[i] == '\n')
            {
                line_index++;
                continue;
            }

            // If other whitespace character, just loop back
            if(std::isspace(file_contents[i])) continue;

            if(file_contents[i] == '$')
            {
                while(i < file_contents.size && (file_contents[i] == '\t' || !std::iscntrl(file_contents[i]))) i++;
                i--;
            }

            else if(file_contents[i] == '#')
            {
                start = file_contents.data + i;

                while(
                    i < file_contents.size &&
                    file_contents[i] != '$' &&
                    (file_contents[i] == '\t' || !std::iscntrl(file_contents[i]))
                ) {
                    length++;
                    i++;
                }

                directives.push_back(ProgramLine(line_index, std::string_view(start, length)));
                length = 0;
                i--;
            }
            else
            {
                start = file_contents.data + i;

                while(
                    i < file_contents.size &&
                    file_contents[i] != '$' &&
                    (file_contents[i] == '\t' || !std::iscntrl(file_contents[i]))
                ) {
                    length++;
                    i++;
                }

                input.push_back(ProgramLine(line_index, std::string_view(start, length)));
                length = 0;
                i--;
            }
        }
    }

    static void _ParseDirectives(void)
    {
        for(auto &rdirective : Parser::directives)
        {
            const std::string& cur_directive = rdirective.first;
            const std::vector<std::string>& args = rdirective.args;

            if(cur_directive == "version")
            {
                if(args.empty()){ Parser::setError(rdirective.line_number, vmc::GenericError("No version number provided")); continue; }
                try
                {
                    state::syntax_version = std::stoi(args[0]);
                }
                catch(const std::exception& e)
                {
                    Parser::setError(rdirective.line_number, vmc::GenericError("Invalid version number formatting")); continue;
                }
            }
            if(cur_directive == "using")
            {
                if(args.empty()){ Parser::setError(rdirective.line_number, vmc::GenericError("No module name provided")); continue; }
                const std::string& feature = args[0];

                if(feature == "carry")
                {
                    Parser::registerIdentifier(Identifier::Type::REGISTER, "r15", "carry");
                    Parser::reg::take("r15");
                    flags.using_carry = true;
                }
            }
        }
    }
};

void Parser::registerIdentifier(Identifier::Type type, std::string_view target, std::string_view name)
{
    ident::_output_bindings.insert({ std::string(name), Identifier{ type, std::string(target) } });
}

std::size_t Parser::getCurrentLine(void)
{
    return Parser::current_line;
}

bool Parser::ident::exists(std::string_view name)
{
    return _output_bindings.find(std::string(name)) != _output_bindings.end();
}

Identifier::Type Parser::ident::getType(std::string_view name)
{
    if(!Parser::ident::exists(name)) throw vmc::UnknownIdentifier(std::string(name));
    return _output_bindings.at(std::string(name)).type;
}

const std::string& Parser::ident::getTarget(std::string_view name)
{
    if(!Parser::ident::exists(name)) throw vmc::UnknownIdentifier(std::string(name));
    return _output_bindings.at(std::string(name)).target;
}

std::string Parser::ident::getNextFreeRegister(void)
{
    if(!Parser::reg::available())
    {
        Parser::setError(vmc::GenericError("Max register limit has been reached"));
        return _empty_str;
    }
    return std::string(Parser::reg::next());
}

const std::unordered_map<std::string, Identifier>& Parser::ident::getAll(void)
{
    return _output_bindings;
}

void Parser::registerLabel(std::string_view label)
{
    label::_registered_labels.insert(std::string(label));

    for(std::size_t i = label::_unresolved_labels.size(); i-- > 0;)
    {
        if(label::_unresolved_labels[i].str != label) continue;

        if(i != label::_unresolved_labels.size() - 1)
            label::_unresolved_labels[i] = std::move(label::_unresolved_labels.back());

        label::_unresolved_labels.pop_back();
    }
}

bool Parser::label::exists(std::string_view label)
{
    return _registered_labels.find(std::string(label)) != _registered_labels.end();
}

void Parser::label::expect(std::string_view label)
{
    _unresolved_labels.push_back({ Parser::getCurrentLine(), std::string(label) });
}

ConditionalInfo Parser::label::generateConditionals(void)
{
    _conditional.pass_label = "cp" + std::to_string(_conditional_label_index);
    _conditional.fail_label = "cf" + std::to_string(_conditional_label_index);
    _conditional.end_label  = "ce" + std::to_string(_conditional_label_index);

    _conditional_label_index++;

    Parser::registerLabel(_conditional.pass_label);
    Parser::registerLabel(_conditional.fail_label);
    Parser::registerLabel(_conditional.end_label);

    return _conditional;
}

const ConditionalInfo& Parser::label::getCurrentConditionals(void)
{
    return _conditional;
}

namespace Parser::device
{
    bool available(void)
    {
        return !state::_available_devices.empty();
    }

    void take(std::string_view target)
    {
        if(!syntax::devices::isValid(target))
            throw vmc::ParserError("Cannot reserve invalid device target \"" + std::string(target) + "\".");

        if(state::_available_devices.find(target) == state::_available_devices.end())
            throw vmc::ParserError("Device target \"" + std::string(target) + "\" is already reserved or unavailable.");

        state::_available_devices.erase(target);
    }

    std::string_view next(void)
    {
        static std::size_t next_device_index = 0;
        std::string_view next_device;

        do
        {
            if(next_device_index >= syntax::devices::count())
                throw std::runtime_error("Parser::device::next(): No more device targets available.");

            next_device = syntax::devices::getAll()[next_device_index++];
        }
        while(state::_available_devices.find(next_device) == state::_available_devices.end());

        state::_available_devices.erase(next_device);
        return next_device;
    }
}

namespace Parser::reg
{
    bool available(void)
    {
        return !state::_available_registers.empty();
    }

    void take(std::string_view target)
    {
        if(!syntax::registers::isValid(target))
            throw vmc::ParserError("Cannot reserve invalid register target \"" + std::string(target) + "\".");

        if(state::_available_registers.find(target) == state::_available_registers.end())
            throw vmc::ParserError("Register target \"" + std::string(target) + "\" is already reserved or unavailable.");

        state::_available_registers.erase(target);
    }

    std::string_view next(void)
    {
        static std::size_t next_register_index = 0;
        std::string_view next_register;

        do
        {
            if(next_register_index >= syntax::registers::count())
                throw std::runtime_error("Parser::reg::next(): No more register targets available.");

            next_register = syntax::registers::getAll()[next_register_index++];
        }
        while(state::_available_registers.find(next_register) == state::_available_registers.end());

        state::_available_registers.erase(next_register);
        return next_register;
    }
}

void Parser::init()
{
    // Register available devices
    for(const std::string_view& dev : syntax::devices::getAll())
    {
        state::_available_devices.insert(dev);
    }

    // Register available registers
    for(const std::string_view& reg : syntax::registers::getAll())
    {
        state::_available_registers.insert(reg);
    }

    // Initialise globals
    Parser::registerIdentifier(Identifier::Type::DEVICE, "db", "Self");
}

void Parser::parse(const Buffer<char>& file_contents)
{
    // Parse file
    Parser::_ParseFile(file_contents);
    // Parse directives
    Parser::_ParseDirectives();

    if(Parser::hasError()) return;

    if(state::syntax_version != syntax::getCurrentVersion())
    {
        Parser::setError(vmc::GenericError("Version mismatch. Expected: " + std::to_string(syntax::getCurrentVersion()) + ", Found: " + std::to_string(state::syntax_version)));
        return;
    }

    Parser::output.reserve(Parser::input.size());

    // Calling each command in the file
    for(ProgramLine& rcmd : Parser::input)
    {
        if(Parser::hasError()){ break; }

        Cmd::Function command;

        Parser::current_line = rcmd.line_number;
        if(!Cmd::Exists(rcmd.first))
        {
            if(!Parser::ident::exists(rcmd.first)){ Parser::setError(rcmd.line_number, vmc::GenericError("Command \"" + rcmd.first + "\" is not a valid command")); continue; }
            
            if(Parser::ident::getType(rcmd.first) == Identifier::Type::CONSTANT){ Parser::setError(rcmd.line_number, vmc::GenericError("Identifier \"" + rcmd.first + "\" is const and cannot be modified.")); continue; }
            
            if(!Cmd::Exists("set"))  { Parser::setError(rcmd.line_number, vmc::GenericError("'set' command not found."));  continue; } // Hopefully will prevent crashes if set is ever removed
            if(!Cmd::Exists("math")) { Parser::setError(rcmd.line_number, vmc::GenericError("'math' command not found.")); continue; } // Hopefully will prevent crashes if math is ever removed
            
            if(!rcmd.args.empty() && rcmd.args[0] == "=" && rcmd.args.size() == 2)
                command = Cmd::Get("set");
            else
                command = Cmd::Get("math");
            rcmd.args.insert(rcmd.args.begin(), rcmd.first);
        }
        else
            command = Cmd::Get(rcmd.first);

        vmc::ArgumentList arguments(rcmd.args);

        try { command(arguments); }
        catch(const vmc::ParserError& error)
        {
            Parser::setError(error.what());
        }
    }
    
    if(Parser::hasError()) return;

    if(Parser::label::_unresolved_labels.empty()) return;

    // Sort unresolved labels by ascending line number
    std::sort(
        Parser::label::_unresolved_labels.begin(),
        Parser::label::_unresolved_labels.end(),
        [](const vmc::IndexedStr& a, const vmc::IndexedStr& b)
        {
            return a.line_number < b.line_number;
        }
    );

    for(auto& line : Parser::label::_unresolved_labels)
        Parser::setError(line.line_number, vmc::UnregisteredLabelError(line.str));
}

void Parser::setError(std::string error)
{
    Parser::errors.push_back(std::string("(") + std::to_string(Parser::getCurrentLine()) + ") " + error);
}
void Parser::setError(std::size_t index, std::string error)
{
    Parser::errors.push_back(std::string("(") + std::to_string(index) + ") " + error);
}
bool Parser::hasError() { return Parser::errors.size() > 0; }
const std::vector<std::string>& Parser::getErrors() { return Parser::errors; }
