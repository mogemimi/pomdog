// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <memory>

namespace Pomdog::Detail {
class NativeGamepad;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::X11 {

[[nodiscard]] std::unique_ptr<NativeGamepad>
CreateGamepad() noexcept;

} // namespace Pomdog::Detail::X11
