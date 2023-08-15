#include <vector>
#include <algorithm>
#include <string>
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
    conditional.pass_label = "cp" + current_index;
    conditional.fail_label = "cf" + current_index;
    conditional.end_label  = "ce" + current_index;

    register_label({ conditional.pass_label, conditional.fail_label, conditional.end_label });

    return conditional;
}
void ParserGlobals::register_label(const std::string& label)
{
    labels.push_back(label);
}
void ParserGlobals::register_label(const std::vector<std::string>& labels)
{
    for(auto& label : labels)
    {
        this->labels.push_back(label);
    }
}
bool ParserGlobals::label_exists(const std::string& label)
{
    return includes(labels, label);
}



Parser::Parser(std::vector<vmc::Line> &file_contents)
{
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
            if(!args.contains_data()){ errors.add_end(vmc::GenericError(rdirective.m_line_idx, "No version number provided")); continue; }
            flags.version = std::stoi(args[0]);
        }
        if(cur_directive == "using")
        {
            if(!args.contains_data()){ errors.add_end(vmc::GenericError(rdirective.m_line_idx, "No module name provided")); continue; }
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
        if(this->errors.size() > 0){ break; }

        cmd_func cmd;

        if(rcmd.m_first.size() < 1){ continue; }
        if(commands_map.find(rcmd.m_first) == commands_map.end())
        {
            if(!this->globals.references.defined_registers.includes(rcmd.m_first)){ this->errors.add_end(vmc::GenericError(rcmd.m_line_idx, "Command \"" + rcmd.m_first + "\" is not a valid command")); continue; }
            if(commands_map.find("set") == commands_map.end()){ this->errors.add_end(vmc::GenericError(rcmd.m_line_idx, "Call to set command from parser errored")); continue; } // Hopefully will prevent crashes if set is ever removed
            
            rcmd.m_args.add_begin(rcmd.m_first);
            cmd = commands_map.at("set");
        }
        else
        {
            cmd = commands_map.at(rcmd.m_first);
        }
        cmd(rcmd.m_args, rcmd.m_line_idx, this);
    }
}