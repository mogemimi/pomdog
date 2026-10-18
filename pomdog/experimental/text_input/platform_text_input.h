// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class GameHost;
} // namespace pomdog

namespace pomdog::textinput {
class TextInputService;
} // namespace pomdog::textinput

namespace pomdog::textinput {

/// Creates the text input service for the platform that runs gameHost.
///
/// On Windows the service uses the Input Method Manager (IMM32): it reports
/// what an input method editor composes, places its candidate window at the
/// caret, and turns it off while no field has focus. On the web it focuses a
/// hidden input element at the caret, which receives the browser's input
/// method text. Other platforms get a `TextInputServiceKeyboard` until they
/// have a backend.
/// `TextInputService::getCapabilities()` tells what the service provides.
///
/// Returns an error when the platform's text input cannot be set up, such as
/// on a headless host without a window or a keyboard.
[[nodiscard]] std::tuple<std::shared_ptr<TextInputService>, std::unique_ptr<Error>>
createTextInputService(const std::shared_ptr<GameHost>& gameHost);

} // namespace pomdog::textinput
