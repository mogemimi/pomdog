// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

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

    void AddView(std::shared_ptr<UI::UIElement> const& element);

    std::shared_ptr<UI::UIEventDispatcher> Dispatcher() const;

private:
    UI::UIElementHierarchy hierarchy;
    ScopedConnection clientSizeChangedConnection;
    SceneEditor::EditorColorScheme editorColorScheme;

    std::shared_ptr<GameHost> gameHost;

    std::shared_ptr<DepthStencilState> depthStencilState;
    std::shared_ptr<BlendState> blendState;
    std::unique_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<SpriteFont> spriteFont;
    std::unique_ptr<SpriteBatch> spriteBatchDistanceField;
    std::shared_ptr<PipelineState> distanceFieldEffect;
    std::shared_ptr<ConstantBufferBinding> constantBuffers;
    std::shared_ptr<Texture2D> blankTexture;
};

}// namespace SceneEditor
}// namespace Pomdog
