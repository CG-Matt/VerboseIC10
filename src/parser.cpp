#include <vector>
#include <algorithm>
#include <string>
#include "commands.h"
#include "parser.h"
#include "constants.h"
#include "utils.h"

static const short version = 4;

PReference::PReference(std::string internal, std::string external)
{
    m_internal = internal;
    m_external = external;
}

void ParserReferences::add(std::string external_name, std::string internal_name)
{
    PReference ref(internal_name, external_name);
    m_definitions.push_back(ref);
}
std::string ParserReferences::get(std::string external_name) const
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
std::string ParserReferences::get_ext(std::string internal_name) const
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
void ParserGlobals::register_label(std::string label)
{
    labels.push_back(label);
}
void ParserGlobals::register_label(std::vector<std::string> label)
{
    int size = label.size();
    for(unsigned int i = 0; i < size; i++)
    {
        labels.push_back(label[i]);
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
    std::vector<std::string> raw_directive_lines;
    std::vector<std::string> raw_command_lines;

    std::copy_if(file_contents.begin(), file_contents.end(), std::back_inserter(raw_directive_lines), [](std::string line)
    {
        return line.rfind("#", 0) == 0; // Starts with '#'
    });
    std::copy_if(file_contents.begin(), file_contents.end(), std::back_inserter(raw_command_lines), [](std::string line)
    {
        return line.rfind("#", 0) != 0 && line.rfind("$", 0) != 0;
    });

    std::vector<std::vector<std::string>> directive_lines;
    std::vector<std::vector<std::string>> command_lines;

    // Array of directive lines
    std::transform(raw_directive_lines.begin(), raw_directive_lines.end(), std::back_inserter(directive_lines), [](const std::string line)
    {
        return split_string(line, ' ');
    });
    // Array of command lines
    std::transform(raw_command_lines.begin(), raw_command_lines.end(), std::back_inserter(command_lines), [](const std::string line)
    {
        return split_string(line, ' ');
    });

    std::vector<RawDirective> raw_directives;
    std::vector<RawCommand> raw_commands;

    // Convert to raw directives
    std::transform(directive_lines.begin(), directive_lines.end(), std::back_inserter(raw_directives), [](const std::vector<std::string> line)
    {
        RawDirective pdirective(line);
        return line;
    });

    // Convert to raw commands
    std::transform(command_lines.begin(), command_lines.end(), std::back_inserter(raw_commands), [](const std::vector<std::string> line)
    {
        RawCommand pcommand(line);
        return line;
    });

    // Set m_directives to raw_directives
    m_directives = raw_directives;
    // Set m_input to raw_commands
    m_input = raw_commands;
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
                flags.avaiable_registers--;
                flags.using_carry = true;
            }
        }
    }
}
void Parser::parse()
{
    if(flags.version != version){ return; }

    std::vector<std::string> raw_output;
    ParserGlobals& parse_globals = this->globals;
    ParserFlags& parse_flags = this->flags;

    std::transform(m_input.begin(), m_input.end(), std::back_inserter(raw_output), [&parse_globals, &parse_flags](RawCommand& rcmd)
    {
        if(rcmd.m_command.size() < 1){ return (std::string)""; }
        if(commands_map.find(rcmd.m_command) == commands_map.end()){ return "ERROR: Command \"" + rcmd.m_command + "\" is not a valid command"; }
        auto &cmd = commands_map.at(rcmd.m_command);
        return cmd(rcmd.m_arguements, parse_globals, parse_flags);
    });

    std::copy_if(raw_output.begin(), raw_output.end(), std::back_inserter(this->output), [](std::string line)
    {
        return line.size() > 0;
    });
}