// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include <memory>
#include <typeindex>

namespace Pomdog {

class RenderCommand {
public:
    virtual ~RenderCommand() = default;

    virtual std::type_index GetType() const noexcept = 0;

    float GetDrawOrder() const noexcept
    {
        return drawOrder;
    }

    void SetDrawOrder(float drawOrderIn) noexcept
    {
        drawOrder = drawOrderIn;
    }

private:
    float drawOrder = 0;
};

} // namespace Pomdog
