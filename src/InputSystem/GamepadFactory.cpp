// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "GamepadFactory.hpp"
#include "Pomdog/Basic/Platform.hpp"
#ifdef POMDOG_PLATFORM_LINUX
#include "../InputSystem.Linux/GamepadLinux.hpp"
#else
#include "../InputSystem/NativeGamepad.hpp"
#endif

namespace Pomdog::Detail::InputSystem {

std::unique_ptr<NativeGamepad> CreateGamepad()
{
#ifdef POMDOG_PLATFORM_LINUX
    return std::make_unique<Linux::GamepadLinux>();
#else
    return nullptr;
#endif
}

} // namespace Pomdog::Detail::InputSystem
