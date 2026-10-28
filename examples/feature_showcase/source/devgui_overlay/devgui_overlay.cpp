#include "devgui_overlay.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/truetype_font.h"

namespace feature_showcase {

std::unique_ptr<Error>
DevGUIOverlay::initialize(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs)
{
    gameHost_ = gameHost;
    const auto graphicsDevice = gameHost->getGraphicsDevice();

    // NOTE: The panels show only Latin text.
    auto [font, fontErr] = loadTrueTypeFont(fs, "/assets/fonts/NotoSans-Regular.ttf");
    if (fontErr != nullptr) {
        return errors::wrap(std::move(fontErr), "failed to load a font file");
    }

    // NOTE: The renderer scales the SDF glyphs from this raster size to the
    // style's font size.
    constexpr f32 fontRasterSize = 32.0f;
    constexpr bool useSDF = true;
    std::shared_ptr<SpriteFont> spriteFont;
    if (auto [p, err] = createSpriteFont(graphicsDevice, font, fontRasterSize, fontRasterSize, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont = std::move(p);
    }

    if (auto [p, err] = devgui::createRenderer(fs, graphicsDevice, spriteFont, fontRasterSize); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create devgui renderer");
    }
    else {
        renderer_ = std::move(p);
    }

    context_ = std::make_unique<devgui::Context>(renderer_);
    return nullptr;
}

void DevGUIOverlay::update(const std::function<void(devgui::UI&)>& build)
{
    const auto mouse = gameHost_->getMouse();
    if (mouse == nullptr) {
        return;
    }

    // NOTE: The back buffer size divided by the pixel ratio maps each logical
    // pixel to the same physical pixels, even when the client size in logical
    // pixels was rounded.
    const auto metrics = gameHost_->getWindow()->getDisplayMetrics();
    const auto viewportSize = Vector2{
        static_cast<f32>(metrics.backBufferWidth) / metrics.pixelRatio,
        static_cast<f32>(metrics.backBufferHeight) / metrics.pixelRatio,
    };
    const auto input = devgui::makeInput(
        *mouse,
        gameHost_->getTouchscreen().get(),
        gameHost_->getKeyboard().get(),
        nullptr,
        viewportSize,
        gameHost_->getClock()->getTotalGameTime().count());

    {
        devgui::UI ui{*context_, input};
        build(ui);
    }
    gameHost_->getWindow()->requestMouseCursor(context_->getMouseCursor());
}

devgui::InputCapture DevGUIOverlay::getInputCapture() const noexcept
{
    return context_->getInputCapture();
}

void DevGUIOverlay::draw(const std::shared_ptr<gpu::CommandList>& commandList)
{
    constexpr f32 antialiasingWidth = 1.0f;
    renderer_->draw(commandList, context_->getDrawList(), gameHost_->getWindow()->getPixelRatio(), antialiasingWidth);
}

} // namespace feature_showcase
