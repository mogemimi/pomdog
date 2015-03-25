// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP
#define POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>
#include <functional>

namespace Pomdog {

class GameObjectID {
public:
    GameObjectID(): id(0x0) {}

    GameObjectID(std::uint32_t sequenceNumber, std::uint32_t index)
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

    bool operator==(GameObjectID const& other) const {
        return id == other.id;
    }

    bool operator!=(GameObjectID const& other) const {
        return id != other.id;
    }

    bool operator<(GameObjectID const& other) const {
        return id < other.id;
    }

private:
    std::uint64_t id;
};

}// namespace Pomdog

namespace std {

template<>
struct std::hash<Pomdog::GameObjectID> {
    std::size_t operator()(Pomdog::GameObjectID const& key)
    {
        return std::hash<std::uint64_t>()(key.Value());
    }
};

}// namespace std

#endif // !defined(POMDOG_GAMEOBJECTID_F68B7FC0_7009_433E_8F5E_153D5502766A_HPP)
