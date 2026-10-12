// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/ecs/entity_lru_queue.h"
#include "pomdog/ecs/entity_pool.h"
#include "pomdog/experimental/devgui/key.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

/// Stores one 64-bit value per widget that persists across frames, such as
/// whether a tree node is open or how far a region is scrolled.
///
/// The storage holds at most `capacity` values, so its memory stays bounded no
/// matter how many widgets a UI creates over time. When it is full, storing a
/// new value evicts the value that was least recently read or written. Slots
/// come from an `ecs::EntityPool` and their recency is kept in an
/// `ecs::EntityLRUQueue`; an open-addressing table maps widget IDs to slots.
/// Lookups, stores, and evictions take constant time on average.
class WidgetStateStorage final {
public:
    WidgetStateStorage() noexcept;

    /// Allocates room for capacity values and removes any stored values.
    void
    initialize(u32 capacity);

    /// Returns the value stored for id, or nullopt when none is stored.
    ///
    /// A successful lookup marks the value as recently used.
    [[nodiscard]] std::optional<u64>
    find(WidgetID id) noexcept;

    /// Stores value for id and marks it as recently used.
    void
    store(WidgetID id, u64 value) noexcept;

    /// Removes the value stored for id, if any.
    void
    remove(WidgetID id) noexcept;

    /// Removes all values.
    void
    clear() noexcept;

    /// Returns the number of stored values.
    [[nodiscard]] u32
    count() const noexcept;

    /// Returns the maximum number of stored values.
    [[nodiscard]] u32
    capacity() const noexcept;

private:
    [[nodiscard]] std::optional<u32>
    findBucket(WidgetID id) const noexcept;

    void
    removeBucket(u32 bucket) noexcept;

private:
    ecs::EntityPool slots_;
    ecs::EntityLRUQueue recency_;
    std::vector<WidgetID> ids_;
    std::vector<u64> values_;

    /// Holds a slot index plus one per bucket, or zero for an empty bucket.
    std::vector<u32> buckets_;
    u32 bucketMask_ = 0;
};

} // namespace pomdog::devgui
