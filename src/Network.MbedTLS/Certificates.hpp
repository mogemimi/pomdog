// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ArrayView.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

#if defined(_MSC_VER)
// NOTE: Suppress the following warning
// > warning C4686: 'Pomdog::Detail::GetEmbeddedCertificatePEM': possible change in behavior, change in UDT return calling convention
template class ArrayView<std::uint8_t const>;
#endif

[[nodiscard]] ArrayView<std::uint8_t const>
GetEmbeddedCertificatePEM() noexcept;

} // namespace Pomdog::Detail
