#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include "data_structs.hpp"

namespace vmc
{
    static const std::string _empty_str{};

    const std::string& ArgumentList::operator[](std::size_t i) const noexcept { return _data[_offset + i]; }
    
    ArgumentList::ArgumentList(const std::vector<std::string>& data)
        : _data(data)
    {}

    bool ArgumentList::hasNext(void) const noexcept
    {
        return _offset < _data.size();
    }

    const std::string& ArgumentList::peek(void) const noexcept
    {
        if(_offset >= _data.size()){ return _empty_str; }
        return _data[_offset];
    }
    
    const std::string& ArgumentList::next(void) noexcept
    {
        if(_offset >= _data.size()){ return _empty_str; }
        return _data[_offset++];
    }

    const std::string& ArgumentList::next_checked(const std::string& message)
    {
        if(_offset >= _data.size()){ throw vmc::ParserError(message); }
        return _data[_offset++];
    }

    bool ArgumentList::expect(std::string_view value) noexcept
    {
        if(_offset >= _data.size()){ return false; }
        return _data[_offset++] == value;
    }

    ParserError::ParserError(const std::string& message)
        : runtime_error(std::string("ERROR: ") + message)
    {}

    ErrorTemplate::operator std::string() const
    {
        return m_data;
    }

    GenericError::GenericError(std::string data)
    {
        m_data = "ERROR: " + data;
    }

    UnregisteredLabelError::UnregisteredLabelError(std::string label_name)
    {
        m_data = "Unregistered label \"" + label_name + "\" called";
    }

    InvalidMathOpError::InvalidMathOpError(std::string operation)
        : ParserError("Invalid math operation token \"" + operation + "\"")
    {}

    MissingValueError::MissingValueError()
        : ParserError("Missing value.")
    {}

    DuplicateReferenceError::DuplicateReferenceError(std::string reference_name)
        : ParserError("Reference with name \"" + reference_name + "\" already exists.")
    {}

    InvalidComparatorError::InvalidComparatorError(std::string comparator)
        : ParserError("Invalid comparator symbol \"" + comparator + "\"")
    {}

    UnknownIdentifier::UnknownIdentifier(std::string identifier)
        : ParserError("Unknown identifier \"" + identifier + "\".")
    {}
}

template <typename T>
Buffer<T>::Buffer(Buffer&& other) noexcept
    : data(other.data), size(other.size)
{
    other.data = nullptr;
}
