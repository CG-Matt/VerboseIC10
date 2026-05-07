#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>

// Verbose MIPS Compiler
namespace vmc
{
    /**
     * String data paired with a source line number.
     */
    struct IndexedStr
    {
        std::size_t line_number;
        std::string str;
    };

    class ArgumentList
    {
        public:

        /**
         * Wraps a command argument vector without taking ownership of it.
         */
        ArgumentList(const std::vector<std::string>& data);
        ArgumentList(const ArgumentList&) = default;

        /**
         * Returns the number of unconsumed arguments.
         */
        inline std::size_t size() const noexcept { return _data.size() - _offset; };

        /**
         * Returns true if there are no unconsumed arguments.
         */
        inline bool isEmpty() const noexcept { return size() == 0; }
        
        /**
         * Indexes into the unconsumed argument list without advancing it.
         */
        const std::string& operator[](std::size_t i) const noexcept;

        /**
         * Returns true if at least one unconsumed argument is present.
         */
        bool hasNext(void) const noexcept;

        /**
         * Returns the next argument without consuming it.
         * Returns a reference to an empty string if no arguments remain.
        */
        const std::string& peek(void) const noexcept;

        /**
         * Consumes and returns the next argument.
         * Returns a reference to an empty string if no arguments remain.
        */
        const std::string& next(void) noexcept;

        /**
         * Consumes and returns the next argument.
         * Throws `vmc::ParserError` with `message` if no arguments remain.
         */
        const std::string& next_checked(const std::string& message);
        
        /**
         * Consumes the next argument in the list and checks if it matches `value`.
         * Returns false if no arguments remain or the next argument differs.
         */
        bool expect(std::string_view value) noexcept;

        private:
            const std::vector<std::string>& _data;
            std::size_t _offset = 0;
    };

    class ParserError : public std::runtime_error
    {
        public:
        /**
         * Constructs a parser error and prefixes `message` with `ERROR: `.
         */
        explicit ParserError(const std::string& message);
    };

    class InvalidMathOpError : public ParserError
    {
        public:
            /**
             * Reports an unsupported maths assignment or operation token.
             */
            InvalidMathOpError(std::string operation);
    };

    class MissingValueError : public ParserError
    {
        public:
            /**
             * Reports a command argument that requires a value but reached the end of the line.
             */
            MissingValueError();
    };

    class DuplicateReferenceError : public ParserError
    {
        public:
            /**
             * Reports a user-facing identifier that has already been registered.
             */
            DuplicateReferenceError(std::string reference_name);
    };

    class InvalidComparatorError : public ParserError
    {
        public:
            /**
             * Reports an unsupported comparison operator in a conditional expression.
             */
            InvalidComparatorError(std::string comparator);
    };
    
    class UnknownIdentifier : public ParserError
    {
        public:
            /**
             * Reports an identifier lookup that could not be resolved.
             */
            UnknownIdentifier(std::string identifier);
    };

    /**
     * Legacy error formatter for parser-level errors that are not thrown.
     */
    class ErrorTemplate
    {
        protected:
            std::string m_data;

        public:    
            operator std::string() const;
    };        

    class GenericError : public ErrorTemplate
    {
        public:
            /**
             * Prefixes arbitrary parser-level text with `ERROR: `.
             */
            GenericError(std::string data);
    };        

    class UnregisteredLabelError : public ErrorTemplate
    {
        public:
            /**
             * Reports a label reference that was never resolved by a label declaration.
             */
            UnregisteredLabelError(std::string label_name);
    };        
}

template <typename T>
class Buffer
{
    public:
        T* data;
        const size_t size;

    /**
     * Allocates a raw buffer with `size` elements.
     */
    Buffer(size_t size) : size(size)
    {
        this->data = new T[this->size];
    }

    Buffer(const Buffer&) = delete;

    /**
     * Transfers ownership of another buffer's allocation.
     */
    Buffer(Buffer&& other) noexcept;

    /**
     * Returns the element at `i`.
     */
    const T& operator[](std::size_t i) const { return data[i]; }

    ~Buffer()
    {
        if(data) delete[] this->data;
    }
};
