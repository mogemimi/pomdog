// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "EditorColorScheme.hpp"
#include "PrimitiveAxes.hpp"
#include "PrimitiveGrid.hpp"
#include "Pomdog.Experimental/Graphics/LineBatch.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace SceneEditor {

class EditorBackground {
public:
    explicit EditorBackground(std::shared_ptr<GameHost> const& gameHost);

    void SetViewProjection(Matrix4x4 const& viewProjection);

    void Draw(GraphicsContext & graphicsContext);

private:
    LineBatch lineBatch;
    Matrix4x4 viewProjectionMatrix;
    SceneEditor::EditorColorScheme editorColorScheme;
    SceneEditor::PrimitiveAxes primitiveAxes;
    SceneEditor::PrimitiveGrid primitiveGrid;
};

}// namespace SceneEditor
}// namespace Pomdog
