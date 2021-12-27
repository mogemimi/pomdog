// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/image_effects/screen_quad.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/graphics/graphics_command_list.h"
#include "pomdog/graphics/graphics_device.h"
#include "pomdog/graphics/shader_language.h"
#include "pomdog/graphics/vertex_buffer.h"
#include "pomdog/math/vector2.h"
#include "pomdog/math/vector3.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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

    if ((graphicsDevice->GetSupportedLanguage() == ShaderLanguage::HLSL) ||
        (graphicsDevice->GetSupportedLanguage() == ShaderLanguage::Metal)) {
        // Convert to Texture Coordinates in Direct3D
        for (auto& vertex : verticesCombo) {
            vertex.TextureCoord.Y = (1.0f - vertex.TextureCoord.Y);
        }
    }

    std::array<ScreenQuadVertex, 6> const vertices = {{
        verticesCombo[0], verticesCombo[1], verticesCombo[2],
        verticesCombo[2], verticesCombo[3], verticesCombo[0],
    }};

    vertexBuffer = std::get<0>(graphicsDevice->CreateVertexBuffer(
        vertices.data(),
        vertices.size(),
        sizeof(ScreenQuadVertex),
        BufferUsage::Immutable));
}

void ScreenQuad::DrawQuad(GraphicsCommandList& commandList)
{
    commandList.SetVertexBuffer(0, vertexBuffer);
    commandList.Draw(vertexBuffer->GetVertexCount(), 0);
}

} // namespace pomdog
