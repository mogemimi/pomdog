#include "game_main.h"
#include "pomdog/utility/cli_parser.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file_archive.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace quickstart {

GameMain::GameMain() = default;

std::unique_ptr<Error>
GameMain::initialize(const std::shared_ptr<GameHost>& gameHostIn, int argc, const char* const* argv)
{
    gameHost_ = gameHostIn;
    window_ = gameHostIn->getWindow();
    graphicsDevice_ = gameHostIn->getGraphicsDevice();
    clock_ = gameHostIn->getClock();
    commandQueue_ = gameHostIn->getCommandQueue();

    // NOTE: Parse command-line arguments for VFS configuration
    std::string assetsDir;
    std::string archiveFile;
    {
        CLIParser cli;
        cli.add(&assetsDir, "assets-dir", "path to the assets directory");
        cli.add(&archiveFile, "archive-file", "path to the archive file (without extension)");
        if (auto err = cli.parse(argc, argv); err != nullptr) {
            return errors::wrap(std::move(err), "failed to parse command-line arguments");
        }
    }

    if (archiveFile.empty()) {
        auto [resourceDir, resourceDirErr] = FileSystem::getResourceDirectoryPath();
        if (resourceDirErr != nullptr) {
            return errors::wrap(std::move(resourceDirErr), "failed to get resource directory path");
        }
        archiveFile = filepaths::join(resourceDir, "content.idx");
    }

    // NOTE: Initialize VFS
    {
        if (auto [ctx, err] = vfs::create(); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create VFS");
        }
        else {
            fs_ = std::move(ctx);
        }

        if (!archiveFile.empty()) {
            const auto replaceExtension = [](std::string_view filename, std::string_view newExtension) -> std::string {
                auto [base, ext] = filepaths::splitExtensionAsView(filename);
                auto baseStr = std::string(base);
                baseStr += newExtension;
                return baseStr;
            };

            auto [vol, volErr] = vfs::openArchiveFile(archiveFile, replaceExtension(archiveFile, ".pak"));
            if (volErr != nullptr) {
                return errors::wrap(std::move(volErr), "failed to open archive file");
            }
            if (auto mountErr = vfs::mount(fs_, "/assets", std::move(vol), {.readOnly = true, .hashKeyLookup = true}); mountErr != nullptr) {
                return errors::wrap(std::move(mountErr), "failed to mount archive");
            }
        }
        if (!assetsDir.empty()) {
            if (auto mountErr = vfs::mount(fs_, "/assets", assetsDir, {.readOnly = true, .overlayFS = true}); mountErr != nullptr) {
                return errors::wrap(std::move(mountErr), "failed to mount assets directory");
            }
        }
    }

    // NOTE: Display message in log console
    Log::Verbose("Hello, quickstart.");

    // NOTE: Set window name
    window_->setTitle("quickstart");

    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Load a PNG image as texture
    if (auto [texture, err] = loadTexture2D(fs_, graphicsDevice_, "/assets/pomdog.png"); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load texture");
    }
    else {
        texture_ = std::move(texture);
    }

    // NOTE: Create sampler state
    if (auto [sampler, err] = graphicsDevice_->createSamplerState(gpu::SamplerDesc::createPointClamp()); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create sampler state");
    }
    else {
        sampler_ = std::move(sampler);
    }

    {
        // NOTE: Create vertex buffer
        struct VertexCombined final {
            Vector3 Position;
            Vector2 TextureCoord;
        };

        std::array<VertexCombined, 4> verticesCombo = {{
            VertexCombined{Vector3{-1.0f, -1.0f, 0.0f}, Vector2{0.0f, 1.0f}},
            VertexCombined{Vector3{-1.0f, 1.0f, 0.0f}, Vector2{0.0f, 0.0f}},
            VertexCombined{Vector3{1.0f, 1.0f, 0.0f}, Vector2{1.0f, 0.0f}},
            VertexCombined{Vector3{1.0f, -1.0f, 0.0f}, Vector2{1.0f, 1.0f}},
        }};

        if (auto [vertexBuffer, err] = graphicsDevice_->createVertexBuffer(
                verticesCombo.data(),
                static_cast<u32>(verticesCombo.size()),
                sizeof(VertexCombined),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create vertex buffer");
        }
        else {
            vertexBuffer_ = std::move(vertexBuffer);
        }
    }
    {
        // NOTE: Create index buffer
        std::array<u16, 6> indices = {{0, 1, 2, 2, 3, 0}};

        if (auto [indexBuffer, err] = graphicsDevice_->createIndexBuffer(
                gpu::IndexFormat::UInt16,
                indices.data(),
                static_cast<u32>(indices.size()),
                gpu::BufferUsage::Immutable);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create index buffer");
        }
        else {
            indexBuffer_ = std::move(indexBuffer);
        }
    }
    {
        // NOTE: Create constant buffer
        if (auto [constantBuffer, err] = graphicsDevice_->createConstantBuffer(
                sizeof(MyShaderConstants),
                gpu::BufferUsage::Dynamic);
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to create constant buffer");
        }
        else {
            constantBuffer_ = std::move(constantBuffer);
        }
    }
    {
        // NOTE: For details, see 'struct VertexCombined' members
        gpu::InputLayoutDesc inputLayout = {};
        gpu::InputLayoutBuilder::addVertex(inputLayout,
            0, gpu::InputClassification::PerVertex, 20,
            {
                gpu::InputElementFormat::Float32x3,
                gpu::InputElementFormat::Float32x2,
            });

        std::shared_ptr<gpu::Shader> vertexShader;
        std::shared_ptr<gpu::Shader> pixelShader;

        if (auto [shader, err] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::VertexShader,
                "/assets/shaders",
                "simple_effect_vs",
                "SimpleEffectVS");
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to load vertex shader");
        }
        else {
            vertexShader = std::move(shader);
        }

        if (auto [shader, err] = loadShaderAutomagically(
                fs_,
                graphicsDevice_,
                gpu::ShaderPipelineStage::PixelShader,
                "/assets/shaders",
                "simple_effect_ps",
                "SimpleEffectPS");
            err != nullptr) {
            return errors::wrap(std::move(err), "failed to load pixel shader");
        }
        else {
            pixelShader = std::move(shader);
        }

        auto presentationParameters = graphicsDevice_->getPresentationParameters();

        gpu::PipelineDesc pipelineDesc = {};
        pipelineDesc.renderTargetViewFormats = {presentationParameters.backBufferFormat};
        pipelineDesc.depthStencilViewFormat = presentationParameters.depthStencilFormat;
        pipelineDesc.inputLayout = inputLayout;
        pipelineDesc.primitiveTopology = gpu::PrimitiveTopology::TriangleList;
        pipelineDesc.vertexShader = std::move(vertexShader);
        pipelineDesc.pixelShader = std::move(pixelShader);
        pipelineDesc.blendState = gpu::BlendDesc::createDefault();
        pipelineDesc.rasterizerState = gpu::RasterizerDesc::createDefault();
        pipelineDesc.depthStencilState = gpu::DepthStencilDesc::createDefault();

        // NOTE: Create pipeline state
        if (auto [pipelineState, err] = graphicsDevice_->createPipelineState(pipelineDesc); err != nullptr) {
            return errors::wrap(std::move(err), "failed to create pipeline state");
        }
        else {
            pipelineState_ = std::move(pipelineState);
        }
    }
    {
        auto updateShaderConstants = [this]([[maybe_unused]] int width, [[maybe_unused]] int height) {
            const auto presentationParameters = graphicsDevice_->getPresentationParameters();

            const auto viewMatrix = Matrix4x4::createIdentity();
            const auto projectionMatrix = Matrix4x4::createOrthographicLH(
                static_cast<float>(presentationParameters.backBufferWidth),
                static_cast<float>(presentationParameters.backBufferHeight),
                0.0f,
                100.0f);
            myShaderConstants_.ViewProjection = viewMatrix * projectionMatrix;
        };

        // NOTE: Initialize shader resources
        const auto bounds = window_->getClientBounds();
        updateShaderConstants(bounds.width, bounds.height);

        // NOTE: Connect to window resize event notification
        connect_(window_->clientSizeChanged, updateShaderConstants);
    }
    {
        // NOTE: Create timer
        timer_ = std::make_unique<Timer>(clock_);
        timer_->setInterval(std::chrono::milliseconds(500));

        // NOTE: Connect to timer event notification
        connect_(timer_->elapsed, [this] {
            // String formatting using pomdog::StringFormat
            auto title = pomdog::format(
                "quickstart {:3.0f} fps, {} frames",
                std::round(clock_->getFrameRate()),
                clock_->getFrameNumber());

            // NOTE: Set window title
            window_->setTitle(title);
        });
    }

    return nullptr;
}

