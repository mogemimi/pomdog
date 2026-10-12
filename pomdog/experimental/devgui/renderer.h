// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/text_measurer.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
class SpriteFont;
} // namespace pomdog

namespace pomdog::gpu {
class CommandList;
class GraphicsDevice;
} // namespace pomdog::gpu

namespace pomdog::vfs {
class FileSystemContext;
} // namespace pomdog::vfs

namespace pomdog::devgui {
class DrawList;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Draws devgui draw lists with the engine's 2D batches.
///
/// Shapes go through `PrimitiveBatch` and text through an SDF `SpriteFont`.
/// The renderer also measures text with the same font, so pass it to the
/// `Context` as its text measurer.
///
/// This is one possible backend. A game can draw a `DrawList` with its own
/// renderer instead; the draw list holds only shapes, text, and clip
/// rectangles.
class Renderer : public TextMeasurer {
public:
    ~Renderer() override;

    /// Records the commands of drawList into commandList.
    ///
    /// Call it inside a render pass that covers the window, once per frame
    /// after the frame's last `UI` is destroyed. pixelRatio is the number of
    /// physical pixels per pixel of the draw list: the window's pixel ratio,
    /// multiplied by the UI scale passed to `makeInput()`. It sets scissor
    /// rectangles, shape antialiasing, and text sharpness, and the renderer
    /// moves rectangle edges, horizontal and vertical lines, and text origins
    /// onto physical pixels, so that they stay crisp at fractional ratios.
    /// That holds when the draw list's viewport times pixelRatio is the back
    /// buffer size. antialiasingWidth is the width of the shape alpha fringe in
    /// physical pixels, typically 1; zero disables shape antialiasing. There is
    /// no default so that the caller decides it together with pixelRatio.
    /// The renderer changes the scissor rectangle and restores it
    /// to the whole viewport when it returns.
    virtual void
    draw(const std::shared_ptr<gpu::CommandList>& commandList, const DrawList& drawList, f32 pixelRatio, f32 antialiasingWidth) = 0;
};

/// Creates a renderer that draws text with font.
///
/// font must be created with `sdf` set to true. fontRasterSize is the
/// `fontSize` passed to `createSpriteFont()`; text is scaled from it to the
/// style's font size.
[[nodiscard]] std::tuple<std::shared_ptr<Renderer>, std::unique_ptr<Error>>
createRenderer(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
    const std::shared_ptr<SpriteFont>& font,
    f32 fontRasterSize) noexcept;

} // namespace pomdog::devgui
