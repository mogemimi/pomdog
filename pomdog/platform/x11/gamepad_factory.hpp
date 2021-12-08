// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include <memory>

namespace pomdog::detail {
class NativeGamepad;
} // namespace pomdog::detail

namespace pomdog::detail::x11 {

[[nodiscard]] std::unique_ptr<NativeGamepad>
CreateGamepad() noexcept;

} // namespace pomdog::detail::x11
