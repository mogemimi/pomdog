// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

std::unique_ptr<Error>
getLastError() noexcept
{
    const auto errorCode = glGetError();
    if (errorCode == GL_NO_ERROR) {
        return nullptr;
    }
    return errors::make(pomdog::format("glGetError() returned {:#06x}", errorCode));
}

ErrorReporterGL4::ErrorReporterGL4() noexcept = default;

ErrorReporterGL4::ErrorReporterGL4(std::shared_ptr<pomdog::detail::DiagnosticQueue> diagnostics) noexcept
    : diagnostics_(std::move(diagnostics))
{
}

void ErrorReporterGL4::reportPendingErrors(
    std::string_view checkPoint,
    std::source_location location) noexcept
{
    constexpr u32 maxReportCount = 30;

    // NOTE: Several error flags can be set at once, and a lost context can
    // report an error on every call, so read a bounded number of them.
    constexpr int maxErrorsPerCheck = 8;

    for (int i = 0; i < maxErrorsPerCheck; ++i) {
        const auto errorCode = glGetError();
        if (errorCode == GL_NO_ERROR) {
            return;
        }
        if ((diagnostics_ == nullptr) || (reportCount_ > maxReportCount)) {
            continue;
        }
        ++reportCount_;
        if (reportCount_ > maxReportCount) {
            diagnostics_->enqueue(errors::make(pomdog::format(
                "stopped reporting OpenGL errors after {} reports", maxReportCount)));
            continue;
        }
        diagnostics_->enqueue(errors::make(pomdog::format(
            "OpenGL error {:#06x} at or before {} ({}:{})",
            errorCode,
            checkPoint,
            location.file_name(),
            location.line())));
    }
}

} // namespace pomdog::gpu::detail::gl4
