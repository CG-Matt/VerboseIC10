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

void ParserReferences::add(const std::string& external_name, const std::string& internal_name)
{
    PReference ref(internal_name, external_name);
    m_definitions.push_back(ref);
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
    for (auto &reg : m_valid_registers)
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



Parser::Parser(std::vector<std::string> &file_contents)
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
    globals.references.add("Self", "db");
}
void Parser::p_parse_file(std::vector<std::string> file_contents)
{
    for(unsigned int i = 0; i < file_contents.size(); i++)
    {
        std::string& line = file_contents[i];

        if(starts_with(line, "$")){ continue; } // Ignore Comments
        if(starts_with(line, "#"))
        {
            m_directives.push_back(RawDirective(split_string(line, ' '), i+1));
            continue;
        }

        m_input.push_back(RawCommand(split_string(line, ' '), i+1));
    }
}
void Parser::p_parse_directives()
{
    for(auto &rdirective : m_directives)
    {
        std::string &cur_directive = rdirective.m_directive;
        std::vector<std::string> &args = rdirective.m_arguments;

        if(cur_directive == "version"){ flags.version = std::stoi(args[0]); }
        if(cur_directive == "using")
        {
            std::string &feature = args[0];

            if(feature == "carry")
            {
                globals.references.add("carry", "r15");
                flags.available_registers--;
                flags.using_carry = true;
            }
        }
    }
}
void Parser::parse()
{
    if(flags.version != version){ return; }

    std::vector<std::string> raw_output;

    // Calling each command in the file and saving the results to a temporary buffer
    std::transform(m_input.begin(), m_input.end(), std::back_inserter(raw_output), [this](RawCommand& rcmd)
    {
        if(rcmd.m_command.size() < 1){ return (std::string)""; }
        if(commands_map.find(rcmd.m_command) == commands_map.end()){ return (std::string)vmc::GenericError(rcmd.line_idx, "Command \"" + rcmd.m_command + "\" is not a valid command"); }
        auto &cmd = commands_map.at(rcmd.m_command);
        vmc::string_array args(rcmd.m_arguements);
        return cmd(args, rcmd.line_idx, this->globals, this->flags);
    });

    std::copy_if(raw_output.begin(), raw_output.end(), std::back_inserter(this->output), [](std::string line)
    {
        return line.size() > 0;
    });
}