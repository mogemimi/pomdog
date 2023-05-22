#include "voxel_model_test.h"

namespace feature_showcase {

VoxelModelTest::VoxelModelTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetCommandQueue())
{
}

std::unique_ptr<Error> VoxelModelTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateCommandList();
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Create PrimitiveBatch effect
    primitiveBatch = std::make_shared<PrimitiveBatch>(
        graphicsDevice,
        gpu::DepthStencilDescriptor::CreateDefault(),
        std::nullopt,
        *assets);

    // NOTE: Load MagicaVoxel model
    std::tie(voxelModel, err) = assets->Load<magicavoxel::VoxModel>("VoxelModels/MaidChan.vox");
    if (err != nullptr) {
        return errors::Wrap(std::move(err), "failed to load texture");
    }

    return nullptr;
}

void VoxelModelTest::Update()
{
}

void VoxelModelTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::CornflowerBlue().ToVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = std::uint8_t(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.GetBounds();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));

    constexpr float orthographicSize = 16.0f;
    constexpr float rotateSpeed = 0.7f;

    auto projectionMatrix = Matrix4x4::CreateOrthographicLH(
        presentationParameters.backBufferWidth / orthographicSize,
        presentationParameters.backBufferHeight / orthographicSize,
        -100.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::CreateLookAtLH(Vector3::Zero(), Vector3{2.0f, -3.0f, -10.0f}, Vector3::UnitY());
    auto rotateX = Matrix4x4::CreateRotationX(math::PiOver2<float> * 3.0f);
    auto rotateY = Matrix4x4::CreateRotationY(math::TwoPi<float> * rotateSpeed * static_cast<float>(gameHost->GetClock()->GetTotalGameTime().count()));

    primitiveBatch->Begin(commandList, rotateX * rotateY * viewMatrix * projectionMatrix);

    const auto centerOffset = 0.5f * Vector3{
        static_cast<float>(voxelModel->X),
        static_cast<float>(voxelModel->Y),
        static_cast<float>(voxelModel->Z)};

    // Drawing voxels
    for (auto& v : voxelModel->Voxels) {
        auto pos = Vector3{static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z)} - centerOffset;
        auto color = voxelModel->ColorPalette[v.ColorIndex];
        color.a = 255;
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

} // namespace feature_showcase
