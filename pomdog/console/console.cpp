// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/console/console.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(POMDOG_PLATFORM_WIN32)
#include "pomdog/basic/win32/windows_headers.h"
#include <debugapi.h>
#include <limits>
#include <string>
#elif defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten/html5.h>
#else
#include <cstdio>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::console {
namespace {

enum class ConsoleStream : u8 {
    Output,
    Error,
};

#if defined(POMDOG_PLATFORM_WIN32)
[[nodiscard]] std::wstring
toUTF16FromUTF8(std::string_view src)
{
    if (src.empty()) {
        return {};
    }

    if (src.size() > static_cast<std::size_t>(std::numeric_limits<i32>::max())) {
        return {};
    }

    const auto srcSize = static_cast<i32>(src.size());

    const auto dstSize = ::MultiByteToWideChar(
        CP_UTF8,
        0,
        src.data(),
        srcSize,
        nullptr,
        0);

    if (dstSize <= 0) {
        return {};
    }

    std::wstring dst = {};
    dst.resize(static_cast<std::size_t>(dstSize));

    const auto result = ::MultiByteToWideChar(
        CP_UTF8,
        0,
        src.data(),
        srcSize,
        dst.data(),
        dstSize);

    if (result <= 0) {
        return {};
    }

    return dst;
}
#endif

void write(ConsoleStream stream, std::string_view text)
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    if (stream == ConsoleStream::Error) {
        emscripten_console_error(std::string{text}.c_str());
    }
    else {
        emscripten_console_log(std::string{text}.c_str());
    }

#elif defined(POMDOG_PLATFORM_WIN32)
    if (stream == ConsoleStream::Error) {
        OutputDebugStringW(L"[error] ");
    }

    const auto utf16Text = toUTF16FromUTF8(text);
    OutputDebugStringW(utf16Text.c_str());

#else
    auto* file = (stream == ConsoleStream::Error) ? stderr : stdout;
    std::fwrite(text.data(), 1, text.size(), file);
    std::fflush(file);
#endif
}

} // namespace

void write(std::string_view text)
{
    write(ConsoleStream::Output, text);
}

void write_line(std::string_view text)
{
    write(ConsoleStream::Output, text);
    write(ConsoleStream::Output, "\n");
}

void write_error(std::string_view text)
{
    write(ConsoleStream::Error, text);
}

void write_error_line(std::string_view text)
{
    write(ConsoleStream::Error, text);
    write(ConsoleStream::Error, "\n");
}

} // namespace pomdog::console
