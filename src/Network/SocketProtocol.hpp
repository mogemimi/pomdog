// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog::Detail {

enum class SocketProtocol : std::int8_t {
    TCP,
    UDP,
};

} // namespace Pomdog::Detail
