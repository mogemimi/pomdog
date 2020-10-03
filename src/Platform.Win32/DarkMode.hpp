// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>

namespace Pomdog::Detail::Win32 {

bool IsWindowsVersionOrGreaterForWindows10(WORD majorVersion, WORD minorVersion, WORD buildVersion) noexcept;

bool IsDarkMode() noexcept;

std::shared_ptr<Error> UseImmersiveDarkMode(HWND windowHandle, bool enabled) noexcept;

} // namespace Pomdog::Detail::Win32
