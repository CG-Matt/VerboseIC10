#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "comp_structs.hpp"
#include "data_structs.hpp"

#include <unordered_map>

/**
 * Mutable parser state that affects how later source lines are compiled.
 */
struct ParserFlags
{
    /** Enables commands that depend on the reserved carry register. */
    bool using_carry = false;

    /** Set once wildcard device initialisation has claimed device targets. */
    bool devices_initialised = false;

    /** True while parsing lines inside an `if` block. */
    bool in_conditional = false;

    /** True after an `else` branch has been emitted for the current condition. */
    bool is_conditional_else = false;

    /** True while parsing lines inside a subroutine block. */
    bool in_subroutine = false;

    /** True while parsing lines inside a loop block. */
    bool in_loop = false;
};

namespace Parser::Utilities
{
    /**
     * Parses a device expression such as `device.Variable`, `*hash.Variable`, or `@device`.
     * Sets a parser error or throws `vmc::ParserError` if the expression cannot be resolved.
     */
    Device ParseDevice(std::string_view device);
    
    /**
     * Parses bracketed argument syntax such as `[a, b, c]` into individual entries.
     * Sets a parser error if the argument stream is not a valid array.
     */
    std::vector<std::string> ParseArray(vmc::ArgumentList arguments);
    
    /**
     * Resolves a literal, boolean, constant, register, or device reference into an IC10 value.
     * Throws `vmc::ParserError` if a non-literal reference is unknown.
     */
    std::string ParseValue(const std::string& value);
    
    /**
     * Converts `true` to `1` and `false` to `0`.
     */
    std::string ParseBoolean(std::string_view boolean);
};

namespace Parser
{
    /** IC10 output lines emitted by successful command handlers. */
    inline std::vector<std::string> output;

    /** Current parser flags shared by command handlers. */
    inline ParserFlags flags;

    /**
     * Initialises parser state before parsing a file.
     */
    void init(void);

    /**
     * Parses source text and populates `Parser::output` or `Parser::errors`.
     */
    void parse(const Buffer<char>& file_contents);

    /**
     * Returns true if any parser errors have been recorded.
     */
    bool hasError(void);

    /**
     * Records an error at the current parser line.
     */
    void setError(std::string error);

    /**
     * Records an error at a specific source line.
     */
    void setError(std::size_t, std::string error);

    /**
     * Returns all recorded parser errors.
     */
    const std::vector<std::string>& getErrors();

    /**
     * Get the line number of the line the parser is currently working on.
     */
    std::size_t getCurrentLine(void);

    namespace ident
    {
        /**
         * Returns true if an identifier with `name` is registered.
         */
        bool exists(std::string_view name);

        /**
         * Returns the type of the registered identifier `name`.
         * Throws `vmc::ParserError` if `name` is unknown.
         */
        Identifier::Type getType(std::string_view name);

        /**
         * Returns the IC10 target bound to identifier `name`.
         * Throws `vmc::ParserError` if `name` is unknown.
         */
        const std::string& getTarget(std::string_view name);

        /**
         * Reserves and returns the next available IC10 register target.
         */
        std::string getNextFreeRegister(void);

        /**
         * Returns a read-only reference to the internal identifier output bindings map.
         */
        const std::unordered_map<std::string, Identifier>& getAll(void);
    }

    /**
     * Register a new identifier internally with the parser.
     * Names will be resolved to targets at compile time.
     */
    void registerIdentifier(Identifier::Type type, std::string_view target, std::string_view name);

    namespace label
    {
        /**
         * Returns `true` if a label is registered with the parser, `false` otherwise.
         */
        bool exists(std::string_view label);

        /**
         * Records that `label` has been referenced and must be defined before parsing ends.
         */
        void expect(std::string_view label);

        /**
         * Generates and registers a unique set of labels for one conditional block.
         */
        ConditionalInfo generateConditionals(void);

        /**
         * Returns the labels most recently generated for the active conditional block.
         */
        const ConditionalInfo& getCurrentConditionals(void);

        /**
         * Generates and registers a unique label for a loop block.
         */
        const std::string& generateLoopLabel(void);

        /**
         * Returns the label most recently generate for the active loop block.
         */
        const std::string& getCurrentLoopLabel(void);
    }

    /**
     * Register a new label internally with the parser.
     * This will be resolved at compile time.
     */
    void registerLabel(std::string_view label);

    namespace device
    {
        /**
         * Returns true if there is at least one device target available.
         */
        bool available(void);

        /**
         * Reserves `target` so it will not be returned by `next`.
         * Throws `vmc::ParserError` if the target is invalid or unavailable.
         */
        void take(std::string_view target);

        /**
         * Reserves and returns the next available device target.
         */
        std::string_view next(void);
    }

    namespace reg
    {
        /**
         * Returns true if there is at least one register target available.
         */
        bool available(void);

        /**
         * Reserves `target` so it will not be returned by `next`.
         * Throws `vmc::ParserError` if the target is invalid or unavailable.
         */
        void take(std::string_view target);

        /**
         * Reserves and returns the next available register target.
         */
        std::string_view next(void);
    }
};
