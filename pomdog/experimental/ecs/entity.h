// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {

class Entity final {
public:
    constexpr Entity() noexcept
        : id(0)
    {
    }

    constexpr Entity(std::uint32_t sequenceNumber, std::uint32_t index) noexcept
        : id((static_cast<std::uint64_t>(sequenceNumber) << 32) | (index & 0xffffffffUL))
    {
    }

    [[nodiscard]] std::uint32_t GetVersion() const noexcept
    {
        return id >> 32;
    }

    [[nodiscard]] std::uint32_t GetIndex() const noexcept
    {
        return id & 0xffffffffUL;
    }

    [[nodiscard]] std::uint64_t GetUInt64Value() const noexcept
    {
        return id;
    }

    [[nodiscard]] bool operator==(const Entity& other) const noexcept
    {
        return id == other.id;
    }

    [[nodiscard]] bool operator!=(const Entity& other) const noexcept
    {
        return id != other.id;
    }

    [[nodiscard]] bool operator<(const Entity& other) const noexcept
    {
        return id < other.id;
    }

    [[nodiscard]] operator bool() const noexcept
    {
        return id != 0;
    }

    static const Entity Null;

private:
    std::uint64_t id;
};

} // namespace pomdog::ecs

namespace std {

template <>
struct hash<pomdog::ecs::Entity> {
    std::size_t operator()(const pomdog::ecs::Entity& key) const noexcept;
};

} // namespace std
