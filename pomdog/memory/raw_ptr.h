// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/memory/unsafe_ptr.h"

namespace pomdog {

/// Represents a raw pointer.
///
/// This is an alias for `unsafe_ptr<T>`.
/// It is provided for semantic clarity in codebases that distinguish
/// between raw pointers and other pointer types.
/// Use with caution, as improper use can lead to memory leaks,
/// dangling pointers, and undefined behavior.
template <class T>
using raw_ptr = unsafe_ptr<T>;

} // namespace pomdog
