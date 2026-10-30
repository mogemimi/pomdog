// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::openal {

std::unique_ptr<Error>
makeOpenALError(ALenum err, std::string_view message) noexcept
{
    return errors::make(pomdog::format("{}: alGetError() returned {:#06x}", message, err));
}

ErrorReporterAL::ErrorReporterAL() noexcept = default;

ErrorReporterAL::ErrorReporterAL(std::shared_ptr<DiagnosticQueue> diagnostics) noexcept
    : diagnostics_(std::move(diagnostics))
{
}

void ErrorReporterAL::reportPendingErrors(
    std::string_view checkPoint,
    std::source_location location) noexcept
{
    constexpr u32 maxReportCount = 30;

    // NOTE: Unlike OpenGL, OpenAL keeps a single error state, so one call to
    // alGetError() reads and clears it.
    const ALenum errorCode = alGetError();
    if (errorCode == AL_NO_ERROR) {
        return;
    }
    if ((diagnostics_ == nullptr) || (reportCount_ > maxReportCount)) {
        return;
    }
    ++reportCount_;
    if (reportCount_ > maxReportCount) {
        diagnostics_->enqueue(errors::make(pomdog::format(
            "stopped reporting OpenAL errors after {} reports", maxReportCount)));
        return;
    }
    diagnostics_->enqueue(errors::make(pomdog::format(
        "OpenAL error {:#06x} at or before {} ({}:{})",
        errorCode,
        checkPoint,
        location.file_name(),
        location.line())));
}

} // namespace pomdog::detail::openal
