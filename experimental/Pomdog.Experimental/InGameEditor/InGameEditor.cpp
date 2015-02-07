//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "InGameEditor.hpp"
#include "detail/SpriteDrawingContext.hpp"
#include "Pomdog.Experimental/Graphics/EffectPassBuilder.hpp"
#include "Pomdog.Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFontLoader.hpp"
#include "Pomdog.Experimental/UI/UIView.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"

namespace Pomdog {
namespace SceneEditor {
namespace {

// Built-in shaders
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/SpriteBatch_VS.inc.h"
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/Sprite_DistanceField_PS.inc.h"

struct BuiltinEffectSpriteBatchDistanceFieldTrait {
	static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
	{
		using PositionTextureCoord = CustomVertex<Vector4>;
		using SpriteInfoVertex = CustomVertex<Vector4, Vector4, Vector4, Vector4>;

		auto declartation = PositionTextureCoord::Declaration();

		auto effectPass = EffectPassBuilder(graphicsDevice)
			.VertexShaderGLSL(Builtin_GLSL_SpriteBatch_VS, std::strlen(Builtin_GLSL_SpriteBatch_VS))
			.PixelShaderGLSL(Builtin_GLSL_Sprite_DistanceField_PS, std::strlen(Builtin_GLSL_Sprite_DistanceField_PS))
			.InputElements(std::initializer_list<VertexBufferBinding>{
				{declartation, 0, 0},
				{SpriteInfoVertex::Declaration(), declartation.StrideBytes(), 1}
			})
			.Create();
		return std::move(effectPass);
	}
};

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
		spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice);
		//spriteFont = SpriteFontLoader::Load(*assets, "BitmapFonts/UbuntuMono-Regular.fnt");
		spriteFont = SpriteFontLoader::Load(*assets, "BitmapFonts/Ubuntu-Regular.fnt");
		distanceFieldEffect = graphicsDevice->ShaderPool().Create<BuiltinEffectSpriteBatchDistanceFieldTrait>(*graphicsDevice);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *distanceFieldEffect);
		spriteBatchDistanceField = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice,
			distanceFieldEffect, constantBuffers);
	}
	{
		blankTexture = std::make_shared<Texture2D>(graphicsDevice,
			1, 1, false, SurfaceFormat::R8G8B8A8_UNorm);
		std::array<std::uint32_t, 1> pixelData = {0xffffffff};
		blankTexture->SetData(pixelData.data());
	}
	{
		depthStencilState = DepthStencilState::CreateNone(graphicsDevice);
		blendState = BlendState::CreateNonPremultiplied(graphicsDevice);
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
	auto depthStencilStateOld = graphicsContext.GetDepthStencilState();
	graphicsContext.SetDepthStencilState(depthStencilState);

	auto blendStateOld = graphicsContext.GetBlendState();
	graphicsContext.SetBlendState(blendState);

	{
		POMDOG_ASSERT(spriteBatch);
		spriteBatch->Begin(SpriteSortMode::BackToFront);
		UI::SpriteDrawingContext drawingContext(*spriteBatch, *spriteBatchDistanceField, distanceFieldEffect, constantBuffers, *spriteFont, blankTexture);
		hierarchy.Draw(drawingContext);
		spriteBatch->End();
	}

	graphicsContext.SetDepthStencilState(depthStencilStateOld);
	graphicsContext.SetBlendState(blendStateOld);
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
