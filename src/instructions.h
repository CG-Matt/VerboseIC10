#pragma once

#include <string>

namespace ins
{
    // Branch

    std::string beq(const std::string& reg, const std::string& value, const std::string& label);
    std::string bne(const std::string& reg, const std::string& value , const std::string& label);
    std::string bgt(const std::string& value1, const std::string& value2, const std::string& label);
    std::string blt(const std::string& value1, const std::string& value2, const std::string& label);
    std::string beqz(const std::string& reg, const std::string& label);
    std::string bnez(const std::string& reg, const std::string& label);
    std::string bgtz(const std::string& reg, const std::string& label);
    std::string bltz(const std::string& reg, const std::string& label);

    // Branch Relative

    std::string breq(const std::string& reg, const std::string& value, const std::string& lines);
    std::string brne(const std::string& reg, const std::string& value, const std::string& lines);
    std::string brgt(const std::string& reg, const std::string& value, const std::string& lines);
    std::string brlt(const std::string& reg, const std::string& value, const std::string& lines);
    std::string breqz(const std::string& reg, const std::string& lines);
    std::string brnez(const std::string& reg, const std::string& lines);
    std::string brgtz(const std::string& reg, const std::string& lines);
    std::string brltz(const std::string& reg, const std::string& lines);

    // Stack Managment

    std::string push(const std::string& value);
    std::string pop(const std::string& reg);
    std::string peek(const std::string& reg);

    // Other

    std::string alias(const std::string& name, const std::string& deviceOrRegister);
    std::string l(const std::string& reg, const std::string& device, const std::string& variable);
    std::string s(const std::string& reg, const std::string& variable, const std::string& value);
    std::string sb(const std::string& reg, const std::string& variable, const std::string& value);
    std::string sleep(const std::string& time);
    std::string yld();
    std::string math(const std::string& operation, const std::string& output_reg, const std::string& var1, const std::string& var2);
    std::string j(const std::string& label);
    std::string move(const std::string& reg, const std::string& value);
    std::string jr(const std::string& lines);
    std::string label(const std::string& name);

}