// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/ecs/entity_bin.h"
#include "pomdog/ecs/entity.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <limits>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {
namespace {

constexpr u32 InvalidEntity = std::numeric_limits<u32>::max();
constexpr u32 InvalidBin = std::numeric_limits<u32>::max();

} // namespace

EntityBinning::EntityBinning() noexcept = default;

void EntityBinning::initialize(u32 binCount, u32 entitiesCapacity) noexcept
{
    POMDOG_ASSERT(binCount > 0);
    POMDOG_ASSERT(binCount <= std::numeric_limits<u32>::max());
    bins_.resize(binCount);

    POMDOG_ASSERT(entitiesCapacity > 0);
    POMDOG_ASSERT(entitiesCapacity <= u32(0x3FFFFul));
    POMDOG_ASSERT(entitiesCapacity <= std::numeric_limits<u32>::max());
    node_.resize(entitiesCapacity, InvalidBin);
    prev_.resize(entitiesCapacity, InvalidEntity);
    next_.resize(entitiesCapacity, InvalidEntity);
    version_.resize(entitiesCapacity, u16(0));

    for (auto& bin : bins_) {
        bin.firstEntity_ = InvalidEntity;
        bin.entitiesCount_ = 0;
    }
}

void EntityBinning::clear() noexcept
{
    for (auto& bin : bins_) {
        bin.firstEntity_ = InvalidEntity;
        bin.entitiesCount_ = 0;
    }
    std::fill(node_.begin(), node_.end(), InvalidBin);
    std::fill(prev_.begin(), prev_.end(), InvalidEntity);
    std::fill(next_.begin(), next_.end(), InvalidEntity);
    std::fill(version_.begin(), version_.end(), u16(0));
}

void EntityBinning::replace(u32 binIndex, Entity entity) noexcept
{
    const auto oldBinIndex = node_[entity.index()];
    if (oldBinIndex == binIndex) {
        return;
    }
    if (oldBinIndex != InvalidBin) {
        remove(entity);
    }
    append(binIndex, entity);
}

[[nodiscard]] bool
EntityBinning::isEntityIndexEmpty(u32 entityIndex) const noexcept
{
    POMDOG_ASSERT(entityIndex != InvalidEntity);
    POMDOG_ASSERT(entityIndex < node_.size());
    POMDOG_ASSERT(entityIndex < version_.size());
    return node_[entityIndex] == InvalidBin;
}

[[nodiscard]] bool
EntityBinning::exists(Entity entity) const noexcept
{
    POMDOG_ASSERT(entity);
    const auto entityIndex = entity.index();
    POMDOG_ASSERT(entityIndex != InvalidEntity);
    POMDOG_ASSERT(entityIndex < node_.size());
    POMDOG_ASSERT(entityIndex < version_.size());
    if (node_[entityIndex] == InvalidBin) {
        return false;
    }
    POMDOG_ASSERT(version_[entityIndex] == entity.version());
    return true;
}

void EntityBinning::append(u32 binIndex, Entity entity) noexcept
{
    POMDOG_ASSERT(binIndex < bins_.size());
    auto& node = bins_[binIndex];

    POMDOG_ASSERT(entity);

    const auto entityIndex = entity.index();
    POMDOG_ASSERT(entityIndex != InvalidEntity);
    POMDOG_ASSERT(entityIndex < node_.size());
    POMDOG_ASSERT(entityIndex < next_.size());
    POMDOG_ASSERT(entityIndex < prev_.size());
    POMDOG_ASSERT(entityIndex < version_.size());
    POMDOG_ASSERT(node_[entityIndex] == InvalidBin);
    POMDOG_ASSERT(version_[entityIndex] == 0);

    const auto next = node.firstEntity_;
    node.firstEntity_ = entity.index();
    node.entitiesCount_ += 1;
    node_[entityIndex] = binIndex;
    version_[entityIndex] = entity.version();
    POMDOG_ASSERT(next_[entityIndex] == InvalidEntity);
    POMDOG_ASSERT(prev_[entityIndex] == InvalidEntity);

    if (next != InvalidEntity) {
        POMDOG_ASSERT(prev_[next] == InvalidEntity);
        prev_[next] = entityIndex;
        next_[entityIndex] = next;
    }
}

