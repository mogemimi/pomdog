// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/screen_quad.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/command_list.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

ScreenQuad::ScreenQuad(const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice)
{
    struct ScreenQuadVertex {
        Vector3 Position;
        Vector2 TextureCoord;
    };

    std::array<ScreenQuadVertex, 4> verticesCombo = {{
        ScreenQuadVertex{Vector3{-1.0f, -1.0f, 0.5f}, Vector2{0.0f, 0.0f}},
        ScreenQuadVertex{Vector3{-1.0f, 1.0f, 0.5f}, Vector2{0.0f, 1.0f}},
        ScreenQuadVertex{Vector3{1.0f, 1.0f, 0.5f}, Vector2{1.0f, 1.0f}},
        ScreenQuadVertex{Vector3{1.0f, -1.0f, 0.5f}, Vector2{1.0f, 0.0f}},
    }};

    if ((graphicsDevice->getSupportedLanguage() == gpu::ShaderLanguage::HLSL) ||
        (graphicsDevice->getSupportedLanguage() == gpu::ShaderLanguage::Metal)) {
        // Convert to Texture Coordinates in Direct3D
        for (auto& vertex : verticesCombo) {
            vertex.TextureCoord.y = (1.0f - vertex.TextureCoord.y);
        }
    }

    std::array<ScreenQuadVertex, 6> const vertices = {{
        verticesCombo[0],
        verticesCombo[1],
        verticesCombo[2],
        verticesCombo[2],
        verticesCombo[3],
        verticesCombo[0],
    }};

    vertexBuffer = std::get<0>(graphicsDevice->createVertexBuffer(
        vertices.data(),
        vertices.size(),
        sizeof(ScreenQuadVertex),
        gpu::BufferUsage::Immutable));
}

void ScreenQuad::DrawQuad(gpu::CommandList& commandList)
{
    commandList.setVertexBuffer(0, vertexBuffer);
    commandList.draw(vertexBuffer->getVertexCount(), 0);
}

} // namespace pomdog
