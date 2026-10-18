// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::textinput {

/// Describes whether a text input feature works on the current platform.
enum class FeatureSupport : u8 {
    /// The platform offers the feature, but Pomdog does not implement it for
    /// this platform yet.
    NotImplemented,

    /// The platform does not offer the feature, such as a software keyboard
    /// on a desktop operating system without one.
    Unsupported,

    /// Pomdog implements the feature, but it cannot be used at the moment,
    /// for example because the browser has not created the element that
    /// receives text yet.
    Unavailable,

    /// The feature works.
    Available,
};

/// Reports which text input features a service provides.
///
/// Each feature is reported separately because platforms differ in each of
/// them. A GUI handles every event kind regardless of these values; the values
/// let an application decide what to offer, such as a button that shows the
/// software keyboard.
struct TextInputCapabilities final {
    /// Commit events with typed text, including text from keyboard layouts
    /// for other languages.
    FeatureSupport committedText = FeatureSupport::NotImplemented;

    /// Composition events with text that an input method editor (IME) is
    /// still composing, which the GUI draws inside the field.
    FeatureSupport composition = FeatureSupport::NotImplemented;

    /// Placing the input method's candidate window next to
    /// `TextFieldState::caretRect`.
    FeatureSupport candidateWindowPosition = FeatureSupport::NotImplemented;

    /// Reading `TextFieldState::surroundingText` and sending DeleteSurrounding
    /// events, which input methods use for reconversion and corrections.
    FeatureSupport surroundingText = FeatureSupport::NotImplemented;

    /// Showing and hiding a software keyboard on request.
    FeatureSupport softwareKeyboard = FeatureSupport::NotImplemented;

    /// Reporting the area that the software keyboard covers.
    FeatureSupport softwareKeyboardArea = FeatureSupport::NotImplemented;
};

} // namespace pomdog::textinput
