// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/input_layout_helper.h"
#include <catch_amalgamated.hpp>

using pomdog::InputClassification;
using pomdog::InputElement;
using pomdog::InputElementFormat;
using pomdog::InputLayoutDescription;
using pomdog::InputLayoutHelper;

TEST_CASE("InputLayoutHelper", "[InputLayoutHelper]")
{
    SECTION("trivial case")
    {
        InputLayoutHelper helper;
        helper.AddInputSlot()
            .Float4()
            .Float3()
            .Float2()
            .Float();
        InputLayoutDescription inputLayout = helper.CreateInputLayout();

        auto& elements = inputLayout.InputElements;

        REQUIRE(elements.size() == 4);
        REQUIRE(elements[0].ByteOffset == 0);
        REQUIRE(elements[1].ByteOffset == 16);
        REQUIRE(elements[2].ByteOffset == 16 + 12);
        REQUIRE(elements[3].ByteOffset == 16 + 12 + 8);
        REQUIRE(elements[0].Format == InputElementFormat::Float4);
        REQUIRE(elements[1].Format == InputElementFormat::Float3);
        REQUIRE(elements[2].Format == InputElementFormat::Float2);
        REQUIRE(elements[3].Format == InputElementFormat::Float);
        REQUIRE(elements[0].InputSlot == 0);
        REQUIRE(elements[1].InputSlot == 0);
        REQUIRE(elements[2].InputSlot == 0);
        REQUIRE(elements[3].InputSlot == 0);
        REQUIRE(elements[0].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[1].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[2].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[3].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[0].InstanceStepRate == 0);
        REQUIRE(elements[1].InstanceStepRate == 0);
        REQUIRE(elements[2].InstanceStepRate == 0);
        REQUIRE(elements[3].InstanceStepRate == 0);
    }
    SECTION("AddInputSlot")
    {
        InputLayoutHelper helper;
        helper.AddInputSlot()
            .Float3()
            .Float()
            .AddInputSlot(InputClassification::InputPerInstance, 42)
            .Float4()
            .Float2()
            .AddInputSlot(InputClassification::InputPerInstance, 1)
            .Float3()
            .Float4()
            .Float2();
        InputLayoutDescription inputLayout = helper.CreateInputLayout();

        auto& elements = inputLayout.InputElements;

        REQUIRE(elements.size() == 7);
        REQUIRE(elements[0].ByteOffset == 0);
        REQUIRE(elements[1].ByteOffset == 12);
        REQUIRE(elements[2].ByteOffset == 0);
        REQUIRE(elements[3].ByteOffset == 16);
        REQUIRE(elements[4].ByteOffset == 0);
        REQUIRE(elements[5].ByteOffset == 12);
        REQUIRE(elements[6].ByteOffset == 12 + 16);
        REQUIRE(elements[0].Format == InputElementFormat::Float3);
        REQUIRE(elements[1].Format == InputElementFormat::Float);
        REQUIRE(elements[2].Format == InputElementFormat::Float4);
        REQUIRE(elements[3].Format == InputElementFormat::Float2);
        REQUIRE(elements[4].Format == InputElementFormat::Float3);
        REQUIRE(elements[5].Format == InputElementFormat::Float4);
        REQUIRE(elements[6].Format == InputElementFormat::Float2);
        REQUIRE(elements[0].InputSlot == 0);
        REQUIRE(elements[1].InputSlot == 0);
        REQUIRE(elements[2].InputSlot == 1);
        REQUIRE(elements[3].InputSlot == 1);
        REQUIRE(elements[0].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[1].InputSlotClass == InputClassification::InputPerVertex);
        REQUIRE(elements[2].InputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[3].InputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[4].InputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[5].InputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[6].InputSlotClass == InputClassification::InputPerInstance);
        REQUIRE(elements[0].InstanceStepRate == 0);
        REQUIRE(elements[1].InstanceStepRate == 0);
        REQUIRE(elements[2].InstanceStepRate == 42);
        REQUIRE(elements[3].InstanceStepRate == 42);
        REQUIRE(elements[4].InstanceStepRate == 1);
        REQUIRE(elements[5].InstanceStepRate == 1);
        REQUIRE(elements[6].InstanceStepRate == 1);
    }
}
