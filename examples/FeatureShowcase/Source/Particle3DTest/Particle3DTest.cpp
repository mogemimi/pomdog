#include "Particle3DTest.hpp"
#include <Pomdog/Experimental/Graphics/BasicEffect.hpp>
#include <Pomdog/Experimental/Particles/ParticleClipLoader.hpp>
#include <random>

namespace FeatureShowcase {
namespace {

Ray ScreenPointToRay(
    const Point2D& screenPoint,
    const Vector3& cameraPosition,
    const Viewport& viewport,
    const Matrix4x4& viewProjection,
    bool isOrthoProjection)
{
    const auto screenPointVector = Vector3(screenPoint.X, screenPoint.Y, 1.0f);
    const auto worldPoint = viewport.Unproject(screenPointVector, viewProjection);

    if (isOrthoProjection) {
        const auto cameraPositionInScreen = Vector3(screenPoint.X, screenPoint.Y, -1.0f);
        const auto cameraPositionInWorld = viewport.Unproject(cameraPositionInScreen, viewProjection);
        Ray ray = {cameraPositionInWorld, Vector3::Normalize(worldPoint - cameraPositionInWorld)};
        return ray;
    }

    Ray ray = {cameraPosition, Vector3::Normalize(worldPoint - cameraPosition)};
    return ray;
}

} // namespace

Particle3DTest::Particle3DTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void Particle3DTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);

    lineBatch = std::make_shared<LineBatch>(graphicsDevice, *assets);

    // NOTE: Create billboard batch effect
    billboardEffect = std::make_shared<BillboardBatchEffect>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        DepthStencilDescription::CreateReadOnlyDepth(),
        std::nullopt,
        std::nullopt,
        std::nullopt,
        *assets);

    // NOTE: Create billboard batch buffer
    billboardBuffer = std::make_shared<BillboardBatchBuffer>(graphicsDevice, 4096);

    // NOTE: Create sampler state
    sampler = std::make_shared<SamplerState>(
        graphicsDevice,
        SamplerDescription::CreateLinearClamp());

    // NOTE: Create constant buffer
    constantBuffer = std::make_shared<ConstantBuffer>(
        graphicsDevice,
        sizeof(BasicEffect::WorldConstantBuffer),
        BufferUsage::Dynamic);

    std::shared_ptr<Error> err;
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/particle_smoke.png");
    if (err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.1);

    // NOTE: Load particle clip from .json file
    auto [particleClip, clipErr] = assets->Load<ParticleClip>("Particles/Fire3D_Box.json");
    if (clipErr != nullptr) {
        Log::Verbose("failed to load particle json: " + clipErr->ToString());
    }

    particleSystem = std::make_unique<ParticleSystem>(particleClip);
    particleSystem->Play();

    emitterPosition = Vector3::Zero;

    auto mouse = gameHost->GetMouse();
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
        auto assets = gameHost->GetAssetManager();
        auto [particleClip, clipErr] = assets->Load<ParticleClip>(filenames[currentClipIndex]);
        if (clipErr != nullptr) {
            Log::Verbose("failed to load particle json: " + clipErr->ToString());
        }
        else {
            particleSystem = std::make_unique<ParticleSystem>(particleClip);
        }
    };
    connect(mouse->ButtonDown, [this, onClipChanged]([[maybe_unused]] MouseButtons mouseButton) {
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        if (mouseState.RightButton == ButtonState::Pressed) {
            onClipChanged();
        }
    });
}

void Particle3DTest::Update()
{
    auto clock = gameHost->GetClock();
    auto frameDuration = clock->GetFrameDuration();
    particleSystem->Simulate(emitterPosition, Quaternion::CreateFromAxisAngle(Vector3::UnitY, 0.0f), frameDuration);
}

void Particle3DTest::Draw()
{
    const auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    const auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        MathHelper::ToRadians(45.0f),
        static_cast<float>(presentationParameters.BackBufferWidth) / presentationParameters.BackBufferHeight,
        0.01f,
        500.0f);

    const auto totalTime = static_cast<float>(timer->GetTotalTime().count());
    const auto lookAtPosition = Vector3{0.0f, 0.0f, 5.0f};
    const auto rotation = Matrix4x4::CreateRotationY(Math::TwoPi<float> * totalTime);
    const auto cameraPosition = lookAtPosition + Vector3::Transform(Vector3{0.0f, 6.0f, -8.0f}, rotation);
    const auto viewMatrix = Matrix4x4::CreateLookAtLH(cameraPosition, lookAtPosition, Vector3::UnitY);
    const auto viewProjection = viewMatrix * projectionMatrix;

    const auto lightDirection = Vector3::Normalize(Vector3{-0.5f, -1.0f, 0.5f});

    const auto mouseState = gameHost->GetMouse()->GetState();
    if (mouseState.LeftButton == ButtonState::Pressed) {
        auto ray = ScreenPointToRay(
            mouseState.Position,
            cameraPosition,
            viewport,
            viewProjection,
            false);
        auto plane = Plane::CreateFromPointNormal(Vector3::Zero, Vector3::UnitY);
        auto rayIntersection = ray.Intersects(plane);
        if (rayIntersection) {
            emitterPosition = ray.Position + ray.Direction * (*rayIntersection);
        }
    }

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer constants;
    constants.ViewProjection = viewProjection;
    constants.View = viewMatrix;
    constants.Projection = projectionMatrix;
    constants.InverseView = Matrix4x4::Invert(viewMatrix);
    constants.LightDirection = Vector4{lightDirection, 0.0f};
    constantBuffer->SetValue(constants);

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

            auto color = Color::White;
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
    billboardEffect->Draw(commandList, texture, sampler, constantBuffer, *billboardBuffer);

    commandList->Close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue->Reset();
        commandQueue->PushbackCommandList(commandList);
        commandQueue->ExecuteCommandLists();
        commandQueue->Present();
    }
    else {
        commandQueue->PushbackCommandList(commandList);
    }
}

} // namespace FeatureShowcase
