#include "voxel_model_test.h"

namespace feature_showcase {

VoxelModelTest::VoxelModelTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->getGraphicsDevice())
    , commandQueue(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error> VoxelModelTest::initialize()
{
    auto assets = gameHost->getAssetManager();
    auto clock = gameHost->getClock();

    std::unique_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->createCommandList();
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Create PrimitiveBatch effect
    primitiveBatch = std::make_shared<PrimitiveBatch>(
        graphicsDevice,
        gpu::DepthStencilDescriptor::createDefault(),
        std::nullopt,
        *assets);

    // NOTE: Load MagicaVoxel model
    std::tie(voxelModel, err) = assets->load<magicavoxel::VoxModel>("VoxelModels/MaidChan.vox");
    if (err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }

    return nullptr;
}

void VoxelModelTest::update()
{
}

void VoxelModelTest::draw()
{
    auto presentationParameters = graphicsDevice->getPresentationParameters();

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

    constexpr float orthographicSize = 16.0f;
    constexpr float rotateSpeed = 0.7f;

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        presentationParameters.backBufferWidth / orthographicSize,
        presentationParameters.backBufferHeight / orthographicSize,
        -100.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::createLookAtLH(Vector3::createZero(), Vector3{2.0f, -3.0f, -10.0f}, Vector3::createUnitY());
    auto rotateX = Matrix4x4::createRotationX(math::PiOver2<float> * 3.0f);
    auto rotateY = Matrix4x4::createRotationY(math::TwoPi<float> * rotateSpeed * static_cast<float>(gameHost->getClock()->getTotalGameTime().count()));

    primitiveBatch->begin(commandList, rotateX * rotateY * viewMatrix * projectionMatrix);

    const auto centerOffset = 0.5f * Vector3{
        static_cast<float>(voxelModel->X),
        static_cast<float>(voxelModel->Y),
        static_cast<float>(voxelModel->Z)};

    // Drawing voxels
    for (auto& v : voxelModel->Voxels) {
        auto pos = Vector3{static_cast<float>(v.X), static_cast<float>(v.Y), static_cast<float>(v.Z)} - centerOffset;
        auto color = voxelModel->ColorPalette[v.ColorIndex];
        color.a = 255;
        primitiveBatch->drawBox(pos, Vector3{1.0f, 1.0f, 1.0f}, color);
    }

    primitiveBatch->end();

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
