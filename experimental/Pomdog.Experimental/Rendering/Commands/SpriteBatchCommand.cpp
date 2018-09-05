// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Rendering/Commands/SpriteBatchCommand.hpp"
#include <typeinfo>

namespace Pomdog {
namespace Rendering {

std::type_index SpriteRenderCommand::GetType() const noexcept
{
    static const std::type_index index = typeid(SpriteBatchCommand);
    return index;
}

void SpriteBatchCommand::Execute(SpriteBatch& spriteBatch)
{
    spriteBatch.Draw(texture, position, sourceRect, color, rotation, originPivot, scale);
}

void SpriteFontCommand::Execute(SpriteBatch& spriteBatch)
{
    POMDOG_ASSERT(spriteFont);
    spriteFont->Draw(spriteBatch, text, position, color, rotation, Vector2{0.0f, 0.0f}, scale);
}

} // namespace Rendering
} // namespace Pomdog
