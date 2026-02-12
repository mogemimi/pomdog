// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {

/// A constant-time LRU (Least Recently Used) queue for tracking entity usage order.
/// Implemented as a doubly-linked list using array indices with fixed capacity.
/// The head represents the most recently used (newest) entity, and the tail is the least recently used (oldest).
/// NOTE: Must be initialized with a capacity matching EntityPool before use.
class EntityLRUQueue final {
private:
    std::vector<u32> prev_;
    std::vector<u32> next_;

    u32 head_ = ~u32(0);
    u32 tail_ = ~u32(0);
    u32 count_ = 0;

public:
    /// Constructs an empty LRU queue.
    EntityLRUQueue() noexcept;

    /// Initializes the queue with a fixed capacity.
    /// NOTE: Must be called before using other methods.
    void
    initialize(u32 capacity) noexcept;

    /// Clears all entries in the queue.
    void
    clear() noexcept;

    /// Adds an index to the front (head) of the queue or moves it to the front if already present.
    void
    put(u32 index) noexcept;

    /// Removes an index from the queue.
    void
    remove(u32 index) noexcept;

    /// Returns the least recently used (oldest) index from the tail, if any.
    [[nodiscard]] std::optional<u32>
    oldest() const noexcept;

    /// Returns true if the index is currently cached in the queue.
    [[nodiscard]] bool
    cached(u32 index) const noexcept;

    /// Returns the number of indices currently in the queue.
    [[nodiscard]] u32
    count() const noexcept;

    /// Returns the maximum number of indices the queue can hold.
    [[nodiscard]] u32
    capacity() const noexcept;

    /// Returns true if the queue is full.
    [[nodiscard]] bool
    full() const noexcept;

    /// Iterates from most recently used (head) to least recently used (tail).
    ///
    /// NOTE: If entries are removed during the iteration, they will be skipped.
    void
    forEachLatest(std::function<void(u32 index)> f) const noexcept;

    /// Iterates from least recently used (tail) to most recently used (head).
    ///
    /// NOTE: If entries are removed during the iteration, they will be skipped.
    void
    forEachOldest(std::function<void(u32 index)> f) const noexcept;

    /// Finds the first index that satisfies the predicate from newest (head) to oldest (tail).
    /// Performs a linear search from the most recently used to the least recently used.
    ///
    /// NOTE: If entries are removed during the search, they will be skipped.
    [[nodiscard]] std::optional<u32>
    findLatest(std::function<bool(u32 index)> f) const noexcept;

    /// Finds the first index that satisfies the predicate from oldest (tail) to newest (head).
    /// Performs a linear search from the least recently used to the most recently used.
    ///
    /// NOTE: If entries are removed during the search, they will be skipped.
    [[nodiscard]] std::optional<u32>
    findOldest(std::function<bool(u32 index)> f) const noexcept;
};

} // namespace pomdog::ecs
