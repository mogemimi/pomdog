#include "texture_atlas_static_test.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

constexpr f32 panelWidth = 320.0f;

/// Looks up all consecutively named frames (name_1, name_2, ...) from an atlas.
[[nodiscard]] std::vector<TextureRegion>
fetchFrames(const std::shared_ptr<TextureAtlas>& atlas, std::string_view base, i32 count)
{
    std::vector<TextureRegion> frames;
    frames.reserve(static_cast<std::size_t>(count));
    for (i32 i = 1; i <= count; ++i) {
        const auto name = pomdog::format("{}_{}", base, i);
        frames.push_back(atlas->getRegionByKey(computeStringHash32(name)));
    }
    return frames;
}

/// Returns the name that the atlas uses for the given frame of a sprite.
[[nodiscard]] std::string
getFrameName(std::string_view spriteName, i32 frameIndex, std::size_t frameCount)
{
    if (frameCount <= 1) {
        return std::string{spriteName};
    }
    return pomdog::format("{}_{}", spriteName, frameIndex + 1);
}

/// Draws the outline of a rectangle whose top-left corner is topLeft.
void drawOutline(PrimitiveBatch& batch, const Vector2& topLeft, f32 width, f32 height, const Color& color)
{
    const auto topRight = Vector2{topLeft.x + width, topLeft.y};
    const auto bottomLeft = Vector2{topLeft.x, topLeft.y - height};
    const auto bottomRight = Vector2{topLeft.x + width, topLeft.y - height};
    batch.drawLine(topLeft, topRight, color, 1.0f);
    batch.drawLine(topRight, bottomRight, color, 1.0f);
    batch.drawLine(bottomRight, bottomLeft, color, 1.0f);
    batch.drawLine(bottomLeft, topLeft, color, 1.0f);
}

} // namespace

