#include <string>
#include <vector>
#include <algorithm>
#include "data_structs.h"
#include "commands.h"
#include "parser.h"
#include "constants.h"
#include "utils.h"

static const short version = 4;

PReference::PReference(const std::string& internal, const std::string& external)
{
    m_internal = internal;
    m_external = external;
}

void ParserReferences::add(const std::string& external_name, const std::string& internal_name, ref_type ref_type)
{
    m_definitions.push_back(PReference(internal_name, external_name));
    
    if(ref_type == ParserReferences::ref_type::reg)
    {
        this->defined_registers.add_end(external_name);
    }
}
/*Checks if a reference with the supplied external name exists*/
bool ParserReferences::exists(const std::string& external_name) const
{
    auto it = std::find_if(m_definitions.begin(), m_definitions.end(), [&external_name](const PReference ref)
    {
        return ref.m_external == external_name;
    });
    return it != m_definitions.end();
}
std::string ParserReferences::get(const std::string& external_name) const
{
    auto res = std::find_if(m_definitions.begin(), m_definitions.end(), [&external_name](const PReference ref)
    {
        return ref.m_external == external_name;
    });
    if(res != m_definitions.end())
    {
        return m_definitions[res - m_definitions.begin()].m_internal;
    }
    return "";
}
std::string ParserReferences::get_ext(const std::string& internal_name) const
{
    auto res = std::find_if(m_definitions.begin(), m_definitions.end(), [&internal_name](const PReference ref)
    {
        return ref.m_internal == internal_name;
    });
    if(res != m_definitions.end())
    {
        return m_definitions[res - m_definitions.begin() + 1].m_external;
    }
    return "";
}
std::string ParserReferences::get_free() const
{
    for (auto &reg : valid_registers)
    {
        auto res = std::find_if(m_definitions.begin(), m_definitions.end(), [&reg](const PReference ref)
        {
            return ref.m_internal == reg;
        });
        if(res == m_definitions.end())
        {
            return reg;
        }
    }
    return "";
}



ConditionalInfo ParserGlobals::generate_conditional_labels()
{
    int current_index = conditional.idx;
    conditional.idx++;
    conditional.pass_label = "cp" + std::to_string(current_index);
    conditional.fail_label = "cf" + std::to_string(current_index);
    conditional.end_label  = "ce" + std::to_string(current_index);

    register_label({ conditional.pass_label, conditional.fail_label, conditional.end_label });

    return conditional;
}
void ParserGlobals::register_label(const std::string& label)
{
    this->labels.push_back(label);

    if(this->unresolved_labels.size() < 1){ return; }

    std::vector<vmc::Line>::iterator it = this->unresolved_labels.begin();
    while(it != this->unresolved_labels.end())
    {
        auto it = std::find_if(this->unresolved_labels.begin(), this->unresolved_labels.end(), [&label](const vmc::Line& line)
        {
            return line.m_data == label;
        });
        if(it == this->unresolved_labels.end()){ break; }

        this->unresolved_labels.erase(it);
    }
}
void ParserGlobals::register_label(const std::vector<std::string>& labels)
{
    for(auto& label : labels)
    {
        this->labels.push_back(label);

        if(this->unresolved_labels.size() < 1){ return; }

        std::vector<vmc::Line>::iterator it = this->unresolved_labels.begin();
        while(it != this->unresolved_labels.end())
        {
            auto it = std::find_if(this->unresolved_labels.begin(), this->unresolved_labels.end(), [&label](const vmc::Line& line)
            {
                return line.m_data == label;
            });
            if(it == this->unresolved_labels.end()){ break; }

            this->unresolved_labels.erase(it);
        }
    }
}
bool ParserGlobals::label_exists(const std::string& label)
{
    return includes(labels, label);
}



void ParserUtils::set_parent(Parser* parent){ m_parent = parent; }
/*
    Parses the provided device into a name and variable and whether or not it is a PrefabHash.
    If an error occurs it adds it to m_parent->errors and sets the parsers error flag.
*/
Device ParserUtils::parse_device(std::string& device_in)
{
    Device device_out;

    if(starts_with(device_in, "@")) // Alias for PrefabHash
    {
        if(includes(device_in, '.'))
        {
            m_parent->set_error(vmc::GenericError(m_parent->get_current_line(), "A device cannot have a prefix and a variable name defined. (Excluding * for PrefabHash use)"));
            return device_out;
        }
        device_in.erase(device_in.begin());
        device_out.name = m_parent->globals.references.get(device_in);
        if(device_out.name.size() < 1){ m_parent->set_error(vmc::UndefinedDeviceError(m_parent->get_current_line(), device_in)); }
        device_out.variable = "PrefabHash";
        return device_out;
    }
    if(starts_with(device_in, "*")) // Use prefabhash
    {
        device_in.erase(device_in.begin());
        device_out.is_prefabhash = true;
    }

    /*Check if the device variable requested is valid*/
    if(!includes(device_in, '.'))
    {
        m_parent->set_error(vmc::GenericError(m_parent->get_current_line(), "Inavlid device \"" + device_in + "\""));
        return device_out;
    }

    std::vector<std::string> data = split_string(device_in, '.');
    device_out.name = m_parent->globals.references.get(data[0]);
    if(device_out.name.size() < 1){ m_parent->set_error(vmc::UndefinedDeviceError(m_parent->get_current_line(), data[0])); }
    device_out.variable = data[1];
    return device_out;
}
/*
    Parses the provided string array view into a std::vector<std::string>.
    If an error occurs it adds it to m_parent->errors and sets the parsers error flag.
*/
std::vector<std::string> ParserUtils::parse_array(const vmc::string_array_view& view)
{
    std::string line = join_string(view); // Turn the array into a string
    
    if(!starts_with(line, "[")) // Return error if first char does not open array
    {
        m_parent->set_error(vmc::GenericError(m_parent->get_current_line(), "First character does not open array."));
        return std::vector<std::string>();
    }

    if(!includes(line, ']')) // Return error if no array terminator is found
    {
        m_parent->set_error(vmc::GenericError(m_parent->get_current_line(), "No array terminator found. \"]\" expected."));
        return std::vector<std::string>();
    }

    size_t terminator_idx = line.find_first_of(']');
    if(terminator_idx != line.size())
    {
        line.erase(terminator_idx + 1);
    }

    line.erase(line.begin());
    line.erase(line.end() - 1);
    return split_string(line, ',');
}
std::string ParserUtils::parse_value(const std::string& value)
{
    if(is_boolean(value)){ return parse_boolean(value); }
    if(is_number(value)){ return value; }
    if(m_parent->globals.references.exists(value)){ return m_parent->globals.references.get(value); }

    m_parent->set_error(vmc::UndefinedRegisterError(m_parent->get_current_line(), value));
    return value;
}



Parser::Parser(std::vector<vmc::Line> &file_contents)
{
    // Initialise utils
    this->utils.set_parent(this);
    // Initialise globals
    p_init_globals();
    // Parse file
    p_parse_file(file_contents);
    // Parse directives
    p_parse_directives();
}
void Parser::p_init_globals()
{
    globals.references.add("Self", "db", ParserReferences::ref_type::dev);
}
void Parser::p_parse_file(std::vector<vmc::Line>& file_contents)
{
    this->m_input.reserve(file_contents.size()); // Reduced mallocs by ~40
    for(auto& line : file_contents)
    {
        std::string& text = line.m_data;

        if(text.size() < 1){ continue; } // Ensure that the line is not empty
        if(text.find_first_not_of(' ') == std::string::npos){ continue; } // Ensure that the line isnt just spaces

        while(starts_with(text, " ")){ text.erase(text.begin()); }

        if(starts_with(text, "$")){ continue; } // Ignore Comments
        if(starts_with(text, "#"))
        {
            this->m_directives.push_back(ProgramLine(text, line.m_idx+1));
            continue;
        }

        this->m_input.push_back(ProgramLine(text, line.m_idx+1));
    }
}
void Parser::p_parse_directives()
{
    for(auto &rdirective : m_directives)
    {
        std::string& cur_directive = rdirective.m_first;
        vmc::string_array& args = rdirective.m_args;

        if(cur_directive == "version")
        {
            if(!args.contains_data()){ this->set_error(vmc::GenericError(rdirective.m_line_idx, "No version number provided")); continue; }
            flags.version = std::stoi(args[0]);
        }
        if(cur_directive == "using")
        {
            if(!args.contains_data()){ this->set_error(vmc::GenericError(rdirective.m_line_idx, "No module name provided")); continue; }
            std::string &feature = args[0];

            if(feature == "carry")
            {
                globals.references.add("carry", "r15", ParserReferences::ref_type::reg);
                flags.available_registers--;
                flags.using_carry = true;
            }
        }
    }
}
void Parser::parse()
{
    if(flags.version != version){ return; }

    this->output.reserve(m_input.size());

    // Calling each command in the file
    for( auto &rcmd : m_input)
    {
        if(this->has_error()){ break; }

        cmd_func cmd;

        this->m_current_line = rcmd.m_line_idx;
        if(commands_map.find(rcmd.m_first) == commands_map.end())
        {
            if(!this->globals.references.defined_registers.includes(rcmd.m_first)){ this->set_error(vmc::GenericError(rcmd.m_line_idx, "Command \"" + rcmd.m_first + "\" is not a valid command")); continue; }
            if(commands_map.find("set") == commands_map.end()){ this->set_error(vmc::GenericError(rcmd.m_line_idx, "Call to set command from parser errored")); continue; } // Hopefully will prevent crashes if set is ever removed
            
            rcmd.m_args.add_begin(rcmd.m_first);
            cmd = commands_map.at("set");
        }
        else
        {
            cmd = commands_map.at(rcmd.m_first);
        }
        cmd(rcmd.m_args, rcmd.m_line_idx, this);
    }
    
    if(this->has_error()){ return; }

    if(this->globals.unresolved_labels.size() > 0)
    {
        for(auto& line : this->globals.unresolved_labels)
        {
            this->set_error(vmc::UnregistedLabelError(line.m_idx, line.m_data));
        }
    }
}
/*Returns the current line number*/
uint16_t Parser::get_current_line() const { return m_current_line; }
/*Adds the provided error to the errors list and sets the has_error flag.*/
void Parser::set_error(std::string error)
{
    this->m_errors.add_end(error);
    this->flags.has_error = true;
}
/*Returns the state of the parsers has_error flag.*/
bool Parser::has_error() const { return this->flags.has_error; }
/*Returns a read only reference to the errors.*/
const vmc::string_array& Parser::get_errors() const { return this->m_errors; }
/*
    Wrapper for parser->globals.references.get().
    If a reference is not found it sets the parsers error flag.
*/
std::string Parser::get_ref(const std::string& external_name)
{
    if(!this->globals.references.exists(external_name)){ this->set_error(vmc::UndefinedRegisterError(this->get_current_line(), external_name)); }
    return this->globals.references.get(external_name);
}