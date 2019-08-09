// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Network/ArrayView.hpp"
#include <cstdint>

namespace Pomdog::Detail {

#if defined(_MSC_VER)
// NOTE: Suppress the following warning
// > warning C4686: 'Pomdog::Detail::GetEmbeddedCertificatePEM': possible change in behavior, change in UDT return calling convention
template
class ArrayView<std::uint8_t const>;
#endif

[[nodiscard]] ArrayView<std::uint8_t const>
GetEmbeddedCertificatePEM() noexcept;

} // namespace Pomdog::Detail
