// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Graphics/PrimitiveBatch.hpp"
#include "Pomdog/Experimental/Graphics/SpriteBatch.hpp"
#include "Pomdog/Experimental/Graphics/SpriteFont.hpp"
#include "Pomdog/Experimental/Graphics/TrueTypeFont.hpp"
#include "Pomdog/Experimental/UI/FontSize.hpp"
#include "Pomdog/Experimental/UI/FontWeight.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Pomdog {
class RenderCommand;
class SpriteFont;
} // namespace Pomdog

namespace Pomdog::UI {

class DrawingContext final {
public:
    DrawingContext(
        const std::shared_ptr<GraphicsDevice>& graphicsDevice,
        AssetManager& assets);

    Matrix3x2 Top() const;

    void Push(const Matrix3x2& matrix);

    void Pop();

    void Reset(int viewportWidth, int viewportHeight);

    void PushScissorRect(const Rectangle& scissorRect);

    void PopScissorRect();

    void BeginDraw(
        const std::shared_ptr<GraphicsCommandList>& commandList,
        const Matrix4x4& transformMatrix);

    void EndDraw();

    std::shared_ptr<SpriteFont>
    GetFont(FontWeight fontWeight, FontSize fontSize) const;

    PrimitiveBatch* GetPrimitiveBatch();
    SpriteBatch* GetSpriteBatch();

private:
    std::unordered_map<std::uint32_t, std::shared_ptr<SpriteFont>> spriteFonts;
    std::shared_ptr<TrueTypeFont> fontRegular;
    std::shared_ptr<TrueTypeFont> fontBold;

    std::shared_ptr<GraphicsCommandList> commandList;
    std::shared_ptr<SpriteBatch> spriteBatch;
    std::shared_ptr<PrimitiveBatch> primitiveBatch;
    std::vector<Matrix3x2> matrixStack;
    std::vector<Rectangle> scissorRects;
    int viewportWidth;
    int viewportHeight;
};

} // namespace Pomdog::UI
