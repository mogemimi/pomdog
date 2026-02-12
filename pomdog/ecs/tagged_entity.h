// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/ecs/entity.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs {

/// Represents a tagged entity that serves as a phantom type
/// to distinguish entities with different tags.
template <typename T>
class TaggedEntity final {
private:
    Entity entity_ = {};

public:
    /// Creates a null (invalid) tagged entity.
    TaggedEntity() noexcept = default;

    /// Creates a tagged entity from an Entity.
    TaggedEntity(Entity entity) noexcept
        : entity_(entity)
    {
    }

    /// Returns the version as a 14-bit unsigned integer.
    [[nodiscard]] u16 version() const noexcept
    {
        return entity_.version();
    }

    /// Returns the index as an 18-bit unsigned integer.
    [[nodiscard]] u32 index() const noexcept
    {
        return entity_.index();
    }

    /// Returns the ID as a 32-bit unsigned integer composed of the index and version.
    [[nodiscard]] u32 uint32() const noexcept
    {
        return entity_.uint32();
    }

    /// Returns the underlying Entity.
    [[nodiscard]] Entity entity() const noexcept
    {
        return entity_;
    }

    /// Returns true if this tagged entity is equal to another entity.
    [[nodiscard]] bool operator==(const Entity& other) const noexcept
    {
        return entity_.operator==(other);
    }

    /// Returns true if this tagged entity is not equal to another entity.
    [[nodiscard]] bool operator!=(const Entity& other) const noexcept
    {
        return entity_.operator!=(other);
    }

    /// Returns true if this tagged entity is less than another entity.
    [[nodiscard]] bool operator<(const Entity& other) const noexcept
    {
        return entity_.operator<(other);
    }

    /// Returns true if this tagged entity is equal to another tagged entity.
    [[nodiscard]] bool operator==(const TaggedEntity& other) const noexcept
    {
        return entity_.operator==(other.entity_);
    }

    /// Returns true if this tagged entity is not equal to another tagged entity.
    [[nodiscard]] bool operator!=(const TaggedEntity& other) const noexcept
    {
        return entity_.operator!=(other.entity_);
    }

    /// Returns true if this tagged entity is less than another tagged entity.
    [[nodiscard]] bool operator<(const TaggedEntity& other) const noexcept
    {
        return entity_.operator<(other.entity_);
    }

    /// Returns true if this tagged entity is valid (non-null).
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return entity_.operator bool();
    }
};

} // namespace pomdog::ecs

namespace std {

template <class T>
struct hash<pomdog::ecs::TaggedEntity<T>> {
    inline std::size_t operator()(const pomdog::ecs::TaggedEntity<T>& key) const noexcept
    {
        return std::hash<pomdog::u32>()(key.uint32());
    }
};

} // namespace std
