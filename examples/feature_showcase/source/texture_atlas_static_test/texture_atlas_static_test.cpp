#include "texture_atlas_static_test.h"
#include "pomdog/experimental/gui/horizontal_layout.h"
#include "pomdog/experimental/gui/stack_panel.h"
#include "pomdog/experimental/gui/text_block.h"
#include "pomdog/experimental/gui/vertical_layout.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/utility/string_hash32.h"
#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

namespace {

/// Looks up all consecutively named frames (name_1, name_2, ...) from an atlas.
[[nodiscard]] std::vector<TextureRegion>
fetchFrames(const std::shared_ptr<TextureAtlas>& atlas, std::string_view base, int count)
{
    std::vector<TextureRegion> frames;
    frames.reserve(static_cast<std::size_t>(count));
    for (int i = 1; i <= count; ++i) {
        const auto name = pomdog::format("{}_{}", base, i);
        const auto key = computeStringHash32(name);
        frames.push_back(atlas->getRegionByKey(key));
    }
    return frames;
}

/// Returns the animated frame index given a total elapsed time, fps, and frame count.
[[nodiscard]] i32
animFrame(f64 totalSeconds, f32 fps, i32 frameCount) noexcept
{
    const auto idx = static_cast<i32>(totalSeconds * fps);
    return idx % frameCount;
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
        atlasGirlWithGun_ = std::move(atlas);
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
        atlasMisc_ = std::move(atlas);
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
        atlasSpritesheet_ = std::move(atlas);
    }

    // NOTE: Pre-fetch sprite regions
    girlWithGunFrames_ = fetchFrames(atlasGirlWithGun_, "girl_with_gun", 88);
    gothapronFrames_ = fetchFrames(atlasMisc_, "gothapron", 20);
    coinFrames_ = fetchFrames(atlasMisc_, "coin", 6);
    punchMaidFrames_ = fetchFrames(atlasMisc_, "punch_maid", 5);
    sailorWalkFrames_ = fetchFrames(atlasMisc_, "sailor_walk", 4);

    particleLightningRegion_ = atlasSpritesheet_->getRegionByKey(computeStringHash32("particle_lightning"));
    particleSmokeRegion_ = atlasSpritesheet_->getRegionByKey(computeStringHash32("particle_smoke"));
    pomdogRegion_ = atlasSpritesheet_->getRegionByKey(computeStringHash32("pomdog"));

    // NOTE: GUI
    drawingContext_ = std::make_unique<gui::DrawingContext>();
    if (auto err = drawingContext_->initialize(graphicsDevice_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DrawingContext");
    }

    auto window = gameHost_->getWindow();
    hierarchy_ = std::make_unique<gui::WidgetHierarchy>(window, gameHost_->getKeyboard());

    auto dispatcher = hierarchy_->getDispatcher();
    {
        auto stackPanel = std::make_shared<gui::StackPanel>(dispatcher, 300, 80);
        stackPanel->setPosition(Point2D{5, 140});
        stackPanel->setPadding(gui::Thickness{8, 2, 2, 8});
        hierarchy_->addChild(stackPanel);

        auto verticalLayout = std::make_shared<gui::VerticalLayout>(dispatcher, 284, 80);
        verticalLayout->setLayoutSpacing(4);
        stackPanel->addChild(verticalLayout);

        auto addLabel = [&](std::string_view text) {
            auto row = std::make_shared<gui::HorizontalLayout>(dispatcher, 280, 16);
            verticalLayout->addChild(row);
            auto tb = std::make_shared<gui::TextBlock>(dispatcher);
            tb->setColor(Color::createWhite());
            tb->setText(std::string{text});
            row->addChild(tb);
        };

        addLabel("girl_with_gun x88 @12fps  |  gothapron x20 @8fps");
        addLabel("coin x6 @12fps  |  punch_maid x5 @10fps  |  sailor_walk x4 @6fps");
        addLabel("particle_lightning  |  particle_smoke  |  pomdog  (static)");
    }

    return nullptr;
}

void TextureAtlasStaticTest::update()
{
    hierarchy_->update();
    if (auto mouse = gameHost_->getMouse(); mouse != nullptr) {
        hierarchy_->touch(*mouse);
    }
    auto clock = gameHost_->getClock();
    hierarchy_->updateAnimation(clock->getFrameDuration());
}

void TextureAtlasStaticTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(presentationParameters.backBufferWidth),
        static_cast<f32>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    const auto w = static_cast<f32>(presentationParameters.backBufferWidth);
    const auto h = static_cast<f32>(presentationParameters.backBufferHeight);

