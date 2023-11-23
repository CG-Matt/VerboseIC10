#pragma once

#include <string>

typedef const std::string& str;

namespace ins
{
    // Branch

    inline std::string branch(const str comp, const str reg, const str value, const str label){ return "b" + comp + " " + reg + " " + value + " " + label; }
    inline std::string branch_z(const str comp, const str reg, const str label){ return "b" + comp + "z " + reg + " " + label; }

    // Branch Relative

    inline std::string branch_relative(const str comp, const str reg, const str value, const str lines){ return "br" + comp + " " + reg + " " + value + " " + lines; }
    inline std::string branch_relative_z(const str comp, const str reg, const str lines){ return "br" + comp + "z " + reg + " " + lines; }

    // Subroutine calls
    
    inline std::string jal(str label){ return "jal " + label; }
    inline std::string sub_call(const str comp, const str reg, const str value, const str label){ return "b" + comp + "al " + reg + " " + value + " " + label; }
    inline std::string sub_call_z(const str comp, const str reg, const str label){ return "b" + comp + "zal " + reg + " " + label; }

    // Stack Managment

    inline std::string push(const str value){ return "push " + value; }
    inline std::string pop(const str reg){ return "pop " + reg; }
    inline std::string peek(const str reg){ return "peek " + reg; }

    // Other

    inline std::string alias(const str name, const str deviceOrRegister){ return "alias " + name + " " + deviceOrRegister; }
    inline std::string l(const str reg, const str device, const str variable){ return "l " + reg + " " + device + " " + variable; }
    inline std::string s(const str reg, const str variable, const str value){ return "s " + reg + " " + variable + " " + value; }
    inline std::string sb(const str reg, const str variable, const str value){ return "sb " + reg + " " + variable + " " + value; }
    inline std::string sleep(const str time){ return "sleep " + time; }
    inline std::string yld(){ return "yield"; }
    inline std::string math(const str operation, const str output_reg, const str var1, const str var2){ return operation + " " + output_reg + " " + var1 + " " + var2; }
    inline std::string math_func(const str operation, const str output_reg, const str var){ return operation + " " + output_reg + " " + var; }
    inline std::string j(const str label){ return "j " + label; }
    inline std::string move(const str reg, const str value){ return "move " + reg + " " + value; }
    inline std::string jr(const str lines){ return "jr " + lines; }
    inline std::string label(const str name){ return name + ":"; }

}