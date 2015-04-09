// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "EditorBackground.hpp"

namespace Pomdog {
namespace SceneEditor {
//-----------------------------------------------------------------------
EditorBackground::EditorBackground(std::shared_ptr<GameHost> const& gameHost)
    : lineBatch(gameHost->GraphicsContext(), gameHost->GraphicsDevice(), *gameHost->AssetManager())
    , primitiveAxes(editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ)
    , primitiveGrid(editorColorScheme.GuideLine, editorColorScheme.Grid)
{}
//-----------------------------------------------------------------------
void EditorBackground::SetViewProjection(Matrix4x4 const& viewProjectionIn)
{
    viewProjectionMatrix = viewProjectionIn;
}
//-----------------------------------------------------------------------
void EditorBackground::Draw(GraphicsContext & graphicsContext)
{
    lineBatch.Begin(viewProjectionMatrix);
    {
        primitiveGrid.Draw(lineBatch);
        primitiveAxes.Draw(lineBatch);
    }
    lineBatch.End();
}
//-----------------------------------------------------------------------
}// namespace SceneEditor
}// namespace Pomdog
