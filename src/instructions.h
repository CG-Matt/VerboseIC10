#pragma once

#include <string>

typedef const std::string& str;

namespace ins
{
    // Branch

    inline std::string beq(const str reg, const str value, const str label){ return "beq " + reg + " " + value + " " + label; }
    inline std::string bne(const str reg, const str value , const str label){ return "bne " + reg + " " + value + " " + label; }
    inline std::string bgt(const str value1, const str value2, const str label){ return "bgt " + value1 + " " + value2 + " " + label; }
    inline std::string blt(const str value1, const str value2, const str label){ return "blt " + value1 + " " + value2 + " " + label; }
    inline std::string beqz(const str reg, const str label){ return "beqz " + reg + " " + label; }
    inline std::string bnez(const str reg, const str label){ return "bnez " + reg + " " + label; }
    inline std::string bgtz(const str reg, const str label){ return "bgtz " + reg + " " + label; }
    inline std::string bltz(const str reg, const str label){ return "bltz " + reg + " " + label; }

    // Branch Relative

    inline std::string breq(const str reg, const str value, const str lines){ return "breq " + reg + " " + value + " " + lines; }
    inline std::string brne(const str reg, const str value, const str lines){ return "brne " + reg + " " + value + " " + lines; }
    inline std::string brgt(const str reg, const str value, const str lines){ return "brgt " + reg + " " + value + " " + lines; }
    inline std::string brlt(const str reg, const str value, const str lines){ return "brlt " + reg + " " + value + " " + lines; }
    inline std::string breqz(const str reg, const str lines){ return "breqz " + reg + " " + lines; }
    inline std::string brnez(const str reg, const str lines){ return "brnez " + reg + " " + lines; }
    inline std::string brgtz(const str reg, const str lines){ return "brgtz " + reg + " " + lines; }
    inline std::string brltz(const str reg, const str lines){ return "brltz " + reg + " " + lines; }

    // Subroutine calls
    inline std::string jal(str label){ return "jal " + label; }
    inline std::string beqal(const str reg, const str value, const str lines){ return "beqal " + reg + " " + value + " " + lines; }
    inline std::string bneal(const str reg, const str value, const str lines){ return "bneal " + reg + " " + value + " " + lines; }
    inline std::string bgtal(const str reg, const str value, const str lines){ return "bgtal " + reg + " " + value + " " + lines; }
    inline std::string bltal(const str reg, const str value, const str lines){ return "bltal " + reg + " " + value + " " + lines; }
    inline std::string beqzal(const str reg, const str lines){ return "beqzal " + reg + " " + lines; }
    inline std::string bnezal(const str reg, const str lines){ return "bnezal " + reg + " " + lines; }
    inline std::string bgtzal(const str reg, const str lines){ return "bgtzal " + reg + " " + lines; }
    inline std::string bltzal(const str reg, const str lines){ return "bltzal " + reg + " " + lines; }

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
    inline std::string math(const str operation, const str output_reg, const str var1, const str var2){ return "" + operation + " " + output_reg + " " + var1 + " " + var2; }
    inline std::string j(const str label){ return "j " + label; }
    inline std::string move(const str reg, const str value){ return "move " + reg + " " + value; }
    inline std::string jr(const str lines){ return "jr " + lines; }
    inline std::string label(const str name){ return name + ":"; }

}