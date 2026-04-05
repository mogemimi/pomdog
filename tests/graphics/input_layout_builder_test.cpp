// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/gpu/input_layout_builder.h"
#include "pomdog/gpu/input_layout_desc.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::gpu::InputClassification;
using pomdog::gpu::InputElementFormat;
using pomdog::gpu::InputLayoutDesc;
namespace InputLayoutBuilder = pomdog::gpu::InputLayoutBuilder;

TEST_CASE("InputLayoutBuilder")
{
    SUBCASE("addVertex single element")
    {
        InputLayoutDesc desc = {};
        InputLayoutBuilder::addVertex(desc,
            0, InputClassification::PerVertex, 4,
            {
                InputElementFormat::Float32x1,
            });

        REQUIRE(desc.vertexBuffers.size() == 1);
        REQUIRE(desc.vertexBuffers[0].elements.size() == 1);
        REQUIRE(desc.vertexBuffers[0].elements[0].format == InputElementFormat::Float32x1);
        REQUIRE(desc.vertexBuffers[0].elements[0].byteOffset == 0);
        REQUIRE(desc.vertexBuffers[0].strideBytes == 4);
        REQUIRE(desc.vertexBuffers[0].inputSlot == 0);
        REQUIRE(desc.vertexBuffers[0].inputSlotClass == InputClassification::PerVertex);
        REQUIRE(desc.vertexBuffers[0].instanceStepRate == 0);
    }
    SUBCASE("addVertex multiple elements")
    {
        InputLayoutDesc desc = {};
        InputLayoutBuilder::addVertex(desc,
            0, InputClassification::PerVertex, 32,
            {
                InputElementFormat::Float32x3,
                InputElementFormat::Float32x3,
                InputElementFormat::Float32x2,
            });

        REQUIRE(desc.vertexBuffers.size() == 1);
        auto& elements = desc.vertexBuffers[0].elements;
        REQUIRE(elements.size() == 3);
        REQUIRE(elements[0].format == InputElementFormat::Float32x3);
        REQUIRE(elements[1].format == InputElementFormat::Float32x3);
        REQUIRE(elements[2].format == InputElementFormat::Float32x2);
        REQUIRE(elements[0].byteOffset == 0);
        REQUIRE(elements[1].byteOffset == 12);
        REQUIRE(elements[2].byteOffset == 24);
    }
    SUBCASE("addVertex with byte offsets")
    {
        InputLayoutDesc desc = {};
        InputLayoutBuilder::addVertex(desc,
            0, InputClassification::PerVertex, 40,
            {
                InputElementFormat::Float32x4,
                InputElementFormat::Float32x3,
                InputElementFormat::Float32x2,
                InputElementFormat::Float32x1,
            });

        auto& elements = desc.vertexBuffers[0].elements;
        REQUIRE(elements.size() == 4);
        REQUIRE(elements[0].byteOffset == 0);
        REQUIRE(elements[1].byteOffset == 16);
        REQUIRE(elements[2].byteOffset == 16 + 12);
        REQUIRE(elements[3].byteOffset == 16 + 12 + 8);
    }
    SUBCASE("multiple vertex buffers")
    {
        InputLayoutDesc desc = {};
        InputLayoutBuilder::addVertex(desc,
            0, InputClassification::PerVertex, 16,
            {
                InputElementFormat::Float32x3,
                InputElementFormat::Float32x1,
            });
        InputLayoutBuilder::addVertex(desc,
            1, InputClassification::PerInstance, 24,
            {
                InputElementFormat::Float32x4,
                InputElementFormat::Float32x2,
            });
        InputLayoutBuilder::addVertex(desc,
            2, InputClassification::PerInstance, 28,
            {
                InputElementFormat::Float32x3,
                InputElementFormat::Float32x4,
            });

        REQUIRE(desc.vertexBuffers.size() == 3);

        REQUIRE(desc.vertexBuffers[0].inputSlot == 0);
        REQUIRE(desc.vertexBuffers[0].inputSlotClass == InputClassification::PerVertex);
        REQUIRE(desc.vertexBuffers[0].strideBytes == 16);
        REQUIRE(desc.vertexBuffers[0].elements.size() == 2);
        REQUIRE(desc.vertexBuffers[0].instanceStepRate == 0);

        REQUIRE(desc.vertexBuffers[1].inputSlot == 1);
        REQUIRE(desc.vertexBuffers[1].inputSlotClass == InputClassification::PerInstance);
        REQUIRE(desc.vertexBuffers[1].strideBytes == 24);
        REQUIRE(desc.vertexBuffers[1].elements.size() == 2);
        REQUIRE(desc.vertexBuffers[1].instanceStepRate == 1);

        REQUIRE(desc.vertexBuffers[2].inputSlot == 2);
        REQUIRE(desc.vertexBuffers[2].inputSlotClass == InputClassification::PerInstance);
        REQUIRE(desc.vertexBuffers[2].strideBytes == 28);
        REQUIRE(desc.vertexBuffers[2].elements.size() == 2);
        REQUIRE(desc.vertexBuffers[2].instanceStepRate == 1);
    }
    SUBCASE("Uint8 and Unorm8 formats")
    {
        InputLayoutDesc desc = {};
        InputLayoutBuilder::addVertex(desc,
            0, InputClassification::PerVertex, 8,
            {
                InputElementFormat::Uint8x4,
                InputElementFormat::Unorm8x4,
            });

        auto& elements = desc.vertexBuffers[0].elements;
        REQUIRE(elements.size() == 2);
        REQUIRE(elements[0].format == InputElementFormat::Uint8x4);
        REQUIRE(elements[1].format == InputElementFormat::Unorm8x4);
        REQUIRE(elements[0].byteOffset == 0);
        REQUIRE(elements[1].byteOffset == 4);
    }
}
