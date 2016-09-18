// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include "Pomdog.Experimental/Graphics/SpriteBatchRenderer.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Rendering {

class SpriteBatchCommand final : public RenderCommand {
public:
    std::type_index GetType() const noexcept override;

    float GetDrawOrder() const noexcept override;

public:
    std::function<void(SpriteBatchRenderer &)> onDraw;
    float drawOrder;
};

} // namespace Rendering
} // namespace Pomdog
