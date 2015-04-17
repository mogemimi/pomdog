// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "InGameEditor.hpp"
#include "detail/SpriteDrawingContext.hpp"
#include "Pomdog.Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFontLoader.hpp"
#include "Pomdog.Experimental/UI/UIView.hpp"
#include "Pomdog/Content/AssetBuilders/PipelineStateBuilder.hpp"
#include "Pomdog/Content/AssetBuilders/ShaderBuilder.hpp"
#include "Pomdog/Graphics/InputLayoutHelper.hpp"
#include "Pomdog/Graphics/Shader.hpp"

namespace Pomdog {
namespace SceneEditor {
namespace {

// Built-in shaders
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/SpriteBatch_VS.inc.hpp"
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/Sprite_DistanceField_PS.inc.hpp"
#include "Pomdog.Experimental/Graphics/Shaders/HLSL.Embedded/SpriteBatch_VS.inc.hpp"
#include "Pomdog.Experimental/Graphics/Shaders/HLSL.Embedded/SpriteDistanceField_PS.inc.hpp"

}// unnamed namespace
//-----------------------------------------------------------------------
InGameEditor::InGameEditor(std::shared_ptr<GameHost> const& gameHostIn)
    : hierarchy(gameHostIn->Window())
    , gameHost(gameHostIn)
{
    auto graphicsContext = gameHost->GraphicsContext();
    auto graphicsDevice = gameHost->GraphicsDevice();
    auto assets = gameHost->AssetManager();
    auto window = gameHost->Window();

    {
        spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
        //spriteFont = SpriteFontLoader::Load(*assets, "BitmapFonts/UbuntuMono-Regular.fnt");
        spriteFont = SpriteFontLoader::Load(*assets, "BitmapFonts/Ubuntu-Regular.fnt");

        auto inputLayout = InputLayoutHelper{}
            .AddInputSlot()
            .Float4()
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float4().Float4().Float4().Float4();

        auto vertexShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::VertexShader)
            .SetGLSL(Builtin_GLSL_SpriteBatch_VS, std::strlen(Builtin_GLSL_SpriteBatch_VS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteBatch_VS, sizeof(BuiltinHLSL_SpriteBatch_VS));

        auto pixelShader = assets->CreateBuilder<Shader>()
            .SetPipelineStage(ShaderCompilers::ShaderPipelineStage::PixelShader)
            .SetGLSL(Builtin_GLSL_Sprite_DistanceField_PS, std::strlen(Builtin_GLSL_Sprite_DistanceField_PS))
            .SetHLSLPrecompiled(BuiltinHLSL_SpriteDistanceField_PS, sizeof(BuiltinHLSL_SpriteDistanceField_PS));

        auto builder = assets->CreateBuilder<PipelineState>();
        distanceFieldEffect = builder
            .SetVertexShader(vertexShader.Build())
            .SetPixelShader(pixelShader.Build())
            .SetInputLayout(inputLayout.CreateInputLayout())
            .SetBlendState(BlendDescription::CreateNonPremultiplied())
            .SetDepthStencilState(DepthStencilDescription::CreateNone())
            .Build();

        constantBuffers = builder.CreateConstantBuffers(distanceFieldEffect);

        spriteBatchDistanceField = std::make_unique<SpriteBatch>(
            graphicsContext, graphicsDevice,
            distanceFieldEffect, constantBuffers);
    }
    {
        blankTexture = std::make_shared<Texture2D>(graphicsDevice,
            1, 1, false, SurfaceFormat::R8G8B8A8_UNorm);
        std::array<std::uint32_t, 1> pixelData = {0xffffffff};
        blankTexture->SetData(pixelData.data());
    }

    clientSizeChangedConnection = window->ClientSizeChanged.Connect([this](int width, int height) {
        hierarchy.RenderSizeChanged(width, height);
    });
}
//-----------------------------------------------------------------------
InGameEditor::~InGameEditor() = default;
//-----------------------------------------------------------------------
void InGameEditor::AddView(std::shared_ptr<UI::UIView> const& view)
{
    POMDOG_ASSERT(view->Parent().expired());
    hierarchy.AddChild(view);
}
//-----------------------------------------------------------------------
void InGameEditor::Update()
{
    if (auto mouse = gameHost->Mouse()) {
        hierarchy.Touch(mouse->GetState());
    }

    auto clock = gameHost->Clock();
    hierarchy.UpdateAnimation(clock->FrameDuration());
}
//-----------------------------------------------------------------------
void InGameEditor::DrawGUI(GraphicsContext & graphicsContext)
{
    POMDOG_ASSERT(spriteBatch);
    spriteBatch->Begin(SpriteSortMode::BackToFront);
    UI::SpriteDrawingContext drawingContext(
        *spriteBatch, *spriteBatchDistanceField, distanceFieldEffect,
        constantBuffers, *spriteFont, blankTexture);
    hierarchy.Draw(drawingContext);
    spriteBatch->End();
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
