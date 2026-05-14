// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::console {

/// Writes `text` to the standard output stream for the current platform.
///
/// On Emscripten, `text` is written to the browser console. On Windows,
/// `text` may be written to the debugger output. On other platforms, `text`
/// is written to stdout.
POMDOG_EXPORT void
write(std::string_view text);

/// Writes `text` followed by a newline to the standard output stream for
/// the current platform.
POMDOG_EXPORT void
write_line(std::string_view text);

/// Writes `text` to the standard error stream for the current platform.
///
/// On Emscripten, `text` is written to the browser error console. On Windows,
/// `text` may be written to the debugger output with an error prefix. On other
/// platforms, `text` is written to stderr.
POMDOG_EXPORT void
write_error(std::string_view text);

/// Writes `text` followed by a newline to the standard error stream for
/// the current platform.
POMDOG_EXPORT void
write_error_line(std::string_view text);

/// Formats `fmt` with `args` and writes the result to the standard output stream.
template <typename... Args>
void print(fmt::format_string<Args...> fmt, Args&&... args)
{
    write(pomdog::format(std::move(fmt), std::forward<Args>(args)...));
}

/// Formats `fmt` with `args` and writes the result followed by a newline to the
/// standard output stream.
template <typename... Args>
void println(fmt::format_string<Args...> fmt, Args&&... args)
{
    write_line(pomdog::format(std::move(fmt), std::forward<Args>(args)...));
}

/// Formats `fmt` with `args` and writes the result to the standard error stream.
template <typename... Args>
void eprint(fmt::format_string<Args...> fmt, Args&&... args)
{
    write_error(pomdog::format(std::move(fmt), std::forward<Args>(args)...));
}

/// Formats `fmt` with `args` and writes the result followed by a newline to the
/// standard error stream.
template <typename... Args>
void eprintln(fmt::format_string<Args...> fmt, Args&&... args)
{
    write_error_line(pomdog::format(std::move(fmt), std::forward<Args>(args)...));
}

} // namespace pomdog::console
