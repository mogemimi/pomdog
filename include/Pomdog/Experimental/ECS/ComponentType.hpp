// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/ECS/ComponentBuffer.hpp"
#include "Pomdog/Experimental/ECS/ComponentTypeIndex.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::ECS {

class ComponentTypeBase {
public:
    virtual ~ComponentTypeBase() = default;

    [[nodiscard]] virtual std::uint8_t
    GetTypeIndex() const noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<Detail::ComponentBufferBase>
    CreateComponentBuffer() const = 0;
};

template <typename TComponent>
struct ComponentTypeDeclaration final {
    static std::uint8_t GetTypeIndex()
    {
        return Detail::ComponentTypeIndex::Index<TComponent>();
    }
};

template <typename T>
class ComponentType final : public ComponentTypeBase {
public:
    std::uint8_t GetTypeIndex() const noexcept override
    {
        return ComponentTypeDeclaration<T>::GetTypeIndex();
    }

    std::unique_ptr<Detail::ComponentBufferBase> CreateComponentBuffer() const override
    {
        return std::make_unique<Detail::ComponentBuffer<T>>();
    }
};

template <class TComponent, typename... Args>
inline std::shared_ptr<ComponentTypeBase> AddComponent(Args &&...args)
{
    return std::make_shared<ComponentType<TComponent>>(std::forward<Args>(args)...);
}

} // namespace Pomdog::ECS
