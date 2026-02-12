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

/// Manages entities organized into multiple bins.
///
/// Each bin stores entities using a linked-list layout implemented with
/// SoA-like per-entity arrays (`node_`, `prev_`, `next_`, `version_`).
/// Provides operations to initialize the structure, append/remove entities,
/// iterate over a bin, and find the first entity satisfying a predicate.
class EntityBinning final {
private:
    struct Bin final {
        u32 firstEntity_ = 0;
        u32 entitiesCount_ = 0;
    };

    std::vector<Bin> bins_;

    // NOTE: SoA like per-entity data.
    std::vector<u32> node_;
    std::vector<u32> prev_;
    std::vector<u32> next_;
    std::vector<u16> version_;

public:
    /// Default constructor.
    ///
    /// The object is constructed in an uninitialized state. Call `initialize`
    /// before using other APIs.
    EntityBinning() noexcept;

    /// Initialize the binning data structure.
    void
    initialize(u32 binCount, u32 entitiesCapacity) noexcept;

    /// Clear all bins and reset internal storage.
    void
    clear() noexcept;

    /// Replace the first entity index of the specified bin with `entity`.
    ///
    /// If the bin already contains entities this will update internal links
    /// so that `entity` becomes part of the bin.
    void
    replace(u32 binIndex, Entity entity) noexcept;

    /// Returns true if the entity index is not associated with any bin.
    [[nodiscard]] bool
    isEntityIndexEmpty(u32 entityIndex) const noexcept;

    /// Returns true if the specified entity exists in any bin.
    [[nodiscard]] bool
    exists(Entity entity) const noexcept;

    /// Append the specified entity to the given bin.
    void
    append(u32 binIndex, Entity entity) noexcept;

    /// Remove the specified entity from its bin.
    void
    remove(Entity entity) noexcept;

    /// Returns the number of bins.
    [[nodiscard]] u32
    getBinCount() const noexcept;

    /// Returns the capacity for entities.
    [[nodiscard]] u32
    getEntitiesCapacity() const noexcept;

    /// Returns the number of entities in the specified bin.
    [[nodiscard]] u32
    getEntitiesCountPerBin(u32 binIndex) const noexcept;

    /// Iterate over each entity in the specified bin and invoke `f`.
    ///
    /// NOTE:
    /// - It is safe if entities are removed during traversal of `forEach`.
    /// - The order in which entities are examined depends on the internal
    ///   bin implementation and should not be relied upon.
    void
    forEach(u32 binIndex, std::function<void(Entity entity)> f) const noexcept;

    /// Returns the first entity in the specified bin that satisfies the given predicate.
    ///
    /// If no such entity is found, returns a null (invalid) entity.
    ///
    /// NOTE:
    /// - It is safe if entities are removed during traversal of `findIf`.
    /// - The order in which entities are examined depends on the internal
    ///   bin implementation and should not be relied upon.
    [[nodiscard]] Entity
    findIf(u32 binIndex, std::function<bool(Entity entity)> f) const noexcept;
};

} // namespace pomdog::ecs
