#include "beam2d_test.h"
#include "pomdog/experimental/graphics/sprite_line.h"
#include "pomdog/experimental/particles/beam_system.h"
#include <random>

namespace feature_showcase {

Beam2DTest::Beam2DTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::unique_ptr<Error> Beam2DTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, *assets);
    spriteBatch = std::make_shared<SpriteBatch>(
        graphicsDevice,
        BlendDescription::CreateAlphaBlend(),
        std::nullopt,
        SamplerDescription::CreatePointWrap(),
        std::nullopt,
        std::nullopt,
        SpriteBatchPixelShaderMode::Default,
        *assets);

    // NOTE: Load texture from PNG image file.
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/particle_lightning.png");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    timer = std::make_shared<Timer>(clock);
    timer->SetInterval(std::chrono::seconds(1));
    timer->SetScale(0.2);

    beamSystem = std::make_unique<BeamSystem>();

    const auto swayRange = 15.0f; // [0.0f, 70.0f]
    const auto spreadRange = 30.0f; // [0.0f, 70.0f]
    beamSystem->emitter.InterpolationPoints = 34; // [0, 100]
    beamSystem->emitter.StartThickness = 1.2f; // [0.1, 4.0]
    beamSystem->branching.BranchingRate = 0.2f; // [0.0f, 1.0f]
    beamSystem->emitter.SwayRange = std::uniform_real_distribution<float>(-swayRange, swayRange);
    beamSystem->branching.SpreadRange = std::uniform_real_distribution<float>(-spreadRange, spreadRange);

    emitterTarget = Vector2{200.0, 100.0f};

    auto mouse = gameHost->GetMouse();
    connect(mouse->Moved, [this](const Point2D& mousePos) {
        auto mouse = gameHost->GetMouse();
        auto mouseState = mouse->GetState();
        if (mouseState.LeftButton != ButtonState::Pressed) {
            return;
        }
        auto window = gameHost->GetWindow();
        auto pos = mousePos;
        pos.X = pos.X - (window->GetClientBounds().Width / 2);
        pos.Y = -pos.Y + (window->GetClientBounds().Height / 2);
        emitterTarget = math::ToVector2(pos);
    });

    return nullptr;
}

void Beam2DTest::Update()
{
    const auto emitterPosition = Vector2::Zero();
    const auto clock = gameHost->GetClock();
    const auto frameDuration = clock->GetFrameDuration();
    beamSystem->Update(frameDuration, emitterPosition, emitterTarget);
}

void Beam2DTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.DepthStencilBuffer = nullptr;
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = std::uint8_t(0);
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        static_cast<float>(presentationParameters.BackBufferWidth),
        static_cast<float>(presentationParameters.BackBufferHeight),
        0.0f,
        100.0f);

    // Drawing line
    const auto w = static_cast<float>(presentationParameters.BackBufferWidth);
    const auto h = static_cast<float>(presentationParameters.BackBufferHeight);
    primitiveBatch->Begin(commandList, projectionMatrix);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, 0.0f}, Vector2{w * 0.5f, 0.0f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{0.0f, -h * 0.5f}, Vector2{0.0f, h * 0.5f}, Color{221, 220, 218, 160}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, h * 0.25f}, Vector2{w * 0.5f, h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.5f, -h * 0.25f}, Vector2{w * 0.5f, -h * 0.25f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{-w * 0.25f, -h * 0.5f}, Vector2{-w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->DrawLine(Vector2{w * 0.25f, -h * 0.5f}, Vector2{w * 0.25f, h * 0.5f}, Color{221, 220, 218, 60}, 1.0f);
    primitiveBatch->End();

    spriteBatch->Begin(commandList, projectionMatrix);

    auto drawBeam = [&](std::vector<Vector2> const& points, float lineThickness, Color const& color) {
        for (std::size_t i = 1; i < points.size(); ++i) {
            POMDOG_ASSERT(i > 0);
            const auto& start = points[i - 1];
            const auto& end = points[i];
            SpriteLine spriteLine;
            spriteLine.Draw(
                *spriteBatch,
                texture,
                Rectangle{0, 0, 32, 64},
                Rectangle{32, 0, 1, 64},
                Rectangle{33, 0, 31, 64},
                start,
                end,
                lineThickness,
                color);
        }
    };

    for (const auto& beam : beamSystem->beams) {
        drawBeam(beam.Points, beam.Thickness * 0.02f, beam.Color);
    }

    spriteBatch->End();

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

} // namespace feature_showcase
