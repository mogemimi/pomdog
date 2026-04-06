#include "voxel_model_test.h"
#include "pomdog/experimental/magicavoxel/vox_model_loader.h"

namespace feature_showcase {

VoxelModelTest::VoxelModelTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
VoxelModelTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/, int /*argc*/, const char* const* /*argv*/)
{
    auto clock = gameHost_->getClock();

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Create PrimitiveBatch effect
    if (auto [p, err] = createPrimitivePipeline(
            fs_,
            graphicsDevice_,
            gpu::DepthStencilDesc::createDefault(),
            std::nullopt);
        err != nullptr) {
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

    // NOTE: Load MagicaVoxel model
    if (auto [voxelModel, err] = loadVoxModel(fs_, "/assets/voxel_models/maidchan.vox"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        voxelModel_ = std::move(voxelModel);
    }

    return nullptr;
}

void VoxelModelTest::update()
{
}

void VoxelModelTest::draw()
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

    constexpr float orthographicSize = 16.0f;
    constexpr float rotateSpeed = 0.7f;

    auto projectionMatrix = Matrix4x4::createOrthographicLH(
        presentationParameters.backBufferWidth / orthographicSize,
        presentationParameters.backBufferHeight / orthographicSize,
        -100.0f,
        100.0f);

    auto viewMatrix = Matrix4x4::createLookAtLH(Vector3::createZero(), Vector3{2.0f, -3.0f, -10.0f}, Vector3::createUnitY());
    auto rotateX = Matrix4x4::createRotationX(math::PiOver2<float> * 3.0f);
    auto rotateY = Matrix4x4::createRotationY(math::TwoPi<float> * rotateSpeed * static_cast<float>(gameHost_->getClock()->getTotalGameTime().count()));

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(rotateX * rotateY * viewMatrix * projectionMatrix);

    const auto centerOffset = 0.5f * Vector3{
                                         static_cast<f32>(voxelModel_->X),
                                         static_cast<f32>(voxelModel_->Y),
                                         static_cast<f32>(voxelModel_->Z)};

    // Drawing voxels
    for (auto& v : voxelModel_->Voxels) {
        auto pos = Vector3{static_cast<f32>(v.X), static_cast<f32>(v.Y), static_cast<f32>(v.Z)} - centerOffset;
        auto color = voxelModel_->ColorPalette[v.ColorIndex];
        color.a = 255;
        primitiveBatch_->drawBox(pos, Vector3{1.0f, 1.0f, 1.0f}, color);
    }

    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

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
