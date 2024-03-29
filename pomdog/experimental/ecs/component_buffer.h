// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs::detail {

class ComponentBufferBase {
public:
    virtual ~ComponentBufferBase();

    [[nodiscard]] virtual std::size_t GetSize() const noexcept = 0;

    virtual void Resize(std::size_t size) = 0;

    virtual void Reset(std::uint32_t index) = 0;
};

template <typename TComponent>
class ComponentBuffer final : public ComponentBufferBase {
public:
    std::size_t GetSize() const noexcept override
    {
        return components.size();
    }

    void Resize(std::size_t size) override
    {
        components.resize(size);
    }

    void Reset(std::uint32_t index) override
    {
        POMDOG_ASSERT(index < components.size());
        components[index] = TComponent{};
    }

    [[nodiscard]] TComponent* GetComponent(std::uint32_t index)
    {
        POMDOG_ASSERT(index < components.size());
        return &components[index];
    }

private:
    std::vector<TComponent> components;
};

} // namespace pomdog::ecs::detail
