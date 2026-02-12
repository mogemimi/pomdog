// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/ecs/entity_pool.h"
#include "pomdog/ecs/entity.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/bit.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {
namespace {
constexpr u32 bitsPerWord = 64ul;
constexpr u32 maxUint14 = 0x3FFFul;
constexpr u32 maxUint18 = 0x3FFFFul;
} // namespace

EntityPool::EntityPool() noexcept = default;

void EntityPool::initialize(u32 capacity)
{
    POMDOG_ASSERT(capacity <= maxUint18);

    const u32 words = (capacity + (bitsPerWord - 1)) / u32(bitsPerWord);
    POMDOG_ASSERT(words * bitsPerWord >= capacity);
    POMDOG_ASSERT(words * bitsPerWord < capacity + bitsPerWord);

    ownership_.resize(words, u64(0));
    versions_.resize(capacity, u16(1));
    count_ = 0;
}

Entity EntityPool::create() noexcept
{
    POMDOG_ASSERT(!ownership_.empty());
    POMDOG_ASSERT(!versions_.empty());
    POMDOG_ASSERT(ownership_.size() > 0);

    const u32 index = [&]() -> u32 {
        for (u32 k = 0; k < ownership_.size(); k++) {
            if (const auto x = ownership_[k]; x != std::numeric_limits<u64>::max()) {
                const auto p = static_cast<u32>(pomdog::countr_one(x));
                const auto r = (k * bitsPerWord) + p;
                return r;
            }
        }
        return maxUint18;
    }();

    const u32 word = index / bitsPerWord;
    const u64 mask = u64(1) << (index % bitsPerWord);

    POMDOG_ASSERT(word < ownership_.size());
    POMDOG_ASSERT(index < versions_.size());
    POMDOG_ASSERT(index < (ownership_.size() * bitsPerWord));

    POMDOG_ASSERT((ownership_[word] & mask) == 0);
    ownership_[word] |= mask;
    count_++;

    const auto version = versions_[index];
    POMDOG_ASSERT(version > 0);
    return Entity{index, version};
}

void EntityPool::destroy(Entity entity) noexcept
{
    const auto index = entity.index();
    if (index >= capacity()) {
        return;
    }

    const u32 word = index / bitsPerWord;
    const u64 mask = u64(1) << (index % bitsPerWord);

    POMDOG_ASSERT(word < ownership_.size());
    POMDOG_ASSERT(index < versions_.size());

    if (versions_[index] != entity.version()) {
        return;
    }

    ownership_[word] &= (~mask);
    versions_[index] += u16(1);

    if (versions_[index] > maxUint14) {
        versions_[index] = u16(1);
    }
    POMDOG_ASSERT(versions_[index] > 0);

    POMDOG_ASSERT(count_ > 0);
    count_--;
}

void EntityPool::destroyAll() noexcept
{
    POMDOG_ASSERT(capacity() > 0);
    std::fill(ownership_.begin(), ownership_.end(), u64(0));
    std::fill(versions_.begin(), versions_.end(), u16(1));
    count_ = 0;
}

bool EntityPool::exists(Entity entity) const noexcept
{
    const auto index = entity.index();
    return (index < versions_.size()) && (versions_[index] == entity.version());
}

Entity EntityPool::at(int indexInt32) const noexcept
{
    POMDOG_ASSERT(indexInt32 >= 0);
    const auto index = static_cast<u32>(indexInt32);
    POMDOG_ASSERT(index < versions_.size());

    const u32 word = index / bitsPerWord;
    const u64 mask = u64(1) << (index % bitsPerWord);
    POMDOG_ASSERT(word < ownership_.size());

    if ((ownership_[word] & mask) == 0) {
        return ecs::null();
    }
    return Entity{index, versions_[index]};
}

u32 EntityPool::count() const noexcept
{
    return count_;
}

u32 EntityPool::capacity() const noexcept
{
    POMDOG_ASSERT(ownership_.size() * bitsPerWord >= versions_.size());
    POMDOG_ASSERT(ownership_.size() * bitsPerWord < versions_.size() + bitsPerWord);
    return static_cast<u32>(versions_.size());
}

void EntityPool::forEach(std::function<void(Entity entity)> func) const noexcept
{
    POMDOG_ASSERT(func != nullptr);

    const auto ownershipCount = static_cast<u32>(ownership_.size());

    for (u32 k = 0; k < ownershipCount; k++) {
        if (const auto x = ownership_[k]; x != 0) {
            for (u32 p = 0; p < bitsPerWord; p++) {
                const u64 mask = u64(1) << p;
                const u32 index = (k * bitsPerWord) + p;
                if ((x & mask) != 0) {
                    POMDOG_ASSERT(index < versions_.size());
                    func(Entity{index, versions_[index]});
                }
            }
        }
    }
}

void EntityPool::forEachReverse(std::function<void(Entity entity)> func) const noexcept
{
    POMDOG_ASSERT(func != nullptr);

    const auto ownershipCount = static_cast<i32>(ownership_.size());

    for (i32 kSigned = ownershipCount - 1; kSigned >= 0; kSigned--) {
        const auto k = static_cast<u32>(kSigned);

        if (const auto x = ownership_[k]; x != 0) {
            for (i32 pSigned = static_cast<i32>(bitsPerWord) - 1; pSigned >= 0; pSigned--) {
                const auto p = static_cast<u32>(pSigned);

                const u64 mask = u64(1) << p;
                const u32 index = (k * bitsPerWord) + p;
                if ((x & mask) != 0) {
                    POMDOG_ASSERT(index < versions_.size());
                    func(Entity{index, versions_[index]});
                }
            }
        }
    }
}

} // namespace pomdog::ecs
