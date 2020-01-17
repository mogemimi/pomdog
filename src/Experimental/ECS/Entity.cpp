// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/Entity.hpp"

namespace Pomdog::ECS {

const Entity Entity::Null = Entity{};

} // namespace Pomdog::ECS

namespace std {

std::size_t hash<Pomdog::ECS::Entity>::operator()(const Pomdog::ECS::Entity& key) const noexcept
{
    return std::hash<std::uint64_t>()(key.GetUInt64Value());
}

} // namespace std
