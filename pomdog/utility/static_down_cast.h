// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename Derived, typename Base>
[[nodiscard]] Derived* StaticDownCast(Base* base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base) == static_cast<Derived*>(base));
#endif
    return static_cast<Derived*>(base);
}

template <typename Derived, typename Base>
[[nodiscard]] std::shared_ptr<Derived> StaticDownCast(const std::shared_ptr<Base>& base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base.get()) == static_cast<Derived*>(base.get()));
#endif
    return std::static_pointer_cast<Derived>(base);
}

template <typename Derived, typename Base>
[[nodiscard]] std::shared_ptr<Derived> StaticDownCast(std::shared_ptr<Base>&& base) noexcept
{
    static_assert(!std::is_same_v<Derived*, Base*>);
    static_assert(std::is_base_of_v<Base, Derived>);
#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(dynamic_cast<Derived*>(base.get()) == static_cast<Derived*>(base.get()));
#endif
    return std::static_pointer_cast<Derived>(std::move(base));
}

} // namespace pomdog::detail
