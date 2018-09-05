// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/Renderer.hpp"
#include "Pomdog.Experimental/UI/FontSize.hpp"
#include "Pomdog.Experimental/UI/FontWeight.hpp"
#include "Pomdog/Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog/Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog/Experimental/Graphics/TrueTypeFont.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Pomdog {

class RenderCommand;
class SpriteFont;

} // namespace Pomdog

namespace Pomdog {
namespace UI {

class DrawingContext final {
private:
    Renderer renderer;
    std::unordered_map<std::string, std::shared_ptr<SpriteFont>> spriteFonts;
    std::shared_ptr<TrueTypeFont> fontRegular;
    std::shared_ptr<TrueTypeFont> fontBold;

    std::shared_ptr<GraphicsDevice> graphicsDevice;
    std::vector<Matrix3x2> matrixStack;
    int viewportWidth;
    int viewportHeight;

public:
    DrawingContext(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager & assets);

    Matrix3x2 Top() const;

    void Push(const Matrix3x2& matrix);

    void Pop();

    std::shared_ptr<GraphicsCommandList> Render();

    void Reset(int viewportWidth, int viewportHeight);

    void PushCommand(std::reference_wrapper<RenderCommand> && command);

    std::shared_ptr<SpriteFont> GetFont(FontWeight fontWeight, FontSize fontSize);
};

} // namespace UI
} // namespace Pomdog
