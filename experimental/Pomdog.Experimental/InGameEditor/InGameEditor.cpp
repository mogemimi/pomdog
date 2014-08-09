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
	static std::shared_ptr<EffectPass> Create(std::shared_ptr<GraphicsDevice> const& graphicsDevice)
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
	, lineBatch(gameHost->GraphicsContext(), gameHost->GraphicsDevice())
{
	auto graphicsContext = gameHost->GraphicsContext();
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();
	auto window = gameHost->Window();
	
	{
		backgroundPlane = std::make_unique<SceneEditor::GradientPlane>(gameHost);
	}
	{
		spriteBatch = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice);
		//spriteFont = assets->Load<SpriteFont>("BitmapFonts/UbuntuMono-Regular.fnt");
		spriteFont = assets->Load<SpriteFont>("BitmapFonts/Ubuntu-Regular.fnt");
		distanceFieldEffect = graphicsDevice->ShaderPool().Create<BuiltinEffectSpriteBatchDistanceFieldTrait>(graphicsDevice);
		constantBuffers = std::make_shared<ConstantBufferBinding>(graphicsDevice, *distanceFieldEffect);
		spriteBatchDistanceField = std::make_unique<SpriteBatch>(graphicsContext, graphicsDevice,
			distanceFieldEffect, constantBuffers);
	}
	{
		primitiveAxes = std::make_unique<SceneEditor::PrimitiveAxes>(
			editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ);
		primitiveGrid = std::make_unique<SceneEditor::PrimitiveGrid>(
			editorColorScheme.GuideLine, editorColorScheme.Grid);
	}
	{
		pomdogTexture = assets->Load<Texture2D>("pomdog.png");
		depthStencilState = DepthStencilState::CreateNone(graphicsDevice);
	}

	clientSizeChangedConnection = window->ClientSizeChanged.Connect([this] {
		auto gameWindow = gameHost->Window();
		auto bounds = gameWindow->ClientBounds();
		hierarchy.RenderSizeChanged(bounds.Width, bounds.Height);
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
void InGameEditor::SetViewProjection(Matrix4x4 const& viewProjectionIn)
{
	viewProjectionMatrix = viewProjectionIn;
}
//-----------------------------------------------------------------------
void InGameEditor::DrawGrids()
{
	lineBatch.Begin(viewProjectionMatrix);
	{
		POMDOG_ASSERT(primitiveGrid);
		primitiveGrid->Draw(lineBatch);

		POMDOG_ASSERT(primitiveAxes);
		primitiveAxes->Draw(lineBatch);
	}
	lineBatch.End();
}
//-----------------------------------------------------------------------
void InGameEditor::DrawGUI()
{
	POMDOG_ASSERT(spriteBatch);
	{
		spriteBatch->Begin(SpriteSortMode::BackToFront);
		UI::SpriteDrawingContext drawingContext(*spriteBatch, *spriteBatchDistanceField, distanceFieldEffect, constantBuffers, *spriteFont, pomdogTexture);
		hierarchy.Draw(drawingContext);
		spriteBatch->End();
	}
}
//-----------------------------------------------------------------------
void InGameEditor::BeginDraw(GraphicsContext & graphicsContext)
{
	auto oldDepthStencilState = graphicsContext.GetDepthStencilState();
	graphicsContext.SetDepthStencilState(depthStencilState);
	
	backgroundPlane->Draw();
	DrawGrids();
	
	graphicsContext.SetDepthStencilState(oldDepthStencilState);
}
//-----------------------------------------------------------------------
void InGameEditor::EndDraw(GraphicsContext & graphicsContext)
{
	auto oldDepthStencilState = graphicsContext.GetDepthStencilState();
	graphicsContext.SetDepthStencilState(depthStencilState);
	
	DrawGUI();
	
	graphicsContext.SetDepthStencilState(oldDepthStencilState);
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
