#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "data_structs.h"

namespace vmc
{
    /*Access to internal vectors begin iterator*/
    std::vector<std::string>::iterator string_array::begin(){ return m_data.begin(); }
    /*Access to internal vectors begin iterator*/
    std::vector<std::string>::const_iterator string_array::begin() const { return m_data.begin(); }
    /*Access to internal vectors end iterator*/
    std::vector<std::string>::iterator string_array::end(){ return m_data.end(); }
    /*Access to internal vectors end iterator*/
    std::vector<std::string>::const_iterator string_array::end() const { return m_data.end(); }
    /*Access to internal vectors size*/
    std::size_t string_array::size(){ return m_data.size(); }

    string_array::operator std::vector<std::string>() const { return m_data; }
    string_array::operator vmc::string_array_view(){ return vmc::string_array_view{m_data.begin(), m_data.end()}; }
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

    /*Returns the array's index offset*/
    uint32_t string_array::get_offset() const { return m_offset; }
    /*Preallocate space for the internal vector*/
    void string_array::reserve(size_t size){ m_data.reserve(size); }

    /*
        Returns the index of an element in an array.
        Returns "-1" if the element is not found. 
    */
    int32_t string_array::get_idx(const std::string& search_string) const
    {
        auto it = std::find(m_data.begin(), m_data.end(), search_string);
        if(it == m_data.end()){ return -1; }
        
        return it - m_data.begin();
    }
    /*Checks if the array contains the seach_string*/
    bool string_array::includes(const std::string& search_string) const
    {
        auto it = std::find_if(m_data.begin(), m_data.end(), [&search_string](const std::string token)
        {
            return token == search_string;
        });
        return it != m_data.end();
    }

    /*
        Removes the first element from the array and returns it.
        Returns an empty string if it reaches the end of the array.
    */
    std::string string_array::shift()
    {
        if(m_data.empty()){ return ""; }

        std::string str = m_data[0];
        m_data.erase(m_data.begin());
        return str;
    }

    /*
        Virtual Shift, returns reference to string at offset and increases offset.
        Returns refernce to an empty string if it reaches the end of the array.
    */
    std::string& string_array::v_shift()
    {
        if(m_offset >= m_data.size()){ return (std::string&)m_empty_str; }

        std::string& data = m_data[m_offset];
        m_offset++;
        return data;
    }

    /*
        Returns a string_array_view starting at the internal offset.
        As views allow direct access to the array's data they are not const.
    */
    vmc::string_array_view string_array::make_offset_view()
    {
        return vmc::string_array_view{m_data.begin() + m_offset, m_data.end()};
    }

    /*Joins all the elements in the array with the "delim" token and returns them as a string*/
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

    /*Adds a new element to the end of the array*/
    void string_array::add_end(const std::string& data)
    {
        m_data.push_back(data);
    }

    /*Adds a new element to the beginning of the array*/
    void string_array::add_begin(const std::string& data)
    {
        m_data.insert(m_data.begin(), data);
    }

    /*Removes the data from the array at the index specified*/
    void string_array::remove(uint32_t idx)
    {
        m_data.erase(m_data.begin() + idx);
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

    UnregistedLabelError::UnregistedLabelError(uint16_t line_index, std::string label_name)
    {
        m_data = "(" + std::to_string(line_index) + ") Unregistered label \"" + label_name + "\" called";
    }

    InvalidMathOpError::InvalidMathOpError(uint16_t line_index, std::string operation)
    {
        m_data = "(" + std::to_string(line_index) + ") Invalid math operation token \"" + operation + "\"";
    }

    UndefinedRegisterError::UndefinedRegisterError(uint16_t line_index, std::string register_name)
    {
        m_data = "(" + std::to_string(line_index) + ") Undefined register \"" + register_name + "\" called.";
    }

    UndefinedDeviceError::UndefinedDeviceError(uint16_t line_index, std::string device_name)
    {
        m_data = "(" + std::to_string(line_index) + ") Undefined device name \"" + device_name + "\" used.";
    }
}