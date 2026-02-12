// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {
class Entity;
} // namespace pomdog::ecs

namespace pomdog::ecs {

/// Manages creation and destruction of entities in the ECS system.
/// Uses a pool of reusable entity IDs with version tracking to prevent stale references.
class EntityPool final {
private:
    std::vector<u64> ownership_;
    std::vector<u16> versions_;
    u32 count_ = 0;

public:
    /// Constructs an empty entity pool.
    EntityPool() noexcept;

    /// Initializes the entity pool with a given capacity.
    /// NOTE: Must be called before creating entities.
    void initialize(u32 capacity);

    /// Creates a new entity and returns its handle.
    /// NOTE: May reuse indices of previously destroyed entities.
    [[nodiscard]] Entity create() noexcept;

    /// Destroys the specified entity and invalidates its handle.
    /// NOTE: Does nothing if the entity is already destroyed or invalid.
    void destroy(Entity entity) noexcept;

    /// Destroys all entities in the pool.
    void destroyAll() noexcept;

    /// Returns true if the entity is currently alive.
    [[nodiscard]] bool exists(Entity entity) const noexcept;

    /// Returns the entity at the specified index in the pool.
    /// NOTE: Returned entity may be invalid if previously destroyed.
    [[nodiscard]] Entity at(int index) const noexcept;

    /// Returns the number of currently alive entities.
    [[nodiscard]] u32 count() const noexcept;

    /// Returns the maximum number of entities that can be managed.
    [[nodiscard]] u32 capacity() const noexcept;

    /// Iterates over all alive entities in forward order.
    void forEach(std::function<void(Entity entity)> func) const noexcept;

    /// Iterates over all alive entities in reverse order.
    void forEachReverse(std::function<void(Entity entity)> func) const noexcept;
};

} // namespace pomdog::ecs