TextureAtlasStaticTest::TextureAtlasStaticTest(
    const std::shared_ptr<GameHost>& gameHostIn,
    const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
TextureAtlasStaticTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Pipelines
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

    // NOTE: All three tilesets use premultiplied alpha, so use AlphaBlend.
    if (auto [p, err] = createSpritePipeline(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createAlphaBlend(),
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

    // NOTE: Load tilesets
    std::shared_ptr<TextureAtlas> atlasGirlWithGun;
    std::shared_ptr<TextureAtlas> atlasMisc;
    std::shared_ptr<TextureAtlas> atlasSpritesheet;
    if (auto [atlas, err] = createTextureAtlas(
            fs_,
            graphicsDevice_,
            computeStringHash64("/assets"),
            computeStringHash64("pixelart/girl_with_gun.tileset"),
            computeStringHash64("pixelart/girl_with_gun.png"));
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to load girl_with_gun.tileset");
    }
    else {
        atlasGirlWithGun = std::move(atlas);
    }
    if (auto [atlas, err] = createTextureAtlas(
            fs_,
            graphicsDevice_,
            computeStringHash64("/assets"),
            computeStringHash64("pixelart/misc.tileset"),
            computeStringHash64("pixelart/misc.png"));
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to load misc.tileset");
    }
    else {
        atlasMisc = std::move(atlas);
    }
    if (auto [atlas, err] = createTextureAtlas(
            fs_,
            graphicsDevice_,
            computeStringHash64("/assets"),
            computeStringHash64("textures/spritesheet.tileset"),
            computeStringHash64("textures/spritesheet.png"));
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to load spritesheet.tileset");
    }
    else {
        atlasSpritesheet = std::move(atlas);
    }

    // NOTE: Look up the sprite regions by name.
    const auto addAnimation = [this](std::string_view name, std::string_view texturePath, const std::shared_ptr<TextureAtlas>& atlas, i32 frameCount, f32 framesPerSecond) {
        sprites_.push_back(Sprite{
            .name = name,
            .texturePath = texturePath,
            .atlas = atlas,
            .frames = fetchFrames(atlas, name, frameCount),
            .framesPerSecond = framesPerSecond,
        });
    };
    const auto addStill = [this](std::string_view name, std::string_view texturePath, const std::shared_ptr<TextureAtlas>& atlas) {
        sprites_.push_back(Sprite{
            .name = name,
            .texturePath = texturePath,
            .atlas = atlas,
            .frames = {atlas->getRegionByKey(computeStringHash32(name))},
            .framesPerSecond = 0.0f,
        });
    };
    addAnimation("girl_with_gun", "pixelart/girl_with_gun.png", atlasGirlWithGun, 88, 12.0f);
    addAnimation("gothapron", "pixelart/misc.png", atlasMisc, 20, 8.0f);
    addAnimation("coin", "pixelart/misc.png", atlasMisc, 6, 12.0f);
    addAnimation("punch_maid", "pixelart/misc.png", atlasMisc, 5, 10.0f);
    addAnimation("sailor_walk", "pixelart/misc.png", atlasMisc, 4, 6.0f);
    addStill("particle_lightning", "textures/spritesheet.png", atlasSpritesheet);
    addStill("particle_smoke", "textures/spritesheet.png", atlasSpritesheet);
    addStill("pomdog", "textures/spritesheet.png", atlasSpritesheet);

    for (const auto& sprite : sprites_) {
        spriteNames_.push_back(sprite.name);
    }
    framesPerSecond_ = sprites_.front().framesPerSecond;

    // NOTE: GUI
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    return nullptr;
}

void TextureAtlasStaticTest::buildUI(devgui::UI& ui)
{
    using namespace pomdog::devgui::literals;

    const auto options = devgui::WindowOptions{
        .position = Vector2{8.0f, 48.0f},
        .size = Vector2{panelWidth, 390.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, "textureAtlas"_key, "Texture Atlas", options);
    if (!window.isOpen()) {
        return;
    }

    if (devgui::combo(ui, "sprite"_key, "Sprite", selectedSprite_, spriteNames_).changed) {
        frameIndex_ = 0;
        frameTime_ = 0.0;
        framesPerSecond_ = sprites_[static_cast<std::size_t>(selectedSprite_)].framesPerSecond;
    }

    const auto& sprite = sprites_[static_cast<std::size_t>(selectedSprite_)];
    const auto frameCount = static_cast<i32>(sprite.frames.size());
    if (frameCount > 1) {
        devgui::checkbox(ui, "play"_key, "Play", playing_);
        devgui::sliderFloat(ui, "fps"_key, "FPS", framesPerSecond_, 1.0f, 30.0f);
        // NOTE: Number the frames from 1, as their names do.
        i32 frameNumber = frameIndex_ + 1;
        if (devgui::sliderInt(ui, "frame"_key, "Frame", frameNumber, 1, frameCount).changed) {
            frameIndex_ = frameNumber - 1;
            playing_ = false;
            frameTime_ = 0.0;
        }
    }
    devgui::sliderFloat(ui, "scale"_key, "Scale", scale_, 1.0f, 8.0f);
    devgui::checkbox(ui, "showAtlas"_key, "Show Atlas Texture", showAtlas_);

    devgui::separator(ui);

    const auto frameName = getFrameName(sprite.name, frameIndex_, sprite.frames.size());
    const auto& region = sprite.frames[static_cast<std::size_t>(frameIndex_)];
    devgui::labelText(ui, "Region", frameName);
    devgui::labelText(ui, "Key", pomdog::format("0x{:08X}", computeStringHash32(frameName)));
    devgui::labelText(ui, "Subrect", pomdog::format("({}, {}) {}x{}", region.subrectX, region.subrectY, region.subrectWidth, region.subrectHeight));
    devgui::labelText(ui, "Offset", pomdog::format("({}, {})", region.xOffset, region.yOffset));
    devgui::labelText(ui, "Size", pomdog::format("{}x{}", region.width, region.height));
    devgui::labelText(ui, "Texture", sprite.texturePath);
}

void TextureAtlasStaticTest::update()
{
    // NOTE: Advance the animation by whole frames and keep the remainder.
    const auto& sprite = sprites_[static_cast<std::size_t>(selectedSprite_)];
    const auto frameCount = static_cast<i32>(sprite.frames.size());
    if (playing_ && (frameCount > 1)) {
        const auto framesPerSecond = static_cast<f64>(framesPerSecond_);
        frameTime_ += gameHost_->getClock()->getFrameDuration().count();
        const auto steps = static_cast<i32>(frameTime_ * framesPerSecond);
        frameTime_ -= static_cast<f64>(steps) / framesPerSecond;
        frameIndex_ = (frameIndex_ + steps) % frameCount;
    }

    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });
}

void TextureAtlasStaticTest::draw()
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

    // NOTE: Convert a point from window coordinates (origin at the top-left,
    // y-down) to the coordinates of the projection (origin at the center, y-up).
    const auto toWorld = [w, h](f32 x, f32 y) { return Vector2{x - w * 0.5f, h * 0.5f - y}; };

    const auto& sprite = sprites_[static_cast<std::size_t>(selectedSprite_)];
    const auto& region = sprite.frames[static_cast<std::size_t>(frameIndex_)];
    const auto texture = sprite.atlas->getTexture();
    const auto textureWidth = static_cast<f32>(texture->getWidth());
    const auto textureHeight = static_cast<f32>(texture->getHeight());

    // NOTE: Place the atlas texture at the right edge and the sprite in the
    // middle of the space between the panel and the atlas texture.
    constexpr f32 margin = 16.0f;
    const auto contentLeft = 8.0f + panelWidth;
    const auto atlasLeft = std::round(w - margin - textureWidth);
    const auto atlasTop = std::round((h - textureHeight) * 0.5f);
    const auto spriteCenterX = std::round(showAtlas_ ? (contentLeft + atlasLeft) * 0.5f : (contentLeft + w) * 0.5f);
    const auto spriteCenterY = std::round(h * 0.5f);

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);
    if (showAtlas_) {
        spriteBatch_->draw(
            texture,
            toWorld(atlasLeft, atlasTop + textureHeight),
            Rect2D{0, 0, texture->getWidth(), texture->getHeight()},
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.0f, 0.0f},
            1.0f);
    }
    spriteBatch_->draw(
        texture,
        toWorld(spriteCenterX, spriteCenterY),
        region,
        Color::createWhite(),
        Radian<f32>{0.0f},
        Vector2{0.5f, 0.5f},
        scale_);
    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

    // NOTE: Outline the atlas texture and the subrect of the current frame.
    if (showAtlas_) {
        primitiveBatch_->reset();
        primitiveBatch_->setTransform(projectionMatrix);
        drawOutline(*primitiveBatch_, toWorld(atlasLeft, atlasTop), textureWidth, textureHeight, Color{221, 220, 218, 160});
        drawOutline(
            *primitiveBatch_,
            toWorld(atlasLeft + static_cast<f32>(region.subrectX), atlasTop + static_cast<f32>(region.subrectY)),
            static_cast<f32>(region.subrectWidth),
            static_cast<f32>(region.subrectHeight),
            Color{255, 222, 89, 255});
        primitiveBatch_->flush(commandList_, primitivePipeline_);
        primitiveBatch_->submit(graphicsDevice_);
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
