// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents an unsafe raw pointer.
///
/// This is simply an alias for a built-in pointer type (T*).
/// It does not provide any ownership semantics or automatic memory management.
/// Use with caution, as improper use can lead to memory leaks, dangling pointers,
/// and undefined behavior.
template <class T, class = std::enable_if_t<std::is_pointer_v<T*> && !std::is_reference_v<T>>>
using unsafe_ptr = T*;

} // namespace pomdog
