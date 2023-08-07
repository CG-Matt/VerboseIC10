#pragma once

#include <string>
#include <vector>

// Verbose MIPS Compiler
namespace vmc
{
    class string_array
    {
        private:
            std::vector<std::string> m_data;
            uint32_t m_offset = 0;
            const std::string m_empty_str = "";

        public:

        std::vector<std::string>::iterator begin();
        std::vector<std::string>::iterator end();
        std::size_t size();
        
        operator std::vector<std::string>() const;
        std::string& operator[](size_t idx);

        string_array();
        string_array(const std::vector<std::string>& data);

        std::vector<std::string> out() const;

        bool contains_data();
        bool includes(const std::string& search_string) const;
        std::string shift();
        /*Virtual Shift. Returns reference to string at offset and increases offset.*/
        std::string& v_shift();
        std::string join(std::string delim);
        void add_end(const std::string& data);
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
}