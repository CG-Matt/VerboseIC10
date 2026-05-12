#include <unordered_set>
#include <unordered_map>
#include "syntax.hpp"

namespace syntax
{
    static const std::size_t _version = 5;

    namespace devices
    {
        static const std::vector<std::string_view> _available_devices {
            "d0", "d1", "d2", "d3", "d4", "d5"
        };
        
        static const std::unordered_set<std::string_view> _valid_devices {
            "d0", "d1", "d2", "d3", "d4", "d5", "db"
        };
    }

    namespace registers
    {
        static const std::vector<std::string_view> _valid_registers {
            "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
        };
    }

    namespace math
    {
        static const std::unordered_set<std::string_view> _functions {
            "sin", "cos", "tan", "asin", "acos", "atan", "abs", "ceil", "floor", "exp", "log", "sqrt", "round", "trunc"
        };

        static const std::unordered_map<std::string_view, std::string_view> _operations {
            { "+", "add" },
            { "-", "sub" },
            { "*", "mul" },
            { "/", "div" },
            { "%", "mod" }
        };
    }

    namespace logic
    {
        static const std::unordered_map<std::string_view, std::string_view> _comparators {
            {"==", "eq"},
            {"!=", "ne"},
            {">" , "gt"},
            {">=", "ge"},
            {"<" , "lt"},
            {"<=", "le"} 
        };

        static const std::unordered_map<std::string_view, std::string_view> _invert_comparator {
            { "==", "!=" },
            { "!=", "==" },
            { ">" , "<"  },
            { ">=", "<=" },
            { "<" , ">"  },
            { "<=", ">=" }
        };

        static const std::unordered_set<std::string_view> _combinators{ "and", "or" };
    }
}

namespace syntax::devices
{
    bool isValid(std::string_view device_name) { return _valid_devices.find(device_name) != _valid_devices.end(); }
    const std::vector<std::string_view>& getAll(void) { return _available_devices; }
    std::size_t count(void) { return _available_devices.size(); }
}

namespace syntax::registers
{
    bool isValid(std::string_view register_name)
    {
        for(std::string_view valid_register_name : _valid_registers)
            if(valid_register_name == register_name) return true;
        return false;
    }
    const std::vector<std::string_view>& getAll(void) { return _valid_registers; }
    std::size_t count(void) { return _valid_registers.size(); }
}

namespace syntax::math
{
    bool isFunction(std::string_view name) { return _functions.find(name) != _functions.end(); }
    bool isValidSymbol(std::string_view symbol) { return _operations.find(symbol) != _operations.end(); }
    std::string_view toInstruction(std::string_view symbol) { return _operations.at(symbol); }
}

namespace syntax::logic
{
    bool isValidOperation(std::string_view operation) { return _comparators.find(operation) != _comparators.end(); }
    std::string_view toInstruction(std::string_view symbol) { return _comparators.at(symbol); }
    std::string_view invertRelation(std::string_view relation) { return _invert_comparator.at(relation); }
    bool isValidCombinator(std::string_view symbol) { return _combinators.find(symbol) != _combinators.end(); }
}

std::size_t syntax::getCurrentVersion(void){ return _version; }

bool syntax::isNumber(std::string_view str)
{
    if(str.empty()) return false;
    if(str.front() == '-') str.remove_prefix(1); 
    if(str.find_first_not_of("0123456789") == std::string_view::npos) return true;
    if(str.find_first_not_of("0123456789.") != std::string_view::npos) return false;
    if(!(str.size() > 1)) return false;
    if(str.front() == '.') return false;
    
    unsigned int points = 0;
    for(auto &ch : str)
    {
        if(ch == '.') points++;
    }
    if(points > 1) return false;

    return true;
}

bool syntax::isBoolean(std::string_view variable){ return variable == "true" || variable == "false"; }

bool syntax::isReference(std::string_view variable){ return !syntax::isNumber(variable); }