    // NOTE: Axis lines
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    const auto clock = gameHost_->getClock();
    const double totalSec = clock->getTotalGameTime().count();

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);

    // NOTE: girl_with_gun animation (88f @12fps)
    if (!girlWithGunFrames_.empty() && atlasGirlWithGun_) {
        const i32 fi = animFrame(totalSec, 12.0f, static_cast<i32>(girlWithGunFrames_.size()));
        const auto& region = girlWithGunFrames_[static_cast<std::size_t>(fi)];
        spriteBatch_->draw(
            atlasGirlWithGun_->getTexture(),
            Vector2{-200.0f, 120.0f},
            region,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            3.0f);
    }

    // NOTE: gothapron animation (20f @8fps)
    if (!gothapronFrames_.empty() && atlasMisc_) {
        const i32 fi = animFrame(totalSec, 8.0f, static_cast<i32>(gothapronFrames_.size()));
        const auto& region = gothapronFrames_[static_cast<std::size_t>(fi)];
        spriteBatch_->draw(
            atlasMisc_->getTexture(),
            Vector2{80.0f, 120.0f},
            region,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            3.0f);
    }

    // NOTE: coin animation (6f @12fps)
    if (!coinFrames_.empty() && atlasMisc_) {
        const i32 fi = animFrame(totalSec, 12.0f, static_cast<i32>(coinFrames_.size()));
        const auto& region = coinFrames_[static_cast<std::size_t>(fi)];
        spriteBatch_->draw(
            atlasMisc_->getTexture(),
            Vector2{-250.0f, -30.0f},
            region,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            3.0f);
    }

    // NOTE: punch_maid animation (5f @10fps)
    if (!punchMaidFrames_.empty() && atlasMisc_) {
        const i32 fi = animFrame(totalSec, 10.0f, static_cast<i32>(punchMaidFrames_.size()));
        const auto& region = punchMaidFrames_[static_cast<std::size_t>(fi)];
        spriteBatch_->draw(
            atlasMisc_->getTexture(),
            Vector2{-80.0f, -30.0f},
            region,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            3.0f);
    }

    // NOTE: sailor_walk animation (4f @6fps)
    if (!sailorWalkFrames_.empty() && atlasMisc_) {
        const i32 fi = animFrame(totalSec, 6.0f, static_cast<i32>(sailorWalkFrames_.size()));
        const auto& region = sailorWalkFrames_[static_cast<std::size_t>(fi)];
        spriteBatch_->draw(
            atlasMisc_->getTexture(),
            Vector2{100.0f, -30.0f},
            region,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            3.0f);
    }

    // NOTE: Static sprites from spritesheet
    if (atlasSpritesheet_) {
        auto tex = atlasSpritesheet_->getTexture();

        // NOTE: particle_lightning
        spriteBatch_->draw(
            tex,
            Vector2{-200.0f, -170.0f},
            particleLightningRegion_,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            2.0f);

        // NOTE: particle_smoke
        spriteBatch_->draw(
            tex,
            Vector2{-50.0f, -170.0f},
            particleSmokeRegion_,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            2.0f);

        // NOTE: pomdog
        spriteBatch_->draw(
            tex,
            Vector2{200.0f, -130.0f},
            pomdogRegion_,
            Color::createWhite(),
            Radian<f32>{0.0f},
            Vector2{0.5f, 0.5f},
            1.0f);
    }

    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();

    // GUI overlay
    {
        gpu::RenderPass guiPass;
        guiPass.renderTargets[0] = {nullptr, std::nullopt};
        guiPass.viewport = viewport;
        guiPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(guiPass));
    }

    const auto viewMatrix = Matrix4x4::createTranslation(Vector3{
        static_cast<f32>(-presentationParameters.backBufferWidth) * 0.5f,
        static_cast<f32>(-presentationParameters.backBufferHeight) * 0.5f,
        0.0f});

    drawingContext_->reset(presentationParameters.backBufferWidth, presentationParameters.backBufferHeight);
    drawingContext_->beginDraw(commandList_, viewMatrix * projectionMatrix);
    hierarchy_->draw(*drawingContext_);
    drawingContext_->endDraw();

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
