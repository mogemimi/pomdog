// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/ecs/entity_lru_queue.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {
namespace {

constexpr u32 InvalidEntity = std::numeric_limits<u32>::max();

} // namespace

EntityLRUQueue::EntityLRUQueue() noexcept = default;

void EntityLRUQueue::initialize(u32 capacity) noexcept
{
    POMDOG_ASSERT(capacity > 0);
    POMDOG_ASSERT(capacity <= u32(0x3FFFFul));
    POMDOG_ASSERT(capacity <= std::numeric_limits<u32>::max());
    prev_.resize(capacity, InvalidEntity);
    next_.resize(capacity, InvalidEntity);

    head_ = InvalidEntity;
    tail_ = InvalidEntity;
    count_ = 0;
}

void EntityLRUQueue::clear() noexcept
{
    std::fill(prev_.begin(), prev_.end(), InvalidEntity);
    std::fill(next_.begin(), next_.end(), InvalidEntity);

    head_ = InvalidEntity;
    tail_ = InvalidEntity;
    count_ = 0;
}

void EntityLRUQueue::put(u32 index) noexcept
{
    POMDOG_ASSERT(index != InvalidEntity);
    POMDOG_ASSERT(index < prev_.size());
    POMDOG_ASSERT(index < next_.size());

    if (index == head_) {
        return;
    }

    // NOTE: Remove entity from cache-list
    if (const auto prev = prev_[index]; prev != InvalidEntity) {
        if (const auto next = next_[index]; next == InvalidEntity) {
            next_[prev] = InvalidEntity;
            tail_ = prev;
        }
        else {
            next_[prev] = next;
            prev_[next] = prev;
        }

        prev_[index] = InvalidEntity;
        next_[index] = InvalidEntity;

        POMDOG_ASSERT(count_ >= 1);
        count_ -= 1;
    }

    // NOTE: Insert entity into cache-list
    {
        if (head_ == InvalidEntity) {
            head_ = index;
            tail_ = index;
        }
        else {
            prev_[head_] = index;
            next_[index] = head_;
            head_ = index;
        }

        count_ += 1;
    }
}

void EntityLRUQueue::remove(u32 index) noexcept
{
    POMDOG_ASSERT(index != InvalidEntity);
    POMDOG_ASSERT(index < prev_.size());
    POMDOG_ASSERT(index < next_.size());

    const auto prev = prev_[index];
    const auto next = next_[index];

    if ((next != InvalidEntity) && (prev != InvalidEntity)) {
        next_[prev] = next;
        prev_[next] = prev;
    }
    else {
        if (index == head_) {
            head_ = next;
            if (head_ != InvalidEntity) {
                prev_[head_] = InvalidEntity;
            }
        }

        if (index == tail_) {
            tail_ = prev;
            if (tail_ != InvalidEntity) {
                next_[tail_] = InvalidEntity;
            }
        }
    }

    prev_[index] = InvalidEntity;
    next_[index] = InvalidEntity;

    POMDOG_ASSERT(count_ > 0);
    count_ -= 1;
}

[[nodiscard]] std::optional<u32>
EntityLRUQueue::oldest() const noexcept
{
    if (tail_ == InvalidEntity) {
        POMDOG_ASSERT(head_ == tail_);
        POMDOG_ASSERT(head_ == InvalidEntity);
        return std::nullopt;
    }

    POMDOG_ASSERT(tail_ < prev_.size());
    POMDOG_ASSERT(tail_ < next_.size());
    return tail_;
}

[[nodiscard]] bool
EntityLRUQueue::cached(u32 index) const noexcept
{
    POMDOG_ASSERT(index != InvalidEntity);
    POMDOG_ASSERT(index < prev_.size());
    POMDOG_ASSERT(index < next_.size());

    if (index == head_) {
        return true;
    }
    if (prev_[index] != InvalidEntity) {
        return true;
    }
    return false;
}

[[nodiscard]] u32
EntityLRUQueue::count() const noexcept
{
    return count_;
}

[[nodiscard]] u32
EntityLRUQueue::capacity() const noexcept
{
    POMDOG_ASSERT(prev_.size() == next_.size());
    return static_cast<u32>(prev_.size());
}

[[nodiscard]] bool
EntityLRUQueue::full() const noexcept
{
    POMDOG_ASSERT(count_ <= capacity());
    return count_ == capacity();
}

void EntityLRUQueue::forEachLatest(std::function<void(u32 index)> f) const noexcept
{
    POMDOG_ASSERT(f);

    for (u32 index = head_; index != InvalidEntity;) {
        POMDOG_ASSERT(index < prev_.size());
        POMDOG_ASSERT(index < next_.size());

        const auto next = next_[index];
        f(index);

        if ((prev_[index] == InvalidEntity) && (next_[index] == InvalidEntity)) {
            // NOTE: Avoid issues if 'f' modifies the queue (e.g., removes the current index)
            index = next;
        }
        else {
            index = next_[index];
        }
    }
}

void EntityLRUQueue::forEachOldest(std::function<void(u32 index)> f) const noexcept
{
    POMDOG_ASSERT(f);

    for (u32 index = tail_; index != InvalidEntity;) {
        POMDOG_ASSERT(index < prev_.size());
        POMDOG_ASSERT(index < next_.size());

        const auto prev = prev_[index];
        f(index);

        if ((prev_[index] == InvalidEntity) && (next_[index] == InvalidEntity)) {
            // NOTE: Avoid issues if 'f' modifies the queue (e.g., removes the current index)
            index = prev;
        }
        else {
            index = prev_[index];
        }
    }
}

[[nodiscard]] std::optional<u32>
EntityLRUQueue::findLatest(std::function<bool(u32 index)> f) const noexcept
{
    POMDOG_ASSERT(f);

    for (u32 index = head_; index != InvalidEntity;) {
        POMDOG_ASSERT(index < prev_.size());
        POMDOG_ASSERT(index < next_.size());

        const auto next = next_[index];
        if (f(index)) {
            return index;
        }

        if ((prev_[index] == InvalidEntity) && (next_[index] == InvalidEntity)) {
            // NOTE: Avoid issues if 'f' modifies the queue (e.g., removes the current index)
            index = next;
        }
        else {
            index = next_[index];
        }
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<u32>
EntityLRUQueue::findOldest(std::function<bool(u32 index)> f) const noexcept
{
    POMDOG_ASSERT(f);

    for (u32 index = tail_; index != InvalidEntity;) {
        POMDOG_ASSERT(index < prev_.size());
        POMDOG_ASSERT(index < next_.size());

        const auto prev = prev_[index];
        if (f(index)) {
            return index;
        }

        if ((prev_[index] == InvalidEntity) && (next_[index] == InvalidEntity)) {
            // NOTE: Avoid issues if 'f' modifies the queue (e.g., removes the current index)
            index = prev;
        }
        else {
            index = prev_[index];
        }
    }
    return std::nullopt;
}

} // namespace pomdog::ecs
