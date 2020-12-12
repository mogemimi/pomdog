#include "GLTFModelTest.hpp"
#include <Pomdog/Experimental/GLTF/GLTF.hpp>
#include <Pomdog/Experimental/GLTF/GLTFLoader.hpp>
#include <Pomdog/Experimental/Graphics/BasicEffect.hpp>
#include <cmath>

namespace FeatureShowcase {

GLTFModelTest::GLTFModelTest(const std::shared_ptr<GameHost>& gameHostIn)
    : gameHost(gameHostIn)
    , graphicsDevice(gameHostIn->GetGraphicsDevice())
    , commandQueue(gameHostIn->GetGraphicsCommandQueue())
{
}

std::shared_ptr<Error> GLTFModelTest::Initialize()
{
    auto assets = gameHost->GetAssetManager();
    auto clock = gameHost->GetClock();

    std::shared_ptr<Error> err;

    // NOTE: Create graphics command list
    std::tie(commandList, err) = graphicsDevice->CreateGraphicsCommandList();
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to create graphics command list");
    }

    // NOTE: Load texture from image file
    std::tie(texture, err) = assets->Load<Texture2D>("Textures/pomdog.png");
    if (err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to load texture");
    }

    // NOTE: Load model from glTF binary file.
    auto [glb, glbErr] = GLTF::Open(assets->GetAssetPath("GLB/F15.glb"));
    if (glbErr != nullptr) {
        return Errors::Wrap(std::move(glbErr), "failed to load glTF binary");
    }

    POMDOG_ASSERT(!glb->Meshes.empty());
    POMDOG_ASSERT(!glb->Meshes[0].Primitives.empty());
    POMDOG_ASSERT(glb->Meshes[0].Primitives[0].Indices != std::nullopt);
    const auto positionAccessor = glb->Meshes[0].Primitives[0].Attributes["POSITION"];
    const auto normalAccessor = glb->Meshes[0].Primitives[0].Attributes["NORMAL"];
    const auto texcoordAccessor = glb->Meshes[0].Primitives[0].Attributes["TEXCOORD_0"];
    const auto indexAccessor = *glb->Meshes[0].Primitives[0].Indices;

    POMDOG_ASSERT(glb->Accessors.size() == 4);
    POMDOG_ASSERT(glb->Accessors[0].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[0].Type == GLTF::AccessorType::Vec3);
    POMDOG_ASSERT(glb->Accessors[0].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[1].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[1].Type == GLTF::AccessorType::Vec3);
    POMDOG_ASSERT(glb->Accessors[1].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[2].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[2].Type == GLTF::AccessorType::Vec2);
    POMDOG_ASSERT(glb->Accessors[2].ComponentType == GLTF::ComponentType::Float);
    POMDOG_ASSERT(glb->Accessors[3].BufferView != std::nullopt);
    POMDOG_ASSERT(glb->Accessors[3].Type == GLTF::AccessorType::Scalar);
    POMDOG_ASSERT(glb->Accessors[3].ComponentType == GLTF::ComponentType::Ushort);

    {
        using VertexCombined = BasicEffect::VertexPositionNormalTexture;

        // NOTE: Create vertex buffer
        std::vector<VertexCombined> verticesCombo;

        verticesCombo.resize(glb->Accessors[positionAccessor].Count);

        {
            auto& accessor = glb->Accessors[positionAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].Position = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[normalAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].Normal = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[texcoordAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector2*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].TextureCoordinate = vertexData[i];
            }
        }

        std::tie(vertexBuffer1, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        using VertexCombined = BasicEffect::VertexPositionColor;

        // NOTE: Create vertex buffer
        std::vector<VertexCombined> verticesCombo;

        verticesCombo.resize(glb->Accessors[positionAccessor].Count);

        {
            auto& accessor = glb->Accessors[positionAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].Position = vertexData[i];
            }
        }
        {
            auto& accessor = glb->Accessors[normalAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto vertexData = reinterpret_cast<const Vector3*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                verticesCombo[i].Color = Vector4{vertexData[i], 1.0f};
            }
        }

