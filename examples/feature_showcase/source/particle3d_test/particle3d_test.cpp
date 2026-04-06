#include "particle3d_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include "pomdog/utility/path_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

Ray ScreenPointToRay(
    const Point2D& screenPoint,
    const Vector3& cameraPosition,
    const gpu::Viewport& viewport,
    const Matrix4x4& viewProjection,
    bool isOrthoProjection)
{
    const auto screenPointVector = Vector3{math::toVector2(screenPoint), 1.0f};
    const auto worldPoint = viewport.unproject(screenPointVector, viewProjection);

    if (isOrthoProjection) {
        const auto cameraPositionInScreen = Vector3{math::toVector2(screenPoint), -1.0f};
        const auto cameraPositionInWorld = viewport.unproject(cameraPositionInScreen, viewProjection);
        Ray ray = {cameraPositionInWorld, math::normalize(worldPoint - cameraPositionInWorld)};
        return ray;
    }

    Ray ray = {cameraPosition, math::normalize(worldPoint - cameraPosition)};
    return ray;
}

} // namespace

Particle3DTest::Particle3DTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
Particle3DTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    if (auto [p, err] = createLinePipeline(fs_, graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LinePipeline");
    }
    else {
        linePipeline_ = std::move(p);
    }
    if (auto [p, err] = createLineBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create LineBatch");
    }
    else {
        lineBatch_ = std::move(p);
    }

    // NOTE: Create billboard batch effect
    if (auto [p, err] = createBillboardBatchEffect(
            fs_,
            graphicsDevice_,
            gpu::BlendDesc::createAlphaBlend(),
            gpu::DepthStencilDesc::createReadOnlyDepth(),
            std::nullopt,
            std::nullopt,
            std::nullopt);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create BillboardBatchEffect");
    }
    else {
        billboardEffect_ = std::move(p);
    }

    // NOTE: Create billboard batch buffer
    if (auto [p, err] = createBillboardBatchBuffer(graphicsDevice_, 4096); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create BillboardBatchBuffer");
    }
    else {
        billboardBuffer_ = std::move(p);
    }

    // NOTE: Create sampler state
    if (auto [sampler, err] = graphicsDevice_->createSamplerState(gpu::SamplerDesc::createLinearClamp()); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sampler state");
    }
    else {
        sampler_ = std::move(sampler);
    }

    // NOTE: Create constant buffer
    if (auto [constantBuffer, err] = graphicsDevice_->createConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            gpu::BufferUsage::Dynamic);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create constant buffer");
    }
    else {
        constantBuffer_ = std::move(constantBuffer);
    }

    // NOTE: Load particle texture.
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/textures/particle_smoke.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    timer_ = std::make_shared<Timer>(clock);
    timer_->setInterval(std::chrono::seconds(1));
    timer_->setScale(0.1);

    {
        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = loadParticleClip(fs_, "/assets/particles/fire3d_box.json");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(clipErr), "failed to load particle json");
        }

        particleSystem_ = std::make_unique<ParticleSystem>(particleClip);
        particleSystem_->Play();
    }

    emitterPosition_ = Vector3::createZero();

    auto mouse = gameHost_->getMouse();
    auto onClipChanged = [this] {
        std::array<std::string, 5> filenames = {
            "particles/fire3d_box.json",
            "particles/fire3d_cone.json",
            "particles/fire3d_hemisphere.json",
            "particles/fire3d_sphere.json",
            "particles/water3d.json",
        };

        currentClipIndex_++;
        if (currentClipIndex_ >= filenames.size()) {
            currentClipIndex_ = 0;
        }

        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = loadParticleClip(fs_, filepaths::joinUnix("/assets", filenames[currentClipIndex_]));
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->toString());
        }
        else {
            particleSystem_ = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect_(mouse->ButtonDown, [this, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost_->getMouse();
        auto mouseState = mouse->getState();
        if (mouseState.rightButton == ButtonState::Down) {
            onClipChanged();
        }
    });

    return nullptr;
}

void Particle3DTest::update()
{
    auto clock = gameHost_->getClock();
    auto frameDuration = clock->getFrameDuration();
    particleSystem_->Simulate(emitterPosition_, Quaternion::createFromAxisAngle(Vector3::createUnitY(), 0.0f), frameDuration);
}

void Particle3DTest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();

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

    const auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadian(45.0f),
        static_cast<float>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
        0.01f,
        500.0f);

    const auto totalTime = static_cast<float>(timer_->getTotalTime().count());
    const auto lookAtPosition = Vector3{0.0f, 0.0f, 5.0f};
    const auto rotation = Matrix4x4::createRotationY(math::TwoPi<float> * totalTime);
    const auto cameraPosition = lookAtPosition + math::transform(Vector3{0.0f, 6.0f, -8.0f}, rotation);
    const auto viewMatrix = Matrix4x4::createLookAtLH(cameraPosition, lookAtPosition, Vector3::createUnitY());
    const auto viewProjection = viewMatrix * projectionMatrix;

    const auto lightDirection = math::normalize(Vector3{-0.5f, -1.0f, 0.5f});

    const auto mouseState = gameHost_->getMouse()->getState();
    if (mouseState.leftButton == ButtonState::Down) {
        auto ray = ScreenPointToRay(
            mouseState.position,
            cameraPosition,
            viewport,
            viewProjection,
            false);
        auto plane = Plane::createFromPointNormal(Vector3::createZero(), Vector3::createUnitY());
        auto rayIntersection = ray.intersects(plane);
        if (rayIntersection) {
            emitterPosition_ = ray.position + ray.direction * (*rayIntersection);
        }
    }

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer constants;
    constants.viewProjection = viewProjection;
    constants.view = viewMatrix;
    constants.projection = projectionMatrix;
    constants.inverseView = math::invert(viewMatrix);
    constants.lightDirection = Vector4{lightDirection, 0.0f};
    constantBuffer_->setData(0, gpu::makeByteSpan(constants));

    // Drawing line
    lineBatch_->reset();
    lineBatch_->setTransform(viewProjection);
    {
        // NOTE: Draw grid
        constexpr int lineCount = 40;
        constexpr float gridSize = 1.0f;
        constexpr float lineLength = gridSize * (lineCount - 1);
        for (int i = 0; i < lineCount; i++) {
            constexpr float startOffsetX = -10.0f;
            constexpr float startOffsetZ = -10.0f;
            float x = static_cast<float>(i) * gridSize + startOffsetX;
            float z = static_cast<float>(i) * gridSize + startOffsetZ;

            auto color = Color::createWhite();
            if (i % 5 != 0) {
                color = Color{255, 255, 255, 100};
            }

            lineBatch_->drawLine(Vector3{x, 0.0f, startOffsetZ}, Vector3{x, 0.0f, lineLength + startOffsetZ}, color);
            lineBatch_->drawLine(Vector3{startOffsetX, 0.0f, z}, Vector3{lineLength + startOffsetX, 0.0f, z}, color);
        }
    }
    lineBatch_->flush(commandList_, linePipeline_);
    lineBatch_->submit(graphicsDevice_);

    billboardBuffer_->reset();

    for (const auto& particle : particleSystem_->GetParticles()) {
        billboardBuffer_->addBillboard(
            particle.Position,
            Vector2{0.0f, 0.0f},
            Vector2{1.0f, 1.0f},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size * Vector2{1.0f, 1.0f});
    }

    billboardBuffer_->fetchBuffer();
    billboardEffect_->draw(commandList_, texture_, sampler_, constantBuffer_, 0, *billboardBuffer_);

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
