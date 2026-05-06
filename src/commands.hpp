#pragma once

#include <string_view>
#include <functional>
#include "data_structs.hpp"

namespace Cmd
{
    using Function = std::function<void(vmc::ArgumentList args)>;

    /**
     * Returns true if `cmd_str` names a registered VIC10 command.
     */
    bool Exists(std::string_view cmd_str) noexcept;

    /**
     * Returns the command handler for `cmd_str`.
     * Throws `std::out_of_range` if the command does not exist.
     */
    const Function& Get(std::string_view cmd_str);
}
