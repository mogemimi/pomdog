// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/openal/openal_headers.h"
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

namespace pomdog::detail::openal {

[[nodiscard]] std::unique_ptr<Error>
makeOpenALError(ALenum err, std::string_view message) noexcept;

/// ErrorReporterAL reports the OpenAL errors found at a check point to a
/// DiagnosticQueue.
///
/// OpenAL keeps the error state set until alGetError() reads it, so an error
/// found at a check point comes from any call since the previous check. Each
/// instance stops reporting after a fixed number of errors, because an error
/// in a per-frame call repeats every frame.
class ErrorReporterAL final {
private:
    std::shared_ptr<DiagnosticQueue> diagnostics_;
    u32 reportCount_ = 0;

public:
    ErrorReporterAL() noexcept;

    explicit ErrorReporterAL(std::shared_ptr<DiagnosticQueue> diagnostics) noexcept;

    /// Reads the pending OpenAL error and reports it as a diagnostic.
    /// `checkPoint` names where the check runs. Without a queue, the error is
    /// read and discarded.
    void reportPendingErrors(
        std::string_view checkPoint,
        std::source_location location = std::source_location::current()) noexcept;
};

} // namespace pomdog::detail::openal
