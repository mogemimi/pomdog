// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/high_dpi_settings.h"
#include "pomdog/application/window_mode.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/pixel_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class GameControllerDB;
} // namespace pomdog

namespace pomdog {

/// GameHostOptions describes the initialization parameters passed to a
/// GameHost.
///
/// Set by the game developer in `GameSetup::configure()` before `GameHost`
/// is created. The Bootstrap supplies default values and the game developer
/// overrides what it needs.
struct POMDOG_EXPORT GameHostOptions final {
    /// The graphics backend to use.
    /// The default value is set per-platform by the Bootstrap.
    gpu::GraphicsBackend graphicsBackend = gpu::GraphicsBackend::Direct3D11;

    /// The format of the back buffer.
    gpu::PixelFormat surfaceFormat = gpu::PixelFormat::R8G8B8A8_UNorm;

    /// The format of the depth stencil buffer.
    gpu::PixelFormat depthFormat = gpu::PixelFormat::Depth24Stencil8;

    /// The initial client-area width in logical pixels.
    /// The back buffer is sized to `clientWidth * effectivePixelRatio`.
    i32 clientWidth = 800;

    /// The initial client-area height in logical pixels.
    /// The back buffer is sized to `clientHeight * effectivePixelRatio`.
    i32 clientHeight = 480;

    /// The multisample count for the back buffer.
    i32 multiSampleCount = 1;

    /// The initial window mode.
    /// Defaults to Windowed. Use `requestWindowMode()` at runtime to change.
    WindowMode windowMode = WindowMode::Windowed;

    /// The high-DPI behavior settings.
    ///
    /// The default enables HiDPI on every platform and clamps the effective
    /// pixel ratio at 2.0. See `HighDPISettings` for details.
    HighDPISettings highDPI = {};

    /// The maximum rate at which the back buffers can be presented.
    /// 60 by default. If not set, the game runs with an uncapped frame rate.
    std::optional<i32> maxFramesPerSecond = 60;

    /// Whether to enable V-Sync (display sync) at startup.
    /// The default value is set per-platform by the graphics backend. If not
    /// set, the platform default is used.
    std::optional<bool> displaySyncEnabled = std::nullopt;

    /// Whether to initialize the audio subsystem.
    bool enableAudio = true;

    /// Whether to initialize the gamepad subsystem.
    bool enableGamepad = true;

    /// Whether to initialize the network subsystem (IOService and HTTPClient).
    bool enableNetwork = true;

    /// Whether to run in headless mode (hidden window, no input devices).
    /// In headless mode, graphics are still available for offscreen rendering.
    bool headless = false;

    /// The optional game controller database for gamepad initialization.
    /// If set, the gamepad subsystem uses this database for button mappings.
    /// Can be loaded from VFS before GameHost initialization in
    /// `GameSetup::configure()`.
    std::shared_ptr<const GameControllerDB> gameControllerDB;
};

} // namespace pomdog