void GameMain::update()
{
    auto totalTime = static_cast<float>(clock_->getTotalGameTime().count());

    auto rotate = Matrix4x4::createRotationZ(std::cos(totalTime));
    auto scale = Matrix4x4::createScale(Vector3{
        static_cast<float>(texture_->getWidth()),
        static_cast<float>(texture_->getHeight()),
        1.0f});

    myShaderConstants_.Model = scale * rotate;
}

void GameMain::draw()
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

    // Reset graphics command list
    commandList_->reset();

    // Update constant buffer
    constantBuffer_->setData(0, gpu::makeByteSpan(myShaderConstants_));

    // Create graphics commands
    commandList_->beginRenderPass(std::move(pass));
    commandList_->setPipelineState(pipelineState_);
    commandList_->setConstantBuffer(0, constantBuffer_);
    commandList_->setSamplerState(0, sampler_);
    commandList_->setTexture(0, texture_);
    commandList_->setVertexBuffer(0, vertexBuffer_);
    commandList_->setIndexBuffer(indexBuffer_);
    commandList_->drawIndexed(indexBuffer_->getIndexCount(), 0);
    commandList_->endRenderPass();
    commandList_->close();

    // Submit graphics command list for execution
    commandQueue_->reset();
    commandQueue_->pushBackCommandList(commandList_);
    commandQueue_->executeCommandLists();
    commandQueue_->present();
}

} // namespace quickstart
