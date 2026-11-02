// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::network::detail {

enum class AddressFamily : u8 {
    InterNetworkV4,
    InterNetworkV6,
};

} // namespace pomdog::network::detail
