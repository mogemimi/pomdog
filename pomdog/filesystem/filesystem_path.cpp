// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/filesystem/filesystem_path.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"

#if defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/basic/win32/windows_headers.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::platformfs {

std::filesystem::path
toFilesystemPath(std::string_view path) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32)
    if (path.empty()) {
        return {};
    }
    if (path.size() > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return {};
    }

    const auto srcSize = static_cast<i32>(path.size());
    const auto dstSize = ::MultiByteToWideChar(CP_UTF8, 0, path.data(), srcSize, nullptr, 0);
    if (dstSize <= 0) {
        return {};
    }

    std::wstring dst(static_cast<std::size_t>(dstSize), L'\0');
    if (::MultiByteToWideChar(CP_UTF8, 0, path.data(), srcSize, dst.data(), dstSize) <= 0) {
        return {};
    }
    return std::filesystem::path{std::move(dst)};
#else
    return std::filesystem::path{std::string{path}};
#endif
}

std::string
toUTF8String(const std::filesystem::path& path) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32)
    const auto& src = path.native();
    if (src.empty()) {
        return {};
    }
    if (src.size() > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return {};
    }

    const auto srcSize = static_cast<i32>(src.size());
    const auto dstSize = ::WideCharToMultiByte(CP_UTF8, 0, src.data(), srcSize, nullptr, 0, nullptr, nullptr);
    if (dstSize <= 0) {
        return {};
    }

    std::string dst(static_cast<std::size_t>(dstSize), '\0');
    if (::WideCharToMultiByte(CP_UTF8, 0, src.data(), srcSize, dst.data(), dstSize, nullptr, nullptr) <= 0) {
        return {};
    }
    return dst;
#else
    return path.native();
#endif
}

} // namespace pomdog::platformfs
