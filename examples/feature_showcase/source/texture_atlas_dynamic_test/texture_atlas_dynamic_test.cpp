#include "texture_atlas_dynamic_test.h"
#include "pomdog/content/texture_loader.h"
#include "pomdog/experimental/image/image.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

/// Builds a solid-color 64x64 Image for atlas demo purposes.
[[nodiscard]] std::shared_ptr<pomdog::Image>
makeColorImage(i32 w, i32 h, pomdog::Color color)
{
    auto img = std::make_shared<pomdog::Image>(w, h);
    img->fill(pomdog::Color{0, 0, 0, 0});
    for (i32 y = 2; y < h - 2; ++y) {
        for (i32 x = 2; x < w - 2; ++x) {
            img->setPixel(x, y, color);
        }
    }
    return img;
}

} // namespace

TextureAtlasDynamicTest::TextureAtlasDynamicTest(
    const std::shared_ptr<GameHost>& gameHostIn,
    const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
TextureAtlasDynamicTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createPrimitivePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }

    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }

    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createNonPremultiplied(),
            std::nullopt,
            gpu::SamplerDesc::createPointWrap(),
            std::nullopt,
            std::nullopt,
            SpriteBatchPixelShaderMode::Sprite);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpritePipeline");
    }
    else {
        spritePipeline_ = std::move(p);
    }

    if (auto [p, err] = createSpriteBatch(graphicsDevice_, std::nullopt, SpriteBatchOptimizationKind::NotSpecified); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Build a TextureAtlasDynamic from programmatically generated colored sprites.
    dynamicAtlas_ = createTextureAtlasDynamic();

    struct SpriteDesc final {
        std::string name;
        int w;
        int h;
        Color color;
    };

    const std::vector<SpriteDesc> sprites = {
        {"red_32", 32, 32, Color{220, 50, 50, 255}},
        {"green_64", 64, 64, Color{50, 200, 50, 255}},
        {"blue_48", 48, 48, Color{50, 80, 220, 255}},
        {"yellow_24", 24, 24, Color{230, 200, 40, 255}},
        {"cyan_96", 96, 96, Color{40, 200, 210, 255}},
        {"magenta_16", 16, 16, Color{200, 40, 200, 255}},
    };

    for (const auto& s : sprites) {
        auto img = makeColorImage(s.w, s.h, s.color);
        dynamicAtlas_->add(s.name, img, true);
        regionNames_.push_back(s.name);
    }

    if (auto err = dynamicAtlas_->build(graphicsDevice_, 256, 256, 2); err != nullptr) {
        return errors::wrap(std::move(err), "TextureAtlasDynamic::build() failed");
    }

    // NOTE: Cache the regions in the order of regionNames_. The atlas sorts its
    // regions by key, so look each one up by the hash of its name instead of
    // by index.
    for (const auto& name : regionNames_) {
        spriteRegions_.push_back(dynamicAtlas_->getRegionByKey(computeStringHash32(name)));
    }

    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }
    regionNameViews_.assign(regionNames_.begin(), regionNames_.end());

    return nullptr;
}

void TextureAtlasDynamicTest::buildUI(devgui::UI& ui)
{
    using namespace pomdog::devgui::literals;

    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{300.0f, 100.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "textureAtlas"_key, "Texture Atlas", options);
    if (!window.isOpen()) {
        return;
    }
    devgui::combo(ui, "region"_key, "Sprite Region", selectedRegion_, regionNameViews_);
}

void TextureAtlasDynamicTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });
}

void TextureAtlasDynamicTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width),
        static_cast<f32>(clientBounds.height),
        0.0f,
        100.0f);

    const auto w = static_cast<f32>(clientBounds.width);
    const auto h = static_cast<f32>(clientBounds.height);

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 100}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 100}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    if (dynamicAtlas_ && dynamicAtlas_->getTexture()) {
        auto tex = dynamicAtlas_->getTexture();

        spriteBatch_->reset();
        spriteBatch_->setTransform(projectionMatrix);

        // NOTE: Draw the full atlas at top-left
        spriteBatch_->draw(
            tex,
            Vector2{-w * 0.5f + 10.0f + 128.0f, h * 0.5f - 10.0f - 128.0f},
            Rect2D{0, 0, tex->getWidth(), tex->getHeight()},
            SpriteBatchDrawParameters{
                .color = Color::createWhite(),
                .scale = Vector2{1.0f, 1.0f},
            });

        // NOTE: Draw the selected sprite region at centre
        if (!spriteRegions_.empty()) {
            const auto idx = static_cast<std::size_t>(selectedRegion_) % spriteRegions_.size();
            const auto& region = spriteRegions_[idx];
            spriteBatch_->draw(
                tex,
                Vector2{100.0f, 0.0f},
                Rect2D{
                    region.subrectX,
                    region.subrectY,
                    region.subrectWidth,
                    region.subrectHeight,
                },
                SpriteBatchDrawParameters{
                    .color = Color::createWhite(),
                    .scale = Vector2{3.0f, 3.0f},
                });
        }

        spriteBatch_->flush(commandList_, spritePipeline_);
        spriteBatch_->submit(graphicsDevice_);
    }

    commandList_->endRenderPass();

    // NOTE: Draw the panel over the scene.
    {
        gpu::RenderPass overlayPass;
        overlayPass.renderTargets[0] = {nullptr, std::nullopt};
        overlayPass.viewport = viewport;
        overlayPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(overlayPass));
    }
    overlay_.draw(commandList_);

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase
