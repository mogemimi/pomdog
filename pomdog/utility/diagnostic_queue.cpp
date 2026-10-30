// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/diagnostic_queue.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

DiagnosticQueue::DiagnosticQueue() = default;

DiagnosticQueue::~DiagnosticQueue() = default;

void DiagnosticQueue::enqueue(std::unique_ptr<Error> diagnostic)
{
    POMDOG_ASSERT(diagnostic != nullptr);

    std::lock_guard<std::mutex> lock{mutex_};
    diagnostics_.push_back(std::move(diagnostic));
}

void DiagnosticQueue::emit(const std::function<void(std::unique_ptr<Error> diagnostic)>& callback)
{
    POMDOG_ASSERT(callback);
    POMDOG_ASSERT(notifications_.empty());

    {
        std::lock_guard<std::mutex> lock{mutex_};
        std::swap(notifications_, diagnostics_);
        POMDOG_ASSERT(diagnostics_.empty());
    }

    for (auto& diagnostic : notifications_) {
        callback(std::move(diagnostic));
    }
    notifications_.clear();
}

} // namespace pomdog::detail
