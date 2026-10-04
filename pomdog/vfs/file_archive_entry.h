// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdogschemas {
struct Archive;
struct ArchiveEntry;
} // namespace pomdogschemas

namespace pomdog::vfs::detail {

/// Returns the entry for `key` in `archive`.
///
/// Returns an error when the archive has no entry for `key`, or when the entry is
/// malformed: it uses an unknown compression method, it is uncompressed but its
/// stored and original sizes differ, it refers to a dictionary that the archive
/// does not have, or its stored bytes do not fit in a .pak file of `pakSize` bytes.
[[nodiscard]] std::tuple<const pomdogschemas::ArchiveEntry*, std::unique_ptr<Error>>
findArchiveEntry(const pomdogschemas::Archive& archive, u64 key, std::size_t pakSize) noexcept;

} // namespace pomdog::vfs::detail
