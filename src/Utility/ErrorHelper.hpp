// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <system_error>

namespace Pomdog::Detail {

[[nodiscard]] std::errc ToErrc(int err) noexcept;

} // namespace Pomdog::Detail
