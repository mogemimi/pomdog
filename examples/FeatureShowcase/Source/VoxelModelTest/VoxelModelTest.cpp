#include "VoxelModelTest.hpp"
#include <Pomdog/Experimental/Image/GifImage.hpp>
#include <Pomdog/Experimental/Image/GifImageLoader.hpp>
#include <Pomdog/Experimental/TexturePacker/TextureAtlasGenerator.hpp>
#include <Pomdog/Experimental/Tween/EasingHelper.hpp>
#include <random>

namespace FeatureShowcase {

VoxelModelTest::VoxelModelTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

void VoxelModelTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();
    commandList = std::make_shared<GraphicsCommandList>(*graphicsDevice);
    primitiveBatch = std::make_shared<PrimitiveBatch>(graphicsDevice, DepthStencilDescription::CreateDefault(), *assets);

    if (auto[res, err] = assets->Load<MagicaVoxel::VoxModel>("VoxelModels/MaidChan.vox"); err != nullptr) {
        Log::Verbose("failed to load texture: " + err->ToString());
    }
    else {
        voxelModel = std::move(res);
    }
}

void VoxelModelTest::Update()
{
}

void VoxelModelTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets.emplace_back(nullptr, Color::CornflowerBlue.ToVector4());
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    constexpr float orthographicSize = 16.0f;
    constexpr float rotateSpeed = 0.7f;

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.BackBufferWidth / orthographicSize,
        presentationParameters.BackBufferHeight / orthographicSize,
        -100.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::CreateLookAtLH(Vector3::Zero, Vector3{2.0f, -3.0f, -10.0f}, Vector3::UnitY);
    auto rotateX = Matrix4x4::CreateRotationX(Math::PiOver2<float> * 3.0f);
    auto rotateY = Matrix4x4::CreateRotationY(Math::TwoPi<float> * rotateSpeed * static_cast<float>(gameHost->GetClock()->GetTotalGameTime().count()));

    primitiveBatch->Begin(commandList, rotateX * rotateY * viewMatrix * projectionMatrix);

    const auto centerOffset = 0.5f * Vector3{
        static_cast<float>(voxelModel->X),
        static_cast<float>(voxelModel->Y),
        static_cast<float>(voxelModel->Z)};

    // Drawing voxels
    for (auto& v : voxelModel->Voxels) {
        auto pos = Vector3{static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z)} - centerOffset;
        auto color = voxelModel->ColorPalette[v.ColorIndex];
        color.A = 255;
        primitiveBatch->DrawBox(pos, Vector3{1.0f, 1.0f, 1.0f}, color);
    }

    primitiveBatch->End();

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
