// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/gpu/input_element_format.h"
#include "pomdog/gpu/input_layout_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::gpu::InputClassification;
using pomdog::gpu::InputElementFormat;
using pomdog::gpu::InputLayoutDesc;
using pomdog::gpu::InputLayoutHelper;

TEST_CASE("InputLayoutHelper")
{
    SUBCASE("addFloat")
    {
        InputLayoutHelper helper;
        helper.addFloat();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 1);
        REQUIRE(desc.inputElements[0].format == InputElementFormat::Float);
    }
    SUBCASE("addFloat2")
    {
        InputLayoutHelper helper;
        helper.addFloat2();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 1);
        REQUIRE(desc.inputElements[0].format == InputElementFormat::Float2);
    }
    SUBCASE("addFloat3")
    {
        InputLayoutHelper helper;
        helper.addFloat3();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 1);
        REQUIRE(desc.inputElements[0].format == InputElementFormat::Float3);
    }
    SUBCASE("addFloat4")
    {
        InputLayoutHelper helper;
        helper.addFloat4();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 1);
        REQUIRE(desc.inputElements[0].format == InputElementFormat::Float4);
    }
    SUBCASE("add multiple elements")
    {
        InputLayoutHelper helper;
        helper.addFloat3(); // e.g. position
        helper.addFloat3(); // e.g. normal
        helper.addFloat2(); // e.g. texcoord

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 3);
        REQUIRE(desc.inputElements[0].format == InputElementFormat::Float3);
        REQUIRE(desc.inputElements[1].format == InputElementFormat::Float3);
        REQUIRE(desc.inputElements[2].format == InputElementFormat::Float2);
    }
    SUBCASE("addInputSlot")
    {
        InputLayoutHelper helper;
        helper.addInputSlot();
        helper.addFloat3();
        helper.addFloat2();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 2);
    }
    SUBCASE("addInputSlot with InputClassification")
    {
        InputLayoutHelper helper;
        helper.addInputSlot(InputClassification::InputPerVertex, 0);
        helper.addFloat3();
        helper.addFloat2();

        auto desc = helper.createInputLayout();
        REQUIRE(desc.inputElements.size() == 2);
        REQUIRE(desc.inputElements[0].inputSlotClass == InputClassification::InputPerVertex);
    }
    SUBCASE("verify byte offsets and other properties")
    {
        InputLayoutHelper helper;
        helper.addInputSlot();
        helper.addFloat4();
        helper.addFloat3();
        helper.addFloat2();
        helper.addFloat();

        auto desc = helper.createInputLayout();
        auto& elements = desc.inputElements;

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
    SUBCASE("verify complex layout")
    {
        InputLayoutHelper helper;
        helper.addInputSlot();
        helper.addFloat3();
        helper.addFloat();
        helper.addInputSlot(InputClassification::InputPerInstance, 42);
        helper.addFloat4();
        helper.addFloat2();
        helper.addInputSlot(InputClassification::InputPerInstance, 1);
        helper.addFloat3();
        helper.addFloat4();
        helper.addFloat2();

        auto desc = helper.createInputLayout();
        auto& elements = desc.inputElements;

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
