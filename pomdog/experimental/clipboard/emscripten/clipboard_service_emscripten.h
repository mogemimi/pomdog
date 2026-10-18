// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::clipboard::detail::emscripten {

/// Implements `ClipboardService` with the browser's clipboard.
///
/// Requests use the asynchronous clipboard API, `navigator.clipboard`, which
/// browsers offer only on secure pages and may guard with a permission prompt.
/// The user's pastes into the element that the text input service focuses,
/// which carries the `data-pomdog-text-input` attribute, become Paste events
/// instead of text in that element, so that they arrive once.
class ClipboardServiceEmscripten final : public ClipboardService {
public:
    ClipboardServiceEmscripten() noexcept;

    ClipboardServiceEmscripten(const ClipboardServiceEmscripten&) = delete;
    ClipboardServiceEmscripten& operator=(const ClipboardServiceEmscripten&) = delete;

    ~ClipboardServiceEmscripten() override;

    /// Starts listening for pastes.
    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;

    [[nodiscard]] ClipboardCapabilities
    getCapabilities() const noexcept override;

    void
    update() override;

    [[nodiscard]] std::span<const ClipboardEvent>
    getEvents() const noexcept override;

    u64
    requestWriteText(std::string_view text) override;

    u64
    requestReadText() override;

private:
    std::vector<ClipboardEvent> events_;
    ClipboardCapabilities capabilities_ = {};
    u64 nextRequestID_ = 1;
    bool initialized_ = false;
};

} // namespace pomdog::clipboard::detail::emscripten
