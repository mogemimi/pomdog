// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>
#include <functional>

namespace Pomdog {

class EntityID {
public:
    EntityID() : id(0x0) {}

    EntityID(std::uint32_t sequenceNumber, std::uint32_t index)
        : id((static_cast<uint64_t>(sequenceNumber) << 32) | (index & 0xffffffffUL))
    {}

    std::uint32_t SequenceNumber() const {
        return id >> 32;
    }

    std::uint32_t Index() const {
        return id & 0xffffffffUL;
    }

    std::uint64_t Value() const {
        return id;
    }

    bool operator==(EntityID const& other) const {
        return id == other.id;
    }

    bool operator!=(EntityID const& other) const {
        return id != other.id;
    }

    bool operator<(EntityID const& other) const {
        return id < other.id;
    }

private:
    std::uint64_t id;
};

} // namespace Pomdog

namespace std {

template<>
struct std::hash<Pomdog::EntityID> {
    std::size_t operator()(Pomdog::EntityID const& key)
    {
        return std::hash<std::uint64_t>()(key.Value());
    }
};

} // namespace std
