// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::clipboard {

/// Describes whether a clipboard feature works on the current platform.
enum class FeatureSupport : u8 {
    /// The platform offers the feature, but Pomdog does not implement it for
    /// this platform yet.
    NotImplemented,

    /// The platform does not offer the feature.
    Unsupported,

    /// Pomdog implements the feature, but it cannot be used at the moment,
    /// for example because a browser allows the clipboard only on secure pages.
    Unavailable,

    /// The feature works. Each request can still fail, for example when the
    /// user denies access.
    Available,
};

/// Reports which clipboard features a service provides.
struct ClipboardCapabilities final {
    /// Writing text with `ClipboardService::requestWriteText()`.
    FeatureSupport writeText = FeatureSupport::NotImplemented;

    /// Reading text with `ClipboardService::requestReadText()`. Browsers may
    /// ask the user for permission first.
    FeatureSupport readText = FeatureSupport::NotImplemented;

    /// Paste events that carry the text of the user's paste action, such as
    /// Ctrl+V in a browser. Where they are Available, a GUI leaves the paste
    /// shortcut to the platform instead of reading the clipboard, so that the
    /// text arrives once and without a permission prompt.
    FeatureSupport pasteEvents = FeatureSupport::NotImplemented;
};

} // namespace pomdog::clipboard
