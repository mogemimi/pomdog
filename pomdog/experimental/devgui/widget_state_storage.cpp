// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/widget_state_storage.h"
#include "pomdog/ecs/entity.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <bit>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// Returns the bucket where the probe sequence for id starts.
[[nodiscard]] u32
homeBucket(WidgetID id, u32 mask) noexcept
{
    // NOTE: Widget IDs are already well-mixed hashes, so their low bits can be
    // used as the bucket index directly.
    return static_cast<u32>(id.value) & mask;
}

} // namespace

WidgetStateStorage::WidgetStateStorage() noexcept = default;

void WidgetStateStorage::initialize(u32 capacity)
{
    POMDOG_ASSERT(capacity > 0);

    slots_ = ecs::EntityPool{};
    slots_.initialize(capacity);
    recency_ = ecs::EntityLRUQueue{};
    recency_.initialize(capacity);

    ids_.assign(capacity, WidgetID{});
    values_.assign(capacity, 0);

    // NOTE: Keep the load factor at or below one half so that probe sequences
    // stay short.
    const auto bucketCount = std::bit_ceil(capacity * 2);
    buckets_.assign(bucketCount, 0);
    bucketMask_ = bucketCount - 1;
}

std::optional<u64> WidgetStateStorage::find(WidgetID id) noexcept
{
    const auto bucket = findBucket(id);
    if (!bucket) {
        return std::nullopt;
    }
    const auto slot = buckets_[*bucket] - 1;
    recency_.put(slot);
    return values_[slot];
}

void WidgetStateStorage::store(WidgetID id, u64 value) noexcept
{
    POMDOG_ASSERT(!buckets_.empty());
    POMDOG_ASSERT(id.value != 0);

    if (const auto bucket = findBucket(id); bucket) {
        const auto slot = buckets_[*bucket] - 1;
        values_[slot] = value;
        recency_.put(slot);
        return;
    }

    if (slots_.count() >= slots_.capacity()) {
        // NOTE: Evict the least recently used value to make room.
        const auto oldest = recency_.oldest();
        POMDOG_ASSERT(oldest);
        remove(ids_[*oldest]);
    }

    // NOTE: `EntityPool::create()` returns the lowest free index, which is
    // below the capacity because the pool is not full.
    const auto entity = slots_.create();
    const auto slot = entity.index();
    POMDOG_ASSERT(slot < ids_.size());

    ids_[slot] = id;
    values_[slot] = value;
    recency_.put(slot);

    auto bucket = homeBucket(id, bucketMask_);
    while (buckets_[bucket] != 0) {
        bucket = (bucket + 1) & bucketMask_;
    }
    buckets_[bucket] = slot + 1;
}

void WidgetStateStorage::remove(WidgetID id) noexcept
{
    if (const auto bucket = findBucket(id); bucket) {
        removeBucket(*bucket);
    }
}

void WidgetStateStorage::clear() noexcept
{
    if (buckets_.empty()) {
        return;
    }
    slots_.destroyAll();
    recency_.clear();
    std::fill(ids_.begin(), ids_.end(), WidgetID{});
    std::fill(buckets_.begin(), buckets_.end(), 0u);
}

u32 WidgetStateStorage::count() const noexcept
{
    return slots_.count();
}

u32 WidgetStateStorage::capacity() const noexcept
{
    return static_cast<u32>(ids_.size());
}

std::optional<u32> WidgetStateStorage::findBucket(WidgetID id) const noexcept
{
    if (buckets_.empty() || (id.value == 0)) {
        return std::nullopt;
    }

    auto bucket = homeBucket(id, bucketMask_);
    while (buckets_[bucket] != 0) {
        if (ids_[buckets_[bucket] - 1] == id) {
            return bucket;
        }
        bucket = (bucket + 1) & bucketMask_;
    }
    return std::nullopt;
}

void WidgetStateStorage::removeBucket(u32 bucket) noexcept
{
    const auto slot = buckets_[bucket] - 1;
    recency_.remove(slot);
    slots_.destroy(slots_.at(slot));
    ids_[slot] = WidgetID{};

    // NOTE: Backward-shift deletion for linear probing. Each following entry
    // moves into the hole unless its home bucket lies cyclically within
    // (hole, entry], which would put it before its home.
    auto hole = bucket;
    auto next = (hole + 1) & bucketMask_;
    while (buckets_[next] != 0) {
        const auto home = homeBucket(ids_[buckets_[next] - 1], bucketMask_);
        const auto distanceToEntry = (next - home) & bucketMask_;
        const auto distanceToHole = (next - hole) & bucketMask_;
        if (distanceToEntry >= distanceToHole) {
            buckets_[hole] = buckets_[next];
            hole = next;
        }
        next = (next + 1) & bucketMask_;
    }
    buckets_[hole] = 0;
}

} // namespace pomdog::devgui
