// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include <Pomdog/Graphics/InputLayoutHelper.hpp>
#include <gtest/iutest_switch.hpp>

using Pomdog::InputClassification;
using Pomdog::InputElement;
using Pomdog::InputElementFormat;
using Pomdog::InputLayoutDescription;
using Pomdog::InputLayoutHelper;

TEST(InputLayoutHelper, TrivialCase)
{
    InputLayoutHelper helper;
    helper.AddInputSlot()
        .Float4()
        .Float3()
        .Float2()
        .Float();
    InputLayoutDescription inputLayout = helper.CreateInputLayout();

    auto & elements = inputLayout.InputElements;

    ASSERT_EQ(4, elements.size());
    EXPECT_EQ(0, elements[0].ByteOffset);
    EXPECT_EQ(16, elements[1].ByteOffset);
    EXPECT_EQ(16+12, elements[2].ByteOffset);
    EXPECT_EQ(16+12+8, elements[3].ByteOffset);
    EXPECT_EQ(InputElementFormat::Float4, elements[0].Format);
    EXPECT_EQ(InputElementFormat::Float3, elements[1].Format);
    EXPECT_EQ(InputElementFormat::Float2, elements[2].Format);
    EXPECT_EQ(InputElementFormat::Float, elements[3].Format);
    EXPECT_EQ(0, elements[0].InputSlot);
    EXPECT_EQ(0, elements[1].InputSlot);
    EXPECT_EQ(0, elements[2].InputSlot);
    EXPECT_EQ(0, elements[3].InputSlot);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[0].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[1].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[2].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[3].InputSlotClass);
    EXPECT_EQ(0, elements[0].InstanceStepRate);
    EXPECT_EQ(0, elements[1].InstanceStepRate);
    EXPECT_EQ(0, elements[2].InstanceStepRate);
    EXPECT_EQ(0, elements[3].InstanceStepRate);
}

TEST(InputLayoutHelper, AddInputSlot)
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

    auto & elements = inputLayout.InputElements;

    ASSERT_EQ(7, elements.size());
    EXPECT_EQ(0, elements[0].ByteOffset);
    EXPECT_EQ(12, elements[1].ByteOffset);
    EXPECT_EQ(0, elements[2].ByteOffset);
    EXPECT_EQ(16, elements[3].ByteOffset);
    EXPECT_EQ(0, elements[4].ByteOffset);
    EXPECT_EQ(12, elements[5].ByteOffset);
    EXPECT_EQ(12+16, elements[6].ByteOffset);
    EXPECT_EQ(InputElementFormat::Float3, elements[0].Format);
    EXPECT_EQ(InputElementFormat::Float, elements[1].Format);
    EXPECT_EQ(InputElementFormat::Float4, elements[2].Format);
    EXPECT_EQ(InputElementFormat::Float2, elements[3].Format);
    EXPECT_EQ(InputElementFormat::Float3, elements[4].Format);
    EXPECT_EQ(InputElementFormat::Float4, elements[5].Format);
    EXPECT_EQ(InputElementFormat::Float2, elements[6].Format);
    EXPECT_EQ(0, elements[0].InputSlot);
    EXPECT_EQ(0, elements[1].InputSlot);
    EXPECT_EQ(1, elements[2].InputSlot);
    EXPECT_EQ(1, elements[3].InputSlot);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[0].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerVertex, elements[1].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerInstance, elements[2].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerInstance, elements[3].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerInstance, elements[4].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerInstance, elements[5].InputSlotClass);
    EXPECT_EQ(InputClassification::InputPerInstance, elements[6].InputSlotClass);
    EXPECT_EQ(0, elements[0].InstanceStepRate);
    EXPECT_EQ(0, elements[1].InstanceStepRate);
    EXPECT_EQ(42, elements[2].InstanceStepRate);
    EXPECT_EQ(42, elements[3].InstanceStepRate);
    EXPECT_EQ(1, elements[4].InstanceStepRate);
    EXPECT_EQ(1, elements[5].InstanceStepRate);
    EXPECT_EQ(1, elements[6].InstanceStepRate);
}
