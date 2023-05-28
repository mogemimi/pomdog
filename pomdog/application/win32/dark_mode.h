// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/platform/win32/prerequisites_win32.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

[[nodiscard]] bool isWindowsVersionOrGreaterForWindows10(WORD majorVersion, WORD minorVersion, WORD buildVersion) noexcept;

[[nodiscard]] bool isDarkMode() noexcept;

[[nodiscard]] std::unique_ptr<Error> useImmersiveDarkMode(HWND windowHandle, bool enabled) noexcept;

} // namespace pomdog::detail::win32
