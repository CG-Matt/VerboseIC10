#include <cstddef>
#include "utils.hpp"

std::string JoinString(const std::vector<std::string>& data, const std::string& delim)
{
    std::string out;
    
    if(data.size() == 0) return out;

    std::size_t total_size = (data.size() - 1) * delim.size();

    for(const std::string& str : data)
        total_size += str.size();

    out.reserve(total_size);

    for(std::size_t i = 0; i < data.size(); i++)
    {
        if(i != 0) out += delim;
        out += data[i];
    }

    return out;
}

std::vector<std::string> SplitString(std::string_view str, char delim)
{
    // Output vector
    std::vector<std::string> out;

    // Keeps track of the length of the current string slice
    std::size_t length = 0;

    // Split the line into tokens split on whitespace
    while(!str.empty())
    {
        if(str.size() == length || str[length] == delim)
        {
            if(length)
            {
                out.push_back(std::string(str.data(), length));
                str.remove_prefix(length);
                length = 0;
            }
            else
                str.remove_prefix(1);
        }
        else
        {
            length++;
        }
    }

    return out;
}
