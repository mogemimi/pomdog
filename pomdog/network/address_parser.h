// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/address_family.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::AddressParser {

[[nodiscard]] std::tuple<AddressFamily, std::string_view, std::string_view>
transformAddress(std::string_view address);

} // namespace pomdog::detail::AddressParser
