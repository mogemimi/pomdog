// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKINNEDMESH_4A54231B_HPP
#define POMDOG_SKINNEDMESH_4A54231B_HPP

#include "SkinnedMeshPart.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include <memory>
#include <vector>

namespace Pomdog {

class SkinnedMesh {
public:
    std::shared_ptr<VertexBuffer> VertexBuffer;
    std::shared_ptr<IndexBuffer> IndexBuffer;
    std::vector<SkinnedMeshPart> MeshParts;
};

} // namespace Pomdog

#endif // POMDOG_SKINNEDMESH_4A54231B_HPP
