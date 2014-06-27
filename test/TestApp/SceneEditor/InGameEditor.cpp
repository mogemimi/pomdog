//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "InGameEditor.hpp"
#include <Pomdog/Utility/MakeUnique.hpp>
#include "../Graphics/SpriteBatch.hpp"
#include "../Graphics/SpriteFont.hpp"
#include "../Graphics/SpriteFontLoader.hpp"
#include "../SpriteDrawingContext.hpp"

namespace Pomdog {
namespace SceneEditor {
//-----------------------------------------------------------------------
InGameEditor::InGameEditor(std::shared_ptr<GameHost> const& gameHostIn)
	: gameHost(gameHostIn)
{
	auto graphicsContext = gameHost->GraphicsContext();
	auto graphicsDevice = gameHost->GraphicsDevice();
	auto assets = gameHost->AssetManager();
	auto window = gameHost->Window();
	
	{
		backgroundPlane = MakeUnique<SceneEditor::GradientPlane>(gameHost);
	}
	{
		spriteBatch = MakeUnique<SpriteBatch>(graphicsContext, graphicsDevice, *assets);
		spriteFont = assets->Load<SpriteFont>("BitmapFonts/UbuntuMono-Regular.fnt");
		distanceFieldEffect = assets->Load<EffectPass>("Effects/SpriteBatchDistanceField");
		spriteBatchDistanceField = MakeUnique<SpriteBatch>(graphicsContext, graphicsDevice, *assets,
			distanceFieldEffect);
	}
	{
		primitiveAxes = MakeUnique<SceneEditor::PrimitiveAxes>(gameHost,
			editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ);
		primitiveGrid = MakeUnique<SceneEditor::PrimitiveGrid>(gameHost,
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
void InGameEditor::AddUIElement(std::shared_ptr<UI::UIElement> const& element)
{
	hierarchy.AddChild(element);
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
void InGameEditor::DrawGrids(GraphicsContext & graphicsContext)
{
	POMDOG_ASSERT(primitiveGrid);
	primitiveGrid->Draw(graphicsContext, viewProjectionMatrix);
	
	POMDOG_ASSERT(primitiveAxes);
	primitiveAxes->Draw(graphicsContext, viewProjectionMatrix);
}
//-----------------------------------------------------------------------
void InGameEditor::DrawGUI()
{
	POMDOG_ASSERT(spriteBatch);
	{
		spriteBatch->Begin(SpriteSortMode::BackToFront);
		UI::SpriteDrawingContext drawingContext(*spriteBatch, *spriteBatchDistanceField, distanceFieldEffect, *spriteFont, pomdogTexture);
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
	DrawGrids(graphicsContext);
	
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
