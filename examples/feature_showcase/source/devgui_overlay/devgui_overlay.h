#pragma once

#include "pomdog/experimental/devgui/devgui.h"
#include "pomdog/experimental/devgui/renderer.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// Shows the control panel of a showcase scene with devgui.
///
/// The overlay builds its UI in every frame, so that a panel can show values
/// that change without input, such as the frame rate.
class DevGUIOverlay final {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    /// Builds the UI of this frame with build. Call it once per frame from
    /// `Game::update()`.
    void update(const std::function<void(devgui::UI&)>& build);

    /// Returns what the UI took from the input of the last frame, so that the
    /// scene ignores a press or a wheel turn over its panel.
    [[nodiscard]] devgui::InputCapture
    getInputCapture() const noexcept;

    /// Draws the UI into commandList, inside a render pass that covers the
    /// window.
    void draw(const std::shared_ptr<gpu::CommandList>& commandList);

private:
    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<devgui::Renderer> renderer_;
    std::unique_ptr<devgui::Context> context_;
};

} // namespace feature_showcase
