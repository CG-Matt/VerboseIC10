#include <string>

namespace ins
{

// Branch

std::string beq(const std::string& reg, const std::string& value, const std::string& label){ return "beq " + reg + " " + value + " " + label; }
std::string bne(const std::string& reg, const std::string& value , const std::string& label){ return "bne " + reg + " " + value + " " + label; }
std::string bgt(const std::string& value1, const std::string& value2, const std::string& label){ return "bgt " + value1 + " " + value2 + " " + label; }
std::string blt(const std::string& value1, const std::string& value2, const std::string& label){ return "blt " + value1 + " " + value2 + " " + label; }
std::string beqz(const std::string& reg, const std::string& label){ return "beqz " + reg + " " + label; }
std::string bnez(const std::string& reg, const std::string& label){ return "bnez " + reg + " " + label; }
std::string bgtz(const std::string& reg, const std::string& label){ return "bgtz " + reg + " " + label; }
std::string bltz(const std::string& reg, const std::string& label){ return "bltz " + reg + " " + label; }

// Branch Relative

std::string breq(const std::string& reg, const std::string& value, const std::string& lines){ return "breq " + reg + " " + value + " " + lines; }
std::string brne(const std::string& reg, const std::string& value, const std::string& lines){ return "brne " + reg + " " + value + " " + lines; }
std::string brgt(const std::string& reg, const std::string& value, const std::string& lines){ return "brgt " + reg + " " + value + " " + lines; }
std::string brlt(const std::string& reg, const std::string& value, const std::string& lines){ return "brlt " + reg + " " + value + " " + lines; }
std::string breqz(const std::string& reg, const std::string& lines){ return "breqz " + reg + " " + lines; }
std::string brnez(const std::string& reg, const std::string& lines){ return "brnez " + reg + " " + lines; }
std::string brgtz(const std::string& reg, const std::string& lines){ return "brgtz " + reg + " " + lines; }
std::string brltz(const std::string& reg, const std::string& lines){ return "brltz " + reg + " " + lines; }

// Stack Managment

std::string push(const std::string& value){ return "push " + value; }
std::string pop(const std::string& reg){ return "pop " + reg; }
std::string peek(const std::string& reg){ return "peek " + reg; }

// Other

std::string alias(const std::string& name, const std::string& deviceOrRegister){ return "alias " + name + " " + deviceOrRegister; }
std::string l(const std::string& reg, const std::string& device, const std::string& variable){ return "l " + reg + " " + device + " " + variable; }
std::string s(const std::string& reg, const std::string& variable, const std::string& value){ return "s " + reg + " " + variable + " " + value; }
std::string sb(const std::string& reg, const std::string& variable, const std::string& value){ return "sb " + reg + " " + variable + " " + value; }
std::string sleep(const std::string& time){ return "sleep " + time; }
std::string yld(){ return "yield"; }
std::string math(const std::string& operation, const std::string& output_reg, const std::string& var1, const std::string& var2){ return "" + operation + " " + output_reg + " " + var1 + " " + var2; }
std::string j(const std::string& label){ return "j " + label; }
std::string move(const std::string& reg, const std::string& value){ return "move " + reg + " " + value; }
std::string jr(const std::string& lines){ return "jr " + lines; }
std::string label(const std::string& name){ return name + ":"; }

}