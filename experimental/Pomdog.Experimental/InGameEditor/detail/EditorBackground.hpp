// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_EDITORBACKGROUND_B1467252_HPP
#define POMDOG_EDITORBACKGROUND_B1467252_HPP

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

#endif // POMDOG_EDITORBACKGROUND_B1467252_HPP
