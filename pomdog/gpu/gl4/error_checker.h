// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <source_location>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class DiagnosticQueue;
} // namespace pomdog::detail

namespace pomdog::gpu::detail::gl4 {

/// Returns the last OpenGL error as an Error, or nullptr if no error occurred.
[[nodiscard]] std::unique_ptr<Error>
getLastError() noexcept;

/// ErrorReporterGL4 reports the OpenGL errors found at a check point to a
/// DiagnosticQueue.
///
/// OpenGL keeps an error flag set until glGetError() reads it, so an error
/// found at a check point comes from the command just called or from any
/// earlier call since the previous check. Each instance stops reporting after
/// a fixed number of errors, because an error in a draw call repeats every
/// frame.
class ErrorReporterGL4 final {
private:
    std::shared_ptr<pomdog::detail::DiagnosticQueue> diagnostics_;
    u32 reportCount_ = 0;

public:
    ErrorReporterGL4() noexcept;

    explicit ErrorReporterGL4(std::shared_ptr<pomdog::detail::DiagnosticQueue> diagnostics) noexcept;

    /// Reads every pending OpenGL error and reports each one as a diagnostic.
    /// `checkPoint` names where the check runs, such as the command just
    /// called. Without a queue, the errors are read and discarded.
    void reportPendingErrors(
        std::string_view checkPoint,
        std::source_location location = std::source_location::current()) noexcept;
};

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
#define POMDOG_CHECK_ERROR_GL4(reporter, name) \
    (reporter).reportPendingErrors(name)
#else
#define POMDOG_CHECK_ERROR_GL4(reporter, name)
#endif

} // namespace pomdog::gpu::detail::gl4
