// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/ecs/entity.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {

Entity::Entity() noexcept = default;

Entity::Entity(u32 index, u16 version) noexcept
    : id_((static_cast<u32>(version) << 18) | index)
{
}

u16 Entity::version() const noexcept
{
    return static_cast<u16>(id_ >> 18);
}

u32 Entity::index() const noexcept
{
    constexpr u32 maxUint18 = 0x3FFFFul;
    return id_ & maxUint18;
}

u32 Entity::uint32() const noexcept
{
    return id_;
}

bool Entity::operator==(const Entity& other) const noexcept
{
    return id_ == other.id_;
}

bool Entity::operator!=(const Entity& other) const noexcept
{
    return id_ != other.id_;
}

bool Entity::operator<(const Entity& other) const noexcept
{
    return id_ < other.id_;
}

Entity::operator bool() const noexcept
{
    return id_ != u32(0ul);
}

[[nodiscard]] Entity null() noexcept
{
    return Entity{};
}

} // namespace pomdog::ecs

namespace std {

std::size_t hash<pomdog::ecs::Entity>::operator()(const pomdog::ecs::Entity& key) const noexcept
{
    return std::hash<pomdog::u32>()(key.uint32());
}

} // namespace std