        std::tie(vertexBuffer2, err) = graphicsDevice->CreateVertexBuffer(
            verticesCombo.data(),
            verticesCombo.size(),
            sizeof(VertexCombined),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create vertex buffer");
        }
    }
    {
        // NOTE: Create index buffer
        std::vector<std::uint16_t> indices;

        indices.resize(glb->Accessors[indexAccessor].Count);

        {
            auto& accessor = glb->Accessors[indexAccessor];
            auto& bufferView = glb->BufferViews[*accessor.BufferView];
            auto& buffer = glb->Buffers[bufferView.Buffer];
            auto indexData = reinterpret_cast<const std::uint16_t*>(buffer.Data.data() + bufferView.ByteOffset);

            for (std::uint32_t i = 0; i < accessor.Count; i++) {
                indices[i] = indexData[i];
            }
        }

        std::tie(indexBuffer, err) = graphicsDevice->CreateIndexBuffer(
            IndexElementSize::SixteenBits,
            indices.data(),
            indices.size(),
            BufferUsage::Immutable);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create index buffer");
        }
    }
    {
        // NOTE: Create constant buffer
        std::tie(modelConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::ModelConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create constant buffer");
        }

        std::tie(worldConstantBuffer, err) = graphicsDevice->CreateConstantBuffer(
            sizeof(BasicEffect::WorldConstantBuffer),
            BufferUsage::Dynamic);

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create constant buffer");
        }
    }
    {
        // NOTE: Create sampler state
        std::tie(sampler, err) = graphicsDevice->CreateSamplerState(
            SamplerDescription::CreateLinearClamp());

        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create sampler state");
        }
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = true;
        effectDesc.TextureEnabled = true;
        effectDesc.VertexColorEnabled = false;

        // NOTE: Create pipeline state
        std::tie(pipelineState1, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }
    {
        auto presentationParameters = graphicsDevice->GetPresentationParameters();

        BasicEffect::BasicEffectDescription effectDesc;
        effectDesc.LightingEnabled = false;
        effectDesc.TextureEnabled = false;
        effectDesc.VertexColorEnabled = true;

        // NOTE: Create pipeline state
        std::tie(pipelineState2, err) = BasicEffect::CreateBasicEffect(*assets, effectDesc)
            .SetRenderTargetViewFormat(presentationParameters.BackBufferFormat)
            .SetDepthStencilViewFormat(presentationParameters.DepthStencilFormat)
            .SetPrimitiveTopology(PrimitiveTopology::TriangleList)
            .SetDepthStencilState(DepthStencilDescription::CreateDefault())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetRasterizerState(RasterizerDescription::CreateDefault())
            .Build();
        if (err != nullptr) {
            return Errors::Wrap(std::move(err), "failed to create pipeline state");
        }
    }

    return nullptr;
}

void GLTFModelTest::Update()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    constexpr float rotateSpeed = 0.5f;

    auto projectionMatrix = Matrix4x4::CreatePerspectiveFieldOfViewLH(
        Math::ToRadians(45.0f),
        static_cast<float>(presentationParameters.BackBufferWidth) / presentationParameters.BackBufferHeight,
        0.01f,
        1000.0f);

    auto cameraPosition = Vector3{0.0f, 8.0f, -2.0f};
    auto cameraDirection = Vector3{0.0f, -1.0f, 1.0f};
    auto viewMatrix = Matrix4x4::CreateLookAtLH(cameraPosition, cameraPosition + cameraDirection, Vector3::UnitY);

    auto lightDirection = Vector3::Normalize(Vector3{-0.5f, -1.0f, 0.5f});

    // NOTE: Update constant buffer for world
    BasicEffect::WorldConstantBuffer worldConstants;
    worldConstants.ViewProjection = viewMatrix * projectionMatrix;
    worldConstants.View = viewMatrix;
    worldConstants.Projection = projectionMatrix;
    worldConstants.InverseView = Matrix4x4::Invert(viewMatrix);
    worldConstants.LightDirection = Vector4{lightDirection, 0.0f};
    worldConstantBuffer->SetValue(worldConstants);

    auto time = static_cast<float>(gameHost->GetClock()->GetTotalGameTime().count());
    auto rotateY = Math::TwoPi<float> * rotateSpeed * time;

    auto mouse = gameHost->GetMouse()->GetState();
    if (mouse.LeftButton == ButtonState::Pressed) {
        rotateY = -Math::TwoPi<float> * (static_cast<float>(mouse.Position.X) / static_cast<float>(presentationParameters.BackBufferWidth));
    }

    auto modelMatrix = Matrix4x4::CreateTranslation(Vector3{-0.5f, -0.5f, -0.5f})
        * Matrix4x4::CreateScale(1.0f + std::cos(time * 5.0f) * 0.1f)
        * Matrix4x4::CreateRotationY(rotateY)
        * Matrix4x4::CreateTranslation(Vector3{0.0f, 0.0f, 6.0f});

    constexpr float metalness = 0.1f;

    // NOTE: Update constant buffer for model
    BasicEffect::ModelConstantBuffer modelConstants;
    modelConstants.Model = modelMatrix;
    modelConstants.Material = Vector4{metalness, 0.0f, 0.0f, 0.0f};
    modelConstants.Color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
    modelConstantBuffer->SetValue(modelConstants);
}

void GLTFModelTest::Draw()
{
    auto presentationParameters = graphicsDevice->GetPresentationParameters();

    Viewport viewport = {0, 0, presentationParameters.BackBufferWidth, presentationParameters.BackBufferHeight};
    RenderPass pass;
    pass.RenderTargets[0] = {nullptr, Color::CornflowerBlue.ToVector4()};
    pass.ClearDepth = 1.0f;
    pass.ClearStencil = 0;
    pass.Viewport = viewport;
    pass.ScissorRect = viewport.GetBounds();

    auto mouse = gameHost->GetMouse()->GetState();

    commandList->Reset();
    commandList->SetRenderPass(std::move(pass));
    commandList->SetConstantBuffer(0, modelConstantBuffer);
    commandList->SetConstantBuffer(1, worldConstantBuffer);
    commandList->SetSamplerState(0, sampler);
    commandList->SetTexture(0, texture);
    if (mouse.RightButton == ButtonState::Pressed) {
        commandList->SetVertexBuffer(0, vertexBuffer2);
        commandList->SetPipelineState(pipelineState2);
    }
    else {
        commandList->SetVertexBuffer(0, vertexBuffer1);
        commandList->SetPipelineState(pipelineState1);
    }
    commandList->DrawIndexed(indexBuffer, indexBuffer->GetIndexCount(), 0);
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
