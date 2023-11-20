#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "constants.h"
#include "comp_structs.h"
#include "data_structs.h"

enum identifier_type : uint8_t{ reg = 0, dev, constant };

struct Identifier
{
    identifier_type m_type;
    std::string m_value;
};

class PReference
{
    public:
        std::string m_internal;
        std::string m_external;

    PReference(const std::string& internal, const std::string& external);
};

class ParserReferences
{
    private:
        uint8_t m_free_register_offset = 0;
        uint8_t m_free_register_end = valid_registers.size() - 1;

    public:
        std::unordered_map<std::string, Identifier> m_definitions;

    void add(const std::string& name, const std::string& value, identifier_type type);
    void add_end(const std::string& name, const std::string& value, identifier_type type);
    bool exists(const std::string& external_name) const;
    std::string get(const std::string& external_name) const;
    identifier_type get_type(const std::string& external_name) const;
    const std::string& get_free();
    inline const std::unordered_map<std::string, Identifier>& get_all() const { return m_definitions; }
};

class ParserGlobals
{
    public:
        ParserReferences references;
        std::vector<std::string> labels;
        std::vector<vmc::Line> unresolved_labels;
        std::vector<std::string> errors;
        ConditionalInfo conditional;

    ConditionalInfo generate_conditional_labels();
    void register_label(const std::string& label);
    void register_label(const std::vector<std::string>& labels);
    bool label_exists(const std::string& label);
};

struct ParserFlags
{
    bool has_error = false;
    bool using_carry = false;
    bool devices_initialised = false;
    bool in_conditional = false;
    bool is_conditional_else = false;
    bool in_subroutine = false;
    short available_registers = 16;
    short available_devices = 6;
    short version;
};

class Parser; // Declaring Parser class for ParserUtils

class ParserUtils
{
    private:
        Parser* m_parent;

    public:
    
    void set_parent(Parser* parent);
    Device parse_device(std::string& device);
    std::vector<std::string> parse_array(const vmc::string_array_view& view);
    std::string parse_value(const std::string& value);
};

class Parser
{
    public:
        vmc::string_array output;
        ParserGlobals globals;
        ParserFlags flags;
        ParserUtils utils;

    private:
        vmc::string_array m_errors;
        std::vector<ProgramLine> m_directives;
        std::vector<ProgramLine> m_input;
        uint16_t m_current_line;

    public:

    Parser(std::vector<vmc::Line> &file_contents);

    private:

    void p_init_globals();
    void p_parse_file(std::vector<vmc::Line>& file_contents);
    void p_parse_directives();

    public:

    void parse();
    uint16_t get_current_line() const;
    void set_error(std::string error);
    bool has_error() const;
    const vmc::string_array& get_errors() const;
    std::string get_ref(const std::string& external_name);
};