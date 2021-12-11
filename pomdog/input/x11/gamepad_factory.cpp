// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/x11/gamepad_factory.hpp"
#include "pomdog/basic/platform.hpp"
#if defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/input/linux/gamepad_linux.hpp"
#else
#error "Platform undefined or not supported."
#endif

namespace pomdog::detail::x11 {

[[nodiscard]] std::unique_ptr<NativeGamepad>
CreateGamepad() noexcept
{
#if defined(POMDOG_PLATFORM_LINUX)
    return std::make_unique<linux::GamepadLinux>();
#else
#error "Platform undefined or not supported."
#endif
}

} // namespace pomdog::detail::x11
