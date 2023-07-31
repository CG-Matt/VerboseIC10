#pragma once

#include <string>
#include <vector>
#include "constants.h"
#include "comp_structs.h"

class PReference
{
    public:
        std::string m_internal;
        std::string m_external;

    PReference(const std::string& internal, const std::string& external);
};

class ParserReferences
{
    public:
        std::vector<std::string> m_valid_devices = valid_devices;
        std::vector<std::string> m_available_devices = available_devices;
        std::vector<std::string> m_valid_registers = valid_registers;
        std::vector<std::string> m_valid;
        std::vector<PReference> m_definitions;

    void add(const std::string& external_name, const std::string& internal_name);
    std::string get(const std::string& external_name) const;
    std::string get_ext(const std::string& internal_name) const;
    std::string get_free() const;
};

class ParserGlobals
{
    public:
        ParserReferences references;
        std::vector<std::string> labels;
        std::vector<std::string> errors;
        ConditionalInfo conditional;

    ConditionalInfo generate_conditional_labels();
    void register_label(const std::string& label);
    void register_label(const std::vector<std::string>& label);
};

struct ParserFlags
{
    bool using_carry = false;
    bool devices_initialised = false;
    bool in_conditional = false;
    bool is_conditional_else = false;
    short avaiable_registers = 16;
    short version;
};

class Parser
{
    public:
        std::vector<std::string> output;
        ParserGlobals globals;
        ParserFlags flags;

    private:
        std::vector<RawDirective> m_directives;
        std::vector<RawCommand> m_input;

    public:

    Parser(std::vector<std::string> &file_contents);

    private:

    void p_init_globals();
    void p_parse_file(std::vector<std::string> file_contents);
    void p_parse_directives();

    public:

    void parse();
};