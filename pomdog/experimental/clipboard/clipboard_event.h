// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::clipboard {

/// Describes how a clipboard request ended.
enum class ClipboardStatus : u8 {
    /// The request succeeded. A read can still return empty text.
    Succeeded,

    /// The clipboard holds no text, for example only an image.
    NoText,

    /// The platform, or Pomdog on this platform, does not support the
    /// request. `ClipboardService::getCapabilities()` tells which.
    Unsupported,

    /// The user or the browser denied access to the clipboard.
    Denied,

    /// The request failed for another reason, for example because another
    /// application kept the clipboard open.
    Failed,
};

/// Identifies what a `ClipboardEvent` reports.
enum class ClipboardEventKind : u8 {
    /// A request from `ClipboardService::requestWriteText()` ended.
    WriteCompleted,

    /// A request from `ClipboardService::requestReadText()` ended. On success
    /// text holds the clipboard's text.
    ReadCompleted,

    /// The user pasted text through the platform, such as with Ctrl+V in a
    /// browser, without a request. text holds the pasted text.
    Paste,
};

/// Reports the end of a clipboard request or a paste by the user.
///
/// Text is UTF-8 with LF line breaks; services convert from the platform's
/// encoding and line breaks.
struct ClipboardEvent final {
    ClipboardEventKind kind = ClipboardEventKind::WriteCompleted;

    /// The request that ended, or zero for Paste.
    u64 requestID = 0;

    ClipboardStatus status = ClipboardStatus::Succeeded;

    /// The text that was read or pasted. Empty for WriteCompleted.
    std::string text;
};

} // namespace pomdog::clipboard
