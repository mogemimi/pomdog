#include "beam2d_test.h"
#include "pomdog/experimental/graphics/sprite_line.h"
#include "pomdog/experimental/particles/beam_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

Beam2DTest::Beam2DTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
Beam2DTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
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
    if (auto [p, err] = createSpriteBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteBatch");
    }
    else {
        spriteBatch_ = std::move(p);
    }

    // NOTE: Load texture from PNG image file.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/particle_lightning.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.2);

    beamSystem_ = std::make_unique<BeamSystem>();

    const auto swayRange = 15.0f;                  // [0.0f, 70.0f]
    const auto spreadRange = 30.0f;                // [0.0f, 70.0f]
    beamSystem_->emitter.InterpolationPoints = 34; // [0, 100]
    beamSystem_->emitter.StartThickness = 1.2f;    // [0.1, 4.0]
    beamSystem_->branching.BranchingRate = 0.2f;   // [0.0f, 1.0f]
    beamSystem_->emitter.SwayRange = std::uniform_real_distribution<float>(-swayRange, swayRange);
    beamSystem_->branching.SpreadRange = std::uniform_real_distribution<float>(-spreadRange, spreadRange);

    emitterTarget_ = Vector2{200.0, 100.0f};

    return nullptr;
}

void Beam2DTest::update()
{
    const auto mouse = gameHost_->getMouse();
    if (mouse->isButtonDown(MouseButtons::Left)) {
        const auto window = gameHost_->getWindow();
        const auto clientBounds = window->getClientBounds();
        auto pos = mouse->getPosition();
        pos.x = pos.x - (clientBounds.width / 2);
        pos.y = -pos.y + (clientBounds.height / 2);
        emitterTarget_ = math::toVector2(pos);
    }

    const auto emitterPosition = Vector2::createZero();
    const auto clock = gameHost_->getClock();
    const auto frameDuration = clock->getFrameDuration();
    beamSystem_->Update(frameDuration, emitterPosition, emitterTarget_);
}

void Beam2DTest::draw()
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

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<float>(presentationParameters.backBufferWidth),
        static_cast<float>(presentationParameters.backBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.backBufferWidth);
    const auto h = static_cast<float>(presentationParameters.backBufferHeight);
    primitiveBatch_->reset();
    primitiveBatch_->setTransform(projectionMatrix);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->drawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    spriteBatch_->reset();
    spriteBatch_->setTransform(projectionMatrix);

    auto drawBeam = [&](std::vector<Vector2> const& points, float lineThickness, Color const& color) {
        for (std::size_t i = 1; i < points.size(); ++i) {
            POMDOG_ASSERT(i > 0);
            const auto& start = points[i - 1];
            const auto& end = points[i];
            SpriteLine spriteLine;
            spriteLine.draw(
                *spriteBatch_,
                texture_,
                Rect2D{0, 0, 32, 64},
                Rect2D{32, 0, 1, 64},
                Rect2D{33, 0, 31, 64},
                start,
                end,
                lineThickness,
                color);
        }
    };

    for (const auto& beam : beamSystem_->beams) {
        drawBeam(beam.Points, beam.Thickness * 0.02f, beam.Color);
    }

    spriteBatch_->flush(commandList_, spritePipeline_);
    spriteBatch_->submit(graphicsDevice_);

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
