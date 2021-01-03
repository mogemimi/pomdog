// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

bool IsWindowsVersionOrGreaterForWindows10(WORD majorVersion, WORD minorVersion, WORD buildVersion) noexcept;

bool IsDarkMode() noexcept;

std::unique_ptr<Error> UseImmersiveDarkMode(HWND windowHandle, bool enabled) noexcept;

} // namespace Pomdog::Detail::Win32
