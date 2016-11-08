// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include <memory>

namespace Pomdog {

class RenderCommand;
class RenderCommandProcessor;

class Renderer final {
public:
    explicit Renderer(const std::shared_ptr<GraphicsDevice>& graphicsDevice);

    ~Renderer();

    void SetViewMatrix(const Matrix4x4& viewMatrix);

    void SetProjectionMatrix(const Matrix4x4& projectionMatrix);

    std::shared_ptr<GraphicsCommandList> Render();

    void PushCommand(std::reference_wrapper<RenderCommand> && command);

    void Reset();

    void AddProcessor(std::unique_ptr<RenderCommandProcessor> && processor);

    int GetDrawCallCount() const noexcept;

public:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
