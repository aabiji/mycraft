#pragma once

#include <stdexcept>
#include <format>

enum class Direction { front, back, left, right, up, down };

class Error : public std::runtime_error
{
public:
    template <typename...T>
    explicit Error(std::format_string<T...> fmt_str, T&&... args)
        : std::runtime_error(std::format(fmt_str, std::forward<T>(args)...)) {}
};
