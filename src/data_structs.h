#pragma once

#include <string>
#include <vector>

// Verbose MIPS Compiler
namespace vmc
{
    struct Line
    {
        uint16_t m_idx;
        std::string m_data;
    };

    struct string_array_view
    {
        std::vector<std::string>::iterator start;
        std::vector<std::string>::iterator end;
    };

    class string_array
    {
        private:
            std::vector<std::string> m_data;
            uint32_t m_offset = 0;
            const std::string m_empty_str = "";

        public:

        std::vector<std::string>::iterator begin();
        std::vector<std::string>::const_iterator begin() const;
        std::vector<std::string>::iterator end();
        std::vector<std::string>::const_iterator end() const;
        std::size_t size();
        inline bool empty() const { return m_data.empty(); }
        
        operator std::vector<std::string>() const;
        operator vmc::string_array_view();
        std::string& operator[](size_t idx);
        vmc::string_array& operator=(const std::vector<std::string>& data);

        string_array();
        string_array(const std::vector<std::string>& data);

        uint32_t get_offset() const;

        void reserve(size_t size);

        int32_t get_idx(const std::string& search_string) const;
        bool includes(const std::string& search_string) const;
        std::string shift();
        std::string& v_shift();
        vmc::string_array_view make_offset_view();
        std::string join(std::string delim);
        void add_end(const std::string& data);
        void add_begin(const std::string& data);
        void remove(uint32_t idx);
    };

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
            GenericError(uint16_t line_index, std::string data);
    };

    class InsufficientArgsError : public ErrorTemplate
    {
        public:
            InsufficientArgsError(uint16_t line_index, uint16_t recieved, uint16_t expected);
    };

    class UnregistedLabelError : public ErrorTemplate
    {
        public:
            UnregistedLabelError(uint16_t line_index, std::string label_name);
    };

    class InvalidMathOpError : public ErrorTemplate
    {
        public:
            InvalidMathOpError(uint16_t line_index, std::string operation);
    };

    class UndefinedRegisterError : public ErrorTemplate
    {
        public:
            UndefinedRegisterError(uint16_t line_index, std::string register_name);
    };

    class UndefinedDeviceError : public ErrorTemplate
    {
        public:
            UndefinedDeviceError(uint16_t line_index, std::string device_name);
    };
}