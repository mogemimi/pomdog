// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EDITORBACKGROUND_B1467252_52F8_4CA7_92A1_D4154D624DFD_HPP
#define POMDOG_EDITORBACKGROUND_B1467252_52F8_4CA7_92A1_D4154D624DFD_HPP

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
    std::shared_ptr<DepthStencilState> depthStencilState;
    std::shared_ptr<BlendState> blendState;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // !defined(POMDOG_EDITORBACKGROUND_B1467252_52F8_4CA7_92A1_D4154D624DFD_HPP)
