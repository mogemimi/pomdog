// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>
#include <functional>

namespace Pomdog {

class EntityID final {
public:
    EntityID() noexcept : id(0x0) {}

    EntityID(std::uint32_t sequenceNumber, std::uint32_t index) noexcept
        : id((static_cast<std::uint64_t>(sequenceNumber) << 32) | (index & 0xffffffffUL))
    {}

    std::uint32_t SequenceNumber() const noexcept {
        return id >> 32;
    }

    std::uint32_t Index() const noexcept {
        return id & 0xffffffffUL;
    }

    std::uint64_t Value() const noexcept {
        return id;
    }

    bool operator==(EntityID const& other) const noexcept {
        return id == other.id;
    }

    bool operator!=(EntityID const& other) const noexcept {
        return id != other.id;
    }

    bool operator<(EntityID const& other) const noexcept {
        return id < other.id;
    }

private:
    std::uint64_t id;
};

} // namespace Pomdog

namespace std {

template <>
struct hash<Pomdog::EntityID> {
    std::size_t operator()(const Pomdog::EntityID& key) const
    {
        return std::hash<std::uint64_t>()(key.Value());
    }
};

} // namespace std
