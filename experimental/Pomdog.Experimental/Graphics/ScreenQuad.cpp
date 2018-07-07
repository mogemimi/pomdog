// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "ScreenQuad.hpp"
#include "Pomdog/Graphics/GraphicsCommandList.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>

namespace Pomdog {

ScreenQuad::ScreenQuad(const std::shared_ptr<GraphicsDevice>& graphicsDevice)
{
    struct ScreenQuadVertex {
        Vector3 Position;
        Vector2 TextureCoord;
    };

    std::array<ScreenQuadVertex, 4> verticesCombo = {{
        ScreenQuadVertex{Vector3{-1.0f, -1.0f, 0.5f}, Vector2{0.0f, 0.0f}},
        ScreenQuadVertex{Vector3{-1.0f,  1.0f, 0.5f}, Vector2{0.0f, 1.0f}},
        ScreenQuadVertex{Vector3{ 1.0f,  1.0f, 0.5f}, Vector2{1.0f, 1.0f}},
        ScreenQuadVertex{Vector3{ 1.0f, -1.0f, 0.5f}, Vector2{1.0f, 0.0f}},
    }};

    if (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL)
    {
        // Convert to Texture Coordinates in Direct3D
        for (auto & vertex: verticesCombo) {
            vertex.TextureCoord.Y = (1.0f - vertex.TextureCoord.Y);
        }
    }

    std::array<ScreenQuadVertex, 6> const vertices = {{
        verticesCombo[0], verticesCombo[1], verticesCombo[2],
        verticesCombo[2], verticesCombo[3], verticesCombo[0],
    }};

    vertexBuffer = std::make_shared<VertexBuffer>(graphicsDevice,
        vertices.data(), vertices.size(),
        sizeof(ScreenQuadVertex), BufferUsage::Immutable);
}

void ScreenQuad::DrawQuad(GraphicsCommandList & commandList)
{
    commandList.SetVertexBuffer(vertexBuffer);
    commandList.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
    commandList.Draw(vertexBuffer->GetVertexCount(), 0);
}

} // namespace Pomdog
