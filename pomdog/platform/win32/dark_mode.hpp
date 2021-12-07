// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

bool IsWindowsVersionOrGreaterForWindows10(WORD majorVersion, WORD minorVersion, WORD buildVersion) noexcept;

bool IsDarkMode() noexcept;

std::unique_ptr<Error> UseImmersiveDarkMode(HWND windowHandle, bool enabled) noexcept;

} // namespace Pomdog::Detail::Win32
