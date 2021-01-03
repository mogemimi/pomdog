// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GamepadFactory.hpp"
#include "Pomdog/Basic/Platform.hpp"
#if defined(POMDOG_PLATFORM_LINUX)
#include "../Input.Linux/GamepadLinux.hpp"
#else
#error "Platform undefined or not supported."
#endif

namespace Pomdog::Detail::X11 {

[[nodiscard]] std::unique_ptr<NativeGamepad>
CreateGamepad() noexcept
{
#if defined(POMDOG_PLATFORM_LINUX)
    return std::make_unique<Linux::GamepadLinux>();
#else
#error "Platform undefined or not supported."
#endif
}

} // namespace Pomdog::Detail::X11
