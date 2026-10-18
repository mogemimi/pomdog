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

namespace pomdog::clipboard {
class ClipboardService;
} // namespace pomdog::clipboard

namespace pomdog::clipboard {

/// Creates the clipboard service for the platform that runs gameHost.
///
/// On Windows the service uses the Win32 clipboard with the game window as its
/// owner. On the web it writes and reads with the browser's asynchronous
/// clipboard API and reports the user's pastes into the page's text input
/// element as Paste events. Other platforms get a service whose capabilities
/// report NotImplemented and whose requests end with Unsupported, until they
/// have a backend.
///
/// Returns an error when the platform's clipboard cannot be set up, such as on
/// a headless host without a window.
[[nodiscard]] std::tuple<std::shared_ptr<ClipboardService>, std::unique_ptr<Error>>
createClipboardService(const std::shared_ptr<GameHost>& gameHost);

} // namespace pomdog::clipboard
