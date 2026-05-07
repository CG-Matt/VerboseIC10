#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

namespace syntax
{
    namespace devices
    {
        /**
         * Returns true if `device_name` is a valid IC10 device target.
         */
        bool isValid(std::string_view device_name);

        /**
         * Returns all valid IC10 device targets in allocation order.
         */
        const std::vector<std::string_view>& getAll(void);

        /**
         * Returns the number of valid device targets.
         */
        std::size_t count(void);
    };

    namespace registers
    {
        /**
         * Checks if the given register name is a valid one.
         */
        bool isValid(std::string_view register_name);

        /**
         * Returns all valid IC10 register targets in allocation order.
         */
        const std::vector<std::string_view>& getAll(void);

        /**
         * Returns the number of valid register targets.
         */
        std::size_t count(void);
    };

    namespace math
    {
        /**
         * Returns true if the provided string view names a maths function.
         */
        bool isFunction(std::string_view name);

        /**
         * Returns true if the provided symbol is a valid maths symbol.
         */
        bool isValidSymbol(std::string_view symbol);

        /**
         * Returns the IC10 instruction mnemonic for a maths operation symbol.
         * Throws `std::out_of_range` if `symbol` is not valid.
         */
        std::string_view toInstruction(std::string_view symbol);
    };

    namespace logic
    {
        /**
         * Returns true if the provided symbol is a valid logical opeartion.
         */
        bool isValidOperation(std::string_view operation);

        /**
         * Returns the IC10 comparator suffix for a logical operation symbol.
         * Throws `std::out_of_range` if `symbol` is not valid.
         */
        std::string_view toInstruction(std::string_view symbol);

        /**
         * Inverts the relational operator.
         */
        std::string_view invertRelation(std::string_view relation);

        /**
         * Returns true if the provided symbol is a valid logical combinator 
         */
        bool isValidCombinator(std::string_view symbol);
    };

    /**
     * Returns the current version of the syntax the parser is accepting.
     */
    std::size_t getCurrentVersion(void);

    /**
     * Returns true if `str` can successfully be parsed as a number.
     */
    bool isNumber(std::string_view str);
    
    /**
     * Returns true if `str` is equal to `true` or `false`.
     */
    bool isBoolean(std::string_view str);
    
    /**
     * Returns true if `str` is not a number.
     */
    bool isReference(std::string_view str);
}
