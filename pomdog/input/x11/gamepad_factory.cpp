// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/x11/gamepad_factory.h"
#include "pomdog/basic/platform.h"
#if defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/input/linux/gamepad_linux.h"
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
