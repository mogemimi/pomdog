// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/input_layout_helper.h"
#include <catch_amalgamated.hpp>

using pomdog::gpu::InputClassification;
using pomdog::gpu::InputElement;
using pomdog::gpu::InputElementFormat;
using pomdog::gpu::InputLayoutDescriptor;
using pomdog::gpu::InputLayoutHelper;

TEST_CASE("InputLayoutHelper", "[InputLayoutHelper]")
{
    SECTION("trivial case")
    {
        InputLayoutHelper helper;
        helper.addInputSlot()
            .addFloat4()
            .addFloat3()
            .addFloat2()
            .addFloat();
        InputLayoutDescriptor inputLayout = helper.createInputLayout();

        auto& elements = inputLayout.inputElements;

        REQUIRE(elements.size() == 4);
        REQUIRE(elements[0].byteOffset == 0);
        REQUIRE(elements[1].byteOffset == 16);
        REQUIRE(elements[2].byteOffset == 16 + 12);
        REQUIRE(elements[3].byteOffset == 16 + 12 + 8);
        REQUIRE(elements[0].format == InputElementFormat::Float4);
        REQUIRE(elements[1].format == InputElementFormat::Float3);
        REQUIRE(elements[2].format == InputElementFormat::Float2);
        REQUIRE(elements[3].format == InputElementFormat::Float);
        REQUIRE(elements[0].inputSlot == 0);
        REQUIRE(elements[1].inputSlot == 0);
        REQUIRE(elements[2].inputSlot == 0);
        REQUIRE(elements[3].inputSlot == 0);
        REQUIRE(elements[0].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[1].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[2].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[3].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[0].instanceStepRate == 0);
        REQUIRE(elements[1].instanceStepRate == 0);
        REQUIRE(elements[2].instanceStepRate == 0);
        REQUIRE(elements[3].instanceStepRate == 0);
    }
    SECTION("AddInputSlot")
    {
        InputLayoutHelper helper;
        helper.addInputSlot()
            .addFloat3()
            .addFloat()
            .addInputSlot(InputClassification::InputPerInstance, 42)
            .addFloat4()
            .addFloat2()
            .addInputSlot(InputClassification::InputPerInstance, 1)
            .addFloat3()
            .addFloat4()
            .addFloat2();
        InputLayoutDescriptor inputLayout = helper.createInputLayout();

        auto& elements = inputLayout.inputElements;

        REQUIRE(elements.size() == 7);
        REQUIRE(elements[0].byteOffset == 0);
        REQUIRE(elements[1].byteOffset == 12);
        REQUIRE(elements[2].byteOffset == 0);
        REQUIRE(elements[3].byteOffset == 16);
        REQUIRE(elements[4].byteOffset == 0);
        REQUIRE(elements[5].byteOffset == 12);
        REQUIRE(elements[6].byteOffset == 12 + 16);
        REQUIRE(elements[0].format == InputElementFormat::Float3);
        REQUIRE(elements[1].format == InputElementFormat::Float);
        REQUIRE(elements[2].format == InputElementFormat::Float4);
        REQUIRE(elements[3].format == InputElementFormat::Float2);
        REQUIRE(elements[4].format == InputElementFormat::Float3);
        REQUIRE(elements[5].format == InputElementFormat::Float4);
        REQUIRE(elements[6].format == InputElementFormat::Float2);
        REQUIRE(elements[0].inputSlot == 0);
        REQUIRE(elements[1].inputSlot == 0);
        REQUIRE(elements[2].inputSlot == 1);
        REQUIRE(elements[3].inputSlot == 1);
        REQUIRE(elements[0].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[1].inputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[2].inputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[3].inputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[4].inputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[5].inputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[6].inputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[0].instanceStepRate == 0);
        REQUIRE(elements[1].instanceStepRate == 0);
        REQUIRE(elements[2].instanceStepRate == 42);
        REQUIRE(elements[3].instanceStepRate == 42);
        REQUIRE(elements[4].instanceStepRate == 1);
        REQUIRE(elements[5].instanceStepRate == 1);
        REQUIRE(elements[6].instanceStepRate == 1);
    }
}
