#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "data_structs.h"

namespace vmc
{
    std::vector<std::string>::iterator string_array::begin(){ return m_data.begin(); }
    std::vector<std::string>::iterator string_array::end(){ return m_data.end(); }
    std::size_t string_array::size(){ return m_data.size(); }

    string_array::operator std::vector<std::string>() const { return m_data; }
    std::string& string_array::operator[](size_t idx){ return m_data[idx]; }
    vmc::string_array& string_array::operator=(const std::vector<std::string>& data)
    {
        m_data = data;
        return *this;
    }

    string_array::string_array(){}
    string_array::string_array(const std::vector<std::string>& data)
    {
        m_data = data;
    }
    std::vector<std::string> string_array::out() const
    {
        return m_data;
    }

    bool string_array::contains_data(){ return m_data.size() > 0; }
    bool string_array::includes(const std::string& search_string) const
    {
        auto it = std::find_if(m_data.begin(), m_data.end(), [&search_string](const std::string token)
        {
            return token == search_string;
        });
        return it != m_data.end();
    }

    std::string string_array::shift()
    {
        if(m_data.size() < 1){ return ""; }

        std::string str = m_data[0];
        m_data.erase(m_data.begin());
        return str;
    }

    std::string& string_array::v_shift()
    {
        if(m_offset >= m_data.size()){ return (std::string&)m_empty_str; }

        std::string& data = m_data[m_offset];
        m_offset++;
        return data;
    }

    std::string string_array::join(std::string delim)
    {
        std::stringstream ss;
        for(uint16_t i = 0; i < m_data.size(); i++)
        {
            if(i != 0){ ss << delim; }
            ss << m_data[i];
        }

        return ss.str();
    }

    void string_array::add_end(const std::string& data)
    {
        m_data.push_back(data);
    }

    void string_array::add_begin(const std::string& data)
    {
        m_data.insert(m_data.begin(), data);
    }



    ErrorTemplate::operator std::string() const
    {
        return m_data;
    }

    GenericError::GenericError(uint16_t line_index, std::string data)
    {
        m_data = "(" + std::to_string(line_index) + ") ERROR: " + data;
    }

    InsufficientArgsError::InsufficientArgsError(uint16_t line_index, uint16_t recieved, uint16_t expected)
    {
        m_data = "(" + std::to_string(line_index) + ") Insufficient arguments provided. Got " + std::to_string(recieved) + ", expected " + std::to_string(expected) + ".";
    }
}