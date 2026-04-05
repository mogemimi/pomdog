// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

/// Constructs an object of type T in memory obtained from the given allocator.
///
/// This function allocates memory with proper size and alignment for type T
/// using the allocator, then constructs the object via placement new with the
/// provided arguments.
/// The caller must call `placementDelete()` to destroy the object and release the memory.
///
/// @tparam T The type of object to construct.
/// @tparam Allocator An allocator type that provides `allocate(size, alignment)` returning `unsafe_ptr<void>`.
/// @tparam Args Constructor argument types.
/// @param alloc The allocator to obtain memory from.
/// @param args Arguments to forward to the constructor of T.
/// @return A pointer to the constructed object, or nullptr if allocation failed.
template <typename T, class Allocator, typename... Args>
[[nodiscard]] unsafe_ptr<T> placementNew(Allocator& alloc, Args&&... args)
{
    unsafe_ptr<void> p = alloc.allocate(sizeof(T), alignof(T));
    if (p == nullptr) {
        return nullptr;
    }

    unsafe_ptr<T> ptr = new (p) T(std::forward<Args>(args)...);
    return ptr;
}

/// Destroys an object previously constructed with `placementNew()` and releases its memory.
///
/// This function calls the destructor of the object, then returns the memory
/// to the allocator via `deallocate()`. The pointer passed to this function
/// must have been obtained from `placementNew()` using the same allocator.
///
/// @tparam T The type of object to destroy.
/// @tparam Allocator An allocator type that provides `deallocate(unsafe_ptr<const void>)`.
/// @param alloc The allocator that originally provided the memory.
/// @param ptr A pointer to the object to destroy. Must not be nullptr.
template <typename T, class Allocator>
void placementDelete(Allocator& alloc, unsafe_ptr<T> ptr)
{
    POMDOG_ASSERT(ptr != nullptr);
    ptr->~T();
    alloc.deallocate(ptr);
}

} // namespace pomdog::memory
