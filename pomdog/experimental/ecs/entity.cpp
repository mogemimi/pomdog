// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/ecs/entity.h"

namespace pomdog::ecs {

const Entity Entity::Null = Entity{};

} // namespace pomdog::ecs

namespace std {

std::size_t hash<pomdog::ecs::Entity>::operator()(const pomdog::ecs::Entity& key) const noexcept
{
    return std::hash<std::uint64_t>()(key.GetUInt64Value());
}

} // namespace std
