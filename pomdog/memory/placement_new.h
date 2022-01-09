// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

template <typename T, class Allocator, typename... Args>
[[nodiscard]] unsafe_ptr<T> PlacementNew(Allocator& alloc, Args&&... args)
{
    unsafe_ptr<void> p = alloc.Allocate(sizeof(T), alignof(T));
    POMDOG_ASSERT(p != nullptr);

    unsafe_ptr<T> ptr = new (p) T(std::forward<Args>(args)...);
    return ptr;
}

template <typename T, class Allocator>
void PlacementDelete(Allocator& alloc, unsafe_ptr<T> ptr)
{
    POMDOG_ASSERT(ptr != nullptr);
    ptr->~T();
    alloc.Deallocate(ptr);
}

} // namespace pomdog::detail
