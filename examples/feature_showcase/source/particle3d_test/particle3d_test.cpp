#include "particle3d_test.h"
#include "pomdog/experimental/graphics/basic_effect.h"
#include "pomdog/experimental/particles/particle_clip_loader.h"
#include <random>

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

Particle3DTest::Particle3DTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> Particle3DTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    lineBatch = std::make_shared<LineBatch>(graphicsDevice, *assets);

    // NOTE: Create billboard batch effect
    billboardEffect = std::make_shared<BillboardBatchEffect>(
        graphicsDevice,
        gpu::BlendDescriptor::createAlphaBlend(),
        gpu::DepthStencilDescriptor::createReadOnlyDepth(),
        std::nullopt,
        std::nullopt,
        std::nullopt,
        *assets);

    // NOTE: Create billboard batch buffer
    billboardBuffer = std::make_shared<BillboardBatchBuffer>(graphicsDevice, 4096);

    // NOTE: Create sampler state
    std::tie(sampler, err) = graphicsDevice->createSamplerState(gpu::SamplerDescriptor::createLinearClamp());
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sampler state");
    }

    // NOTE: Create constant buffer
    std::tie(constantBuffer, err) = graphicsDevice->createConstantBuffer(
        sizeof(BasicEffect::WorldConstantBuffer),
        gpu::BufferUsage::Dynamic);
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create constant buffer");
    }

    // NOTE: Load particle texture.
    std::tie(texture, err) = assets->Load<gpu::Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->setInterval(std::chrono::seconds(1));
    timer->setScale(0.1);

    {
        // NOTE: Load particle clip from .json file
        auto [particleClip, clipErr] = assets->Load<ParticleClip>("Particles/Fire3D_Box.json");
        if (clipErr != nullptr) {
            return errors::wrap(std::move(err), "failed to load particle json");
        }

        particleSystem = std::make_unique<ParticleSystem>(particleClip);
        particleSystem->Play();
    }

    emitterPosition = Vector3::createZero();

    auto mouse = gameHost->getMouse();
    auto onClipChanged = [this] {
        std::array<std::string, 5> filenames = {
            "Particles/Fire3D_Box.json",
            "Particles/Fire3D_Cone.json",
            "Particles/Fire3D_Hemisphere.json",
            "Particles/Fire3D_Sphere.json",
            "Particles/Water3D.json",
        };

        currentClipIndex++;
        if (currentClipIndex >= filenames.size()) {
            currentClipIndex = 0;
        }

        // NOTE: Load particle clip from .json file
        auto assets = gameHost->getAssetManager();
        auto [particleClip, clipErr] = assets->Load<ParticleClip>(filenames[currentClipIndex]);
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->toString());
        }
        else {
            particleSystem = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect(mouse->ButtonDown, [this, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost->getMouse();
        auto mouseState = mouse->getState();
        if (mouseState.rightButton == ButtonState::Pressed) {
            onClipChanged();
        }
    });

    return nullptr;
}

void Particle3DTest::update()
{
    auto clock = gameHost->getClock();
    auto frameDuration = clock->getFrameDuration();
    particleSystem->Simulate(emitterPosition, Quaternion::createFromAxisAngle(Vector3::createUnitY(), 0.0f), frameDuration);
}

void Particle3DTest::draw()
{
    const auto presentationParameters = graphicsDevice->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList->reset();
    commandList->setRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::createPerspectiveFieldOfViewLH(
        math::toRadians(45.0f),
        static_cast<float>(presentationParameters.backBufferWidth) / presentationParameters.backBufferHeight,
        0.01f,
        500.0f);

    const auto totalTime = static_cast<float>(timer->getTotalTime().count());
    const auto lookAtPosition = Vector3{0.0f, 0.0f, 5.0f};
    const auto rotation = Matrix4x4::createRotationY(math::TwoPi<float> * totalTime);
    const auto cameraPosition = lookAtPosition + math::transform(Vector3{0.0f, 6.0f, -8.0f}, rotation);
    const auto viewMatrix = Matrix4x4::createLookAtLH(cameraPosition, lookAtPosition, Vector3::createUnitY());
    const auto viewProjection = viewMatrix * projectionMatrix;

    const auto lightDirection = math::normalize(Vector3{-0.5f, -1.0f, 0.5f});

    const auto mouseState = gameHost->getMouse()->getState();
    if (mouseState.leftButton == ButtonState::Pressed) {
        auto ray = ScreenPointToRay(
            mouseState.position,
            cameraPosition,
            viewport,
            viewProjection,
            false);
        auto plane = Plane::createFromPointNormal(Vector3::createZero(), Vector3::createUnitY());
        auto rayIntersection = ray.intersects(plane);
        if (rayIntersection) {
            emitterPosition = ray.position + ray.direction * (*rayIntersection);
        }
    }

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer constants;
    constants.ViewProjection = viewProjection;
    constants.View = viewMatrix;
    constants.Projection = projectionMatrix;
    constants.InverseView = math::invert(viewMatrix);
    constants.LightDirection = Vector4{lightDirection, 0.0f};
    constantBuffer->setData(0, gpu::makeByteSpan(constants));

    // Drawing line
    lineBatch->Begin(commandList, viewProjection);
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

            lineBatch->DrawLine(Vector3{x, 0.0f, startOffsetZ}, Vector3{x, 0.0f, lineLength + startOffsetZ}, color);
            lineBatch->DrawLine(Vector3{startOffsetX, 0.0f, z}, Vector3{lineLength + startOffsetX, 0.0f, z}, color);
        }
    }
    lineBatch->End();

    billboardBuffer->Reset();

    for (const auto& particle : particleSystem->GetParticles()) {
        billboardBuffer->AddBillboard(
            particle.Position,
            Vector2{0.0f, 0.0f},
            Vector2{1.0f, 1.0f},
            particle.Color,
            particle.Rotation,
            Vector2{0.5f, 0.5f},
            particle.Size * Vector2{1.0f, 1.0f});
    }

    billboardBuffer->FetchBuffer();
    billboardEffect->Draw(commandList, texture, sampler, constantBuffer, 0, *billboardBuffer);

    commandList->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->reset();
        commandQueue->pushBackCommandList(commandList);
        commandQueue->executeCommandLists();
        commandQueue->present();
    }
    else {
        commandQueue->pushBackCommandList(commandList);
    }
}

} // namespace feature_showcase
