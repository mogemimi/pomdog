//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "InGameEditor.hpp"
#include "detail/SpriteDrawingContext.hpp"
#include "Pomdog.Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog.Experimental/Graphics/SpriteFontLoader.hpp"
#include "Pomdog.Experimental/UI/UIView.hpp"
#include "Pomdog/Graphics/detail/BuiltinShaderPool.hpp"
#include "Pomdog/Graphics/detail/ShaderBytecode.hpp"

namespace Pomdog {
namespace SceneEditor {
namespace {

// Built-in shaders
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/SpriteBatch_VS.inc.h"
#include "Pomdog.Experimental/Graphics/Shaders/GLSL.Embedded/Sprite_DistanceField_PS.inc.h"

struct BuiltinEffectSpriteBatchDistanceFieldTrait {
	static std::shared_ptr<EffectPass> Create(GraphicsDevice & graphicsDevice)
	{
		using Details::ShaderBytecode;
		ShaderBytecode vertexShaderCode = {Builtin_GLSL_SpriteBatch_VS, std::strlen(Builtin_GLSL_SpriteBatch_VS)};
		ShaderBytecode pixelShaderCode = {Builtin_GLSL_Sprite_DistanceField_PS, std::strlen(Builtin_GLSL_Sprite_DistanceField_PS)};
		return std::make_shared<EffectPass>(graphicsDevice, vertexShaderCode, pixelShaderCode);
	}
};

}// unnamed namespace
//-----------------------------------------------------------------------
InGameEditor::InGameEditor(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	auto graphicsContext = gameHost->GraphicsContext();
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();
	auto window = gameHost->Window();
	
	{
		spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice);
		//spriteFont = assets->Load<SpriteFont>("BitmapFonts/UbuntuMono-Regular.fnt");
		spriteFont = assets->Load<SpriteFont>("BitmapFonts/Ubuntu-Regular.fnt");
		distanceFieldEffect = graphicsDevice->ShaderPool().Create<BuiltinEffectSpriteBatchDistanceFieldTrait>(*graphicsDevice);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *distanceFieldEffect);
		spriteBatchDistanceField = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice,
			distanceFieldEffect, constantBuffers);
	}
	{
		pomdogTexture = assets->Load<Texture2D>("pomdog.png");
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
	auto clock = gameHost->Clock();
	auto mouse = gameHost->Mouse();
	{
		hierarchy.Touch(mouse->State());
		hierarchy.UpdateAnimation(clock->FrameDuration());
	}
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
		UI::SpriteDrawingContext drawingContext(*spriteBatch, *spriteBatchDistanceField, distanceFieldEffect, constantBuffers, *spriteFont, pomdogTexture);
		hierarchy.Draw(drawingContext);
		spriteBatch->End();
	}
	
	graphicsContext.SetDepthStencilState(depthStencilStateOld);
	graphicsContext.SetBlendState(blendStateOld);
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
