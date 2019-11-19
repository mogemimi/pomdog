// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <cstddef>
#include <memory>

namespace Pomdog::Detail::GL4 {

struct VertexBufferBindingGL4 final {
    std::shared_ptr<Pomdog::VertexBuffer> VertexBuffer;
    std::size_t VertexOffset = 0;
};

} // namespace Pomdog::Detail::GL4
