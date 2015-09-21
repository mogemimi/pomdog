// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include <cstdint>
#include <typeindex>
#include <memory>

namespace Pomdog {

class RenderCommand;
class RenderCommandProcessor;

class Renderer {
public:
    Renderer();

    ~Renderer();

    void SetViewMatrix(Matrix4x4 const& viewMatrix);

    void SetProjectionMatrix(Matrix4x4 const& projectionMatrix);

    void Render(GraphicsCommandQueue & commandQueue);

    void PushCommand(std::reference_wrapper<RenderCommand> && command);

    void Clear();

    void AddProcessor(std::type_index const& index, std::unique_ptr<RenderCommandProcessor> && processor);

    std::uint32_t DrawCallCount() const;

public:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
