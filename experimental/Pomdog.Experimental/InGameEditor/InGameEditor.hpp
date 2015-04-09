// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_INGAMEEDITOR_9210CCAC_HPP
#define POMDOG_INGAMEEDITOR_9210CCAC_HPP

#include "detail/EditorColorScheme.hpp"
#include "Pomdog.Experimental/UI/UIElementHierarchy.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {

class SpriteBatch;
class SpriteFont;

}// namespace Pomdog

namespace Pomdog {
namespace SceneEditor {

class InGameEditor {
public:
    explicit InGameEditor(std::shared_ptr<GameHost> const& gameHost);

    ~InGameEditor();

    void Update();

    void DrawGUI(GraphicsContext & graphicsContext);

    void AddView(std::shared_ptr<UI::UIView> const& element);

private:
    UI::UIElementHierarchy hierarchy;
    ScopedConnection clientSizeChangedConnection;
    SceneEditor::EditorColorScheme editorColorScheme;

    std::shared_ptr<GameHost> gameHost;

    std::unique_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::unique_ptr<SpriteBatch> spriteBatchDistanceField;
    std::shared_ptr<PipelineState> distanceFieldEffect;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
    std::shared_ptr<Texture2D> blankTexture;
};

}// namespace SceneEditor
}// namespace Pomdog

#endif // POMDOG_INGAMEEDITOR_9210CCAC_HPP