void EntityBinning::remove(Entity entity) noexcept
{
    POMDOG_ASSERT(entity);

    const auto entityIndex = entity.index();
    POMDOG_ASSERT(entityIndex < node_.size());
    POMDOG_ASSERT(entityIndex < next_.size());
    POMDOG_ASSERT(entityIndex < prev_.size());
    POMDOG_ASSERT(entityIndex < version_.size());

    const auto binIndex = node_[entityIndex];
    if (binIndex == InvalidBin) {
        return;
    }

    POMDOG_ASSERT(entity.version() == version_[entityIndex]);
    version_[entityIndex] = 0;

    auto& bin = bins_[binIndex];
    POMDOG_ASSERT(bin.firstEntity_ != InvalidEntity);
    POMDOG_ASSERT(bin.entitiesCount_ > 0);

    if (bin.firstEntity_ == entity.index()) {
        bin.firstEntity_ = InvalidEntity;
        bin.entitiesCount_ -= 1;

        if (const auto nextIndex = next_[entity.index()]; nextIndex != InvalidEntity) {
            bin.firstEntity_ = nextIndex;

            POMDOG_ASSERT(prev_[nextIndex] == entity.index());
            prev_[nextIndex] = InvalidEntity;
        }
        node_[entity.index()] = InvalidBin;
        next_[entity.index()] = InvalidEntity;
        POMDOG_ASSERT(prev_[entity.index()] == InvalidEntity);
    }
    else {
        bin.entitiesCount_ -= 1;

        const auto prev = prev_[entity.index()];
        const auto next = next_[entity.index()];

        if (prev != InvalidEntity) {
            next_[prev] = next;
        }
        if (next != InvalidEntity) {
            prev_[next] = prev;
        }
        node_[entity.index()] = InvalidBin;
        prev_[entity.index()] = InvalidEntity;
        next_[entity.index()] = InvalidEntity;
        POMDOG_ASSERT(prev_[entity.index()] == InvalidEntity);
        POMDOG_ASSERT(next_[entity.index()] == InvalidEntity);
    }
}

[[nodiscard]] u32
EntityBinning::getBinCount() const noexcept
{
    return static_cast<u32>(bins_.size());
}

[[nodiscard]] u32
EntityBinning::getEntitiesCapacity() const noexcept
{
    return static_cast<u32>(version_.size());
}

[[nodiscard]] u32
EntityBinning::getEntitiesCountPerBin(u32 binIndex) const noexcept
{
    POMDOG_ASSERT(binIndex < bins_.size());

    const auto& bin = bins_[binIndex];
    return bin.entitiesCount_;
}

void EntityBinning::forEach(u32 binIndex, std::function<void(Entity entity)> f) const noexcept
{
    POMDOG_ASSERT(binIndex < bins_.size());
    POMDOG_ASSERT(f);

    const auto& bin = bins_[binIndex];
    if (bin.firstEntity_ == InvalidEntity) {
        return;
    }

    for (auto iter = bin.firstEntity_; iter != InvalidEntity;) {
        const auto next = next_[iter];
        f(Entity{iter, version_[iter]});

        if (node_[iter] == InvalidBin) {
            // NOTE: Avoid issues if 'f' modifies the binning (e.g., removes the current entity)
            iter = next;
        }
        else {
            iter = next_[iter];
        }
    }
}

[[nodiscard]] Entity
EntityBinning::findIf(u32 binIndex, std::function<bool(Entity entity)> f) const noexcept
{
    POMDOG_ASSERT(binIndex < bins_.size());
    POMDOG_ASSERT(f);

    const auto& bin = bins_[binIndex];
    if (bin.firstEntity_ == InvalidEntity) {
        return ecs::null();
    }

    for (auto iter = bin.firstEntity_; iter != InvalidEntity;) {
        const auto next = next_[iter];
        const auto entity = Entity{iter, version_[iter]};
        if (f(entity)) {
            return entity;
        }

        if (node_[iter] == InvalidBin) {
            // NOTE: Avoid issues if 'f' modifies the binning (e.g., removes the current entity)
            iter = next;
        }
        else {
            iter = next_[iter];
        }
    }
    return ecs::null();
}

} // namespace pomdog::ecs
