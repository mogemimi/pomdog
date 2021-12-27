// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/network/array_view.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

#if defined(_MSC_VER)
// NOTE: Suppress the following warning
// > warning C4686: 'pomdog::detail::GetEmbeddedCertificatePEM': possible change in behavior, change in UDT return calling convention
template class ArrayView<std::uint8_t const>;
#endif

[[nodiscard]] ArrayView<std::uint8_t const>
GetEmbeddedCertificatePEM() noexcept;

} // namespace pomdog::detail
