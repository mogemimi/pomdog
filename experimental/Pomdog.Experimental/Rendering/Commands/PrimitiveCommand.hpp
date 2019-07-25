// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Rendering/RenderCommand.hpp"
#include <Pomdog/Pomdog.hpp>
#include <memory>

namespace Pomdog {
namespace Rendering {

class PrimitiveCommand final : public RenderCommand {
public:
    std::type_index GetType() const noexcept override;

public:
    std::shared_ptr<VertexBuffer> vertexBuffer;
    std::shared_ptr<ConstantBuffer> constantBuffer;
    std::size_t vertexCount;
    Matrix4x4 worldMatrix;
};

} // namespace Rendering
} // namespace Pomdog
