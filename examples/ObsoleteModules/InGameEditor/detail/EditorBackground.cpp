// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "EditorBackground.hpp"

namespace Pomdog {
namespace SceneEditor {

EditorBackground::EditorBackground(const std::shared_ptr<GameHost>& gameHost)
    : lineBatch(gameHost->GraphicsContext(), gameHost->GraphicsDevice(), *gameHost->AssetManager())
    , primitiveAxes(editorColorScheme.CenterAxisX, editorColorScheme.CenterAxisY, editorColorScheme.CenterAxisZ)
    , primitiveGrid(editorColorScheme.GuideLine, editorColorScheme.Grid)
{
}

void EditorBackground::SetViewProjection(const Matrix4x4& viewProjectionIn)
{
    viewProjectionMatrix = viewProjectionIn;
}

void EditorBackground::Draw(GraphicsContext& graphicsContext)
{
    lineBatch.Begin(viewProjectionMatrix);
    {
        primitiveGrid.Draw(lineBatch);
        primitiveAxes.Draw(lineBatch);
    }
    lineBatch.End();
}

} // namespace SceneEditor
} // namespace Pomdog
