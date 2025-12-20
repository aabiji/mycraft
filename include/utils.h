#pragma once

#include <format>
#include <random>
#include <stdexcept>

#include <glm/glm.hpp>

enum class Direction { front, back, left, right, up, down };

class Error : public std::runtime_error
{
public:
    template <typename...T>
    Error(std::string_view fmt_str, T&&... args)
        : std::runtime_error(
            std::vformat(fmt_str, std::make_format_args(std::forward<T>(args)...))) {}

    explicit Error(std::string msg) : std::runtime_error(msg) {}
};

static unsigned int random_u32()
{
    std::random_device device;
    std::mt19937 engine(device());
    std::uniform_int_distribution<unsigned int> dist;
    return dist(engine);
}

static glm::uvec4 generate_uuidv4()
{
    glm::uvec4 value = glm::uvec4(
        random_u32(), random_u32(), random_u32(), random_u32());
    value.y = (value.y & 0xFFFF0FFF) | 0x00004000; // version 4
    value.z = (value.z & 0x3FFFFFFF) | 0x80000000; // variant
    return value;
}
