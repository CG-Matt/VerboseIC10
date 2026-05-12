#pragma once

#include <string>

namespace ins
{
    using str = const std::string&;

    /**
     * Helpers that format individual IC10 instruction lines.
     */

    // Branch

    inline std::string branch(str comp, str reg, str value, str label){ return "b" + comp + " " + reg + " " + value + " " + label; }
    inline std::string branch_z(str comp, str reg, str label){ return "b" + comp + "z " + reg + " " + label; }

    // Branch Relative

    inline std::string branch_relative(str comp, str reg, str value, str lines){ return "br" + comp + " " + reg + " " + value + " " + lines; }
    inline std::string branch_relative_z(str comp, str reg, str lines){ return "br" + comp + "z " + reg + " " + lines; }

    // Subroutine calls
    
    inline std::string jal(str label){ return "jal " + label; }
    inline std::string sub_call(str comp, str reg, str value, str label){ return "b" + comp + "al " + reg + " " + value + " " + label; }
    inline std::string sub_call_z(str comp, str reg, str label){ return "b" + comp + "zal " + reg + " " + label; }

    // Stack Management

    inline std::string push(str value){ return "push " + value; }
    inline std::string pop(str reg){ return "pop " + reg; }
    inline std::string peek(str reg){ return "peek " + reg; }

    // Other

    inline std::string alias(str name, str deviceOrRegister){ return "alias " + name + " " + deviceOrRegister; }
    inline std::string l(str reg, str device, str variable){ return "l " + reg + " " + device + " " + variable; }
    inline std::string s(str reg, str variable, str value){ return "s " + reg + " " + variable + " " + value; }
    inline std::string sb(str reg, str variable, str value){ return "sb " + reg + " " + variable + " " + value; }
    inline std::string sleep(str time){ return "sleep " + time; }
    inline std::string yld(){ return "yield"; }
    inline std::string math(str operation, str output_reg, str var1, str var2){ return operation + " " + output_reg + " " + var1 + " " + var2; }
    inline std::string math_func(str operation, str output_reg, str var){ return operation + " " + output_reg + " " + var; }
    inline std::string j(str label){ return "j " + label; }
    inline std::string move(str reg, str value){ return "move " + reg + " " + value; }
    inline std::string jr(str lines){ return "jr " + lines; }
    inline std::string label(str name){ return name + ":"; }

}
