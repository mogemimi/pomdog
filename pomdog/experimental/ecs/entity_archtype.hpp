// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/ecs/component_type.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::ECS::Detail {

template <std::uint8_t MaxComponentCapacity>
class EntityArchtype final {
public:
    EntityArchtype() = default;

    explicit EntityArchtype(std::vector<std::shared_ptr<ComponentTypeBase>>&& componentTypesIn)
        : componentTypes(std::move(componentTypesIn))
    {
        for (auto& componentType : componentTypes) {
            POMDOG_ASSERT(componentType);
            const auto typeIndex = componentType->GetTypeIndex();
            componentBitMask[typeIndex] = true;
        }
    }

    EntityArchtype(const EntityArchtype&) = delete;
    EntityArchtype& operator=(const EntityArchtype&) = delete;

    EntityArchtype(EntityArchtype&&) = default;
    EntityArchtype& operator=(EntityArchtype&&) = default;

    const std::vector<std::shared_ptr<ComponentTypeBase>>& GetComponentTypes() const noexcept
    {
        return componentTypes;
    }

private:
    std::vector<std::shared_ptr<ComponentTypeBase>> componentTypes;
    std::bitset<MaxComponentCapacity> componentBitMask;
};

} // namespace Pomdog::ECS::Detail

namespace Pomdog::ECS {

using EntityArchtype = Detail::EntityArchtype<64>;

} // namespace Pomdog::ECS
