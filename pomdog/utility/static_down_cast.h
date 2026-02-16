// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/raw_ptr.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

/// Performs a static downcast from raw_ptr<Base> to raw_ptr<Derived>.
/// In debug builds, it asserts that the cast is valid using dynamic_cast.
template <typename Derived, typename Base>
[[nodiscard]] raw_ptr<Derived>
static_down_cast(raw_ptr<Base> base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base) == static_cast<Derived*>(base));
#endif
    return static_cast<Derived*>(base);
}

/// Performs a static downcast from std::shared_ptr<Base> to std::shared_ptr<Derived>.
/// In debug builds, it asserts that the cast is valid using dynamic_cast.
template <typename Derived, typename Base>
[[nodiscard]] std::shared_ptr<Derived>
static_down_cast(const std::shared_ptr<Base>& base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base.get()) == static_cast<Derived*>(base.get()));
#endif
    return std::static_pointer_cast<Derived>(base);
}

/// Performs a static downcast from std::shared_ptr<Base> to std::shared_ptr<Derived>.
/// In debug builds, it asserts that the cast is valid using dynamic_cast.
template <typename Derived, typename Base>
[[nodiscard]] std::shared_ptr<Derived>
static_down_cast(std::shared_ptr<Base>&& base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base.get()) == static_cast<Derived*>(base.get()));
#endif
    return std::static_pointer_cast<Derived>(std::move(base));
}

} // namespace pomdog::detail
