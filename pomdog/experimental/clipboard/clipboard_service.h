// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/clipboard/clipboard_capabilities.h"
#include "pomdog/experimental/clipboard/clipboard_event.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard {

/// Reads and writes text on the platform's clipboard.
///
/// A service transfers text only. What to copy and how to paste it, such as
/// replacing the selection of a text field, belongs to the caller.
///
/// Requests are asynchronous, because browsers answer clipboard access later,
/// after asking the user. Each request returns an ID, and its result arrives
/// as an event after a later `update()`, even on platforms that finish the
/// request at once. A caller that waits for a result keeps the ID together
/// with whatever the result is for, and checks that the target is still valid
/// when the result arrives:
/// ```cpp
/// clipboard->update();
/// for (const auto& event : clipboard->getEvents()) {
///     if ((event.kind == ClipboardEventKind::ReadCompleted) && (event.requestID == pasteRequest)) {
///         pasteRequest = 0;
///         if (event.status == ClipboardStatus::Succeeded) {
///             pasteIntoField(event.text);
///         }
///     }
/// }
/// ```
///
/// A service is not thread-safe. Use it from the thread that runs the game loop.
class ClipboardService {
public:
    ClipboardService() noexcept;

    ClipboardService(const ClipboardService&) = delete;
    ClipboardService& operator=(const ClipboardService&) = delete;

    virtual ~ClipboardService();

    /// Returns which features the service provides.
    [[nodiscard]] virtual ClipboardCapabilities
    getCapabilities() const noexcept = 0;

    /// Collects the events that arrived since the last call. Call it once per
    /// frame, before reading events.
    virtual void
    update() = 0;

    /// Returns the events that the last `update()` collected, oldest first.
    /// The span stays valid until the next `update()`.
    [[nodiscard]] virtual std::span<const ClipboardEvent>
    getEvents() const noexcept = 0;

    /// Asks to replace the clipboard's contents with text, which is UTF-8.
    ///
    /// Returns the request's ID, which is never zero. Its WriteCompleted event
    /// arrives after a later `update()`.
    virtual u64
    requestWriteText(std::string_view text) = 0;

    /// Asks for the clipboard's text.
    ///
    /// Returns the request's ID, which is never zero. Its ReadCompleted event
    /// arrives after a later `update()`.
    virtual u64
    requestReadText() = 0;
};

} // namespace pomdog::clipboard
