// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {

/// Represents a lightweight handle to an entity in the ECS system.
/// Contains an index and version number to uniquely identify the entity.
///
/// NOTE: Entities should be created and destroyed via EntityPool.
class Entity final {
private:
    u32 id_ = 0ul;

public:
    /// Creates a null (invalid) entity.
    Entity() noexcept;

    /// Creates an entity from an index and a version.
    Entity(u32 index, u16 version) noexcept;

    /// Returns the version as a 14-bit unsigned integer.
    [[nodiscard]] u16 version() const noexcept;

    /// Returns the index as an 18-bit unsigned integer.
    [[nodiscard]] u32 index() const noexcept;

    /// Returns the ID as a 32-bit unsigned integer composed of the index and version.
    [[nodiscard]] u32 uint32() const noexcept;

    /// Returns true if this entity is equal to another.
    [[nodiscard]] bool operator==(const Entity& other) const noexcept;

    /// Returns true if this entity is not equal to another.
    [[nodiscard]] bool operator!=(const Entity& other) const noexcept;

    /// Compares entities by ID for ordering.
    [[nodiscard]] bool operator<(const Entity& other) const noexcept;

    /// Returns true if this entity is valid (non-null).
    [[nodiscard]] explicit operator bool() const noexcept;
};

/// Returns a null (invalid) entity.
///
/// NOTE: Null entities are not considered alive or valid.
[[nodiscard]] Entity null() noexcept;

} // namespace pomdog::ecs

namespace std {

template <>
struct hash<pomdog::ecs::Entity> {
    std::size_t operator()(const pomdog::ecs::Entity& key) const noexcept;
};

} // namespace std
