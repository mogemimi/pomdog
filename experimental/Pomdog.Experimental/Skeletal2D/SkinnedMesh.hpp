// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "SkinnedMeshPart.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class SkinnedMesh {
public:
    std::shared_ptr<Pomdog::VertexBuffer> VertexBuffer;
    std::shared_ptr<Pomdog::IndexBuffer> IndexBuffer;
    std::vector<SkinnedMeshPart> MeshParts;
};

} // namespace Pomdog
