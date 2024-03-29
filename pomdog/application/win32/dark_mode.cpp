// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/win32/dark_mode.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <dwmapi.h>
#include <uxtheme.h>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {
namespace {

[[nodiscard]] bool isHighContrast() noexcept
{
    HIGHCONTRASTW hc;
    hc.cbSize = sizeof hc;

    bool highContrast = false;
    if (::SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0)) {
        highContrast = ((hc.dwFlags & HCF_HIGHCONTRASTON) != 0);
    }
    return highContrast;
}

[[nodiscard]] bool shouldAppsUseDarkMode() noexcept
{
    if (!isWindowsVersionOrGreaterForWindows10(10, 0, 0)) {
        return false;
    }

    constexpr auto UXTHEME_DLL_NAME = L"uxtheme.dll";

    auto uxthemeModule = ::LoadLibraryExW(UXTHEME_DLL_NAME, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (uxthemeModule == nullptr) {
        return false;
    }

    // NOTE: undocumented Windows API
    using ShouldAppsUseDarkModeFuncType = BOOLEAN(WINAPI*)(void);
    constexpr WORD uxthemeShouldAppsUseDarkModeOrdinal = 132;
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4191)
#endif
    auto shouldAppsUseDarkModeFunc = reinterpret_cast<ShouldAppsUseDarkModeFuncType>(
        ::GetProcAddress(uxthemeModule, MAKEINTRESOURCEA(uxthemeShouldAppsUseDarkModeOrdinal)));
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

    bool darkmode = false;
    if (shouldAppsUseDarkModeFunc != nullptr) {
        darkmode = shouldAppsUseDarkModeFunc();
    }
    ::FreeLibrary(uxthemeModule);
    return darkmode;
}

} // namespace

[[nodiscard]] bool isWindowsVersionOrGreaterForWindows10(WORD majorVersion, WORD minorVersion, WORD buildVersion) noexcept
{
#if 0
    return ::IsWindowsVersionOrGreater(majorVersion, minorVersion, buildVersion);
#elif 0
    OSVERSIONINFOEXA versionInfo;
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    versionInfo.dwMajorVersion = majorVersion;
    versionInfo.dwMinorVersion = minorVersion;
    versionInfo.dwBuildNumber = buildVersion;
    ULONGLONG conditionMask = 0;
    VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);
    return ::VerifyVersionInfoA(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, conditionMask) == 0;
#else
    constexpr const char path[128] = "C:\\Windows\\System32\\kernel32.dll";
    [[maybe_unused]] DWORD dummyHandle = 0;
    DWORD fileVersionSize = ::GetFileVersionInfoSizeA(path, &dummyHandle);
    std::vector<LPBYTE> versionInfo(fileVersionSize);

    if (::GetFileVersionInfoA(path, 0, fileVersionSize, versionInfo.data()) == 0) {
        return false;
    }

    UINT bufferSize = 0;
    VS_FIXEDFILEINFO* fileInfo = nullptr;
    if (::VerQueryValueA(versionInfo.data(), "\\", reinterpret_cast<LPVOID*>(&fileInfo), &bufferSize) == 0) {
        return false;
    }
    if (bufferSize == 0) {
        return false;
    }

    const auto windowsMajor = HIWORD(fileInfo->dwProductVersionMS);
    const auto windowsMinor = LOWORD(fileInfo->dwProductVersionMS);
    const auto windowsBuild = HIWORD(fileInfo->dwProductVersionLS);

    if (majorVersion < windowsMajor) {
        return true;
    }
    if (majorVersion == windowsMajor) {
        if (minorVersion < windowsMinor) {
            return true;
        }
        if (minorVersion == windowsMinor) {
            return buildVersion <= windowsBuild;
        }
    }
    return false;
#endif
}

[[nodiscard]] bool isDarkMode() noexcept
{
    return shouldAppsUseDarkMode() && !isHighContrast();
}

[[nodiscard]] std::unique_ptr<Error> useImmersiveDarkMode(HWND windowHandle, bool enabled) noexcept
{
    if (!isWindowsVersionOrGreaterForWindows10(10, 0, 17763)) {
        // NOTE: not support for dark mode
        return nullptr;
    }

    if (auto hr = ::SetWindowTheme(windowHandle, L"DarkMode_Explorer", nullptr); FAILED(hr)) {
        return errors::make("SetWindowTheme() failed.");
    }

    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
    constexpr DWORD DWMWA_USE_IMMERSIVE_DARK_MODE = 20;

    DWORD attribute = 0;
    if (isWindowsVersionOrGreaterForWindows10(10, 0, 18985)) {
        attribute = DWMWA_USE_IMMERSIVE_DARK_MODE;
    }
    else {
        attribute = DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1;
    }

    const BOOL useDarkMode = enabled ? TRUE : FALSE;
    if (auto hr = ::DwmSetWindowAttribute(windowHandle, attribute, &useDarkMode, sizeof(useDarkMode)); FAILED(hr)) {
        return errors::make("DwmSetWindowAttribute(..., DWMWA_USE_IMMERSIVE_DARK_MODE, ...) failed.");
    }

    return nullptr;
}

} // namespace pomdog::detail::win32
